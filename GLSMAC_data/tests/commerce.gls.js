const define_economy = #include('../default/game/economy');
const technologies = #include('../default/technologies');
const economy_ui = #include('../default/ui/parts/game/popup/base_screen/economy');

test.assert(technologies.get_total_commerce_bonus() == 6);
test.assert(#typeof(economy_ui.set_commerce) == 'Callable');

const economic_technology_ids = [
	'IndustrialEconomics',
	'IndustrialAutomation',
	'EnvironmentalEconomics',
	'PlanetaryEconomics',
	'IndustrialNanorobotics',
	'SentientEconometrics',
];
for (id of economic_technology_ids) {
	test.assert(technologies.get_definition(id).commerce_bonus == 1);
}

const make_player = (id, name, known) => {
	let relations = {};
	let sanction_turns = 0;
	const faction = {is_progenitor: false};
	return {
		id: id,
		name: name,
		commerce_bonus: 0,
		get_faction: () => { return faction; },
		set_progenitor: (value) => { faction.is_progenitor = value; },
		get_research_state: () => { return {technologies: known}; },
		get_diplomatic_relation: (other) => {
			const key = 'p' + #to_string(other.id);
			return #is_defined(relations[key]) ? relations[key] : 'neutral';
		},
		set_relation: (other, relation) => {
			relations['p' + #to_string(other.id)] = relation;
		},
		get_sanction_turns: () => { return sanction_turns; },
		set_sanction_turns: (turns) => { sanction_turns = turns; },
	};
};

const alpha = make_player(1, 'Alpha', economic_technology_ids);
const beta = make_player(2, 'Beta', []);
const make_base = (id, owner, energy) => {
	return {
		id: id,
		get_owner: () => { return owner; },
		get_tile: () => { return {id: id}; },
		get_intake: () => { return {ENERGY: energy}; },
		get_consumption: () => { return {ENERGY: 0}; },
		get_facilities: () => { return []; },
		has_facility: (facility_id) => { return facility_id == 'Headquarters'; },
	};
};

const alpha_high = make_base(1, alpha, 80);
const alpha_low = make_base(2, alpha, 40);
const alpha_unmatched = make_base(3, alpha, 8);
const beta_high = make_base(4, beta, 64);
const beta_low = make_base(5, beta, 24);
const bases = [alpha_low, beta_low, alpha_unmatched, beta_high, alpha_high];

let callbacks = {};
let governor_id = -1;
let global_trade_pact = false;
let values = {
	f_technology_get_base_labs: (base) => {
		throw Error('commerce ranking must not resolve lab bonuses');
	},
	f_technology_get_base_labs_value: (base) => { return 0; },
	f_technology_get_definition: technologies.get_definition,
	f_technology_get_total_commerce_bonus: technologies.get_total_commerce_bonus,
	f_social_get_ratings: (player) => { return {effic: 0}; },
	f_social_get_commerce_bonus: (player) => { return player.commerce_bonus; },
	f_council_is_governor: (player) => { return player.id == governor_id; },
	f_council_has_global_trade_pact: () => { return global_trade_pact; },
};
const game = {
	on: (name, callback) => { callbacks[name] = callback; },
	set: (name, value) => { values[name] = value; },
	get: (name) => { return values[name]; },
	get_bm: () => { return {get_bases: () => { return bases; }}; },
	get_tm: () => { return {get_distance: (from, to) => { return 0; }}; },
	get_players: () => { return [alpha, beta]; },
};
define_economy(game);
callbacks.start({});
test.assert(#is_defined(values.f_economy_get_player_commerce_ledger));

alpha.set_relation(beta, 'pact');
beta.set_relation(alpha, 'pact');
let commerce = values.f_economy_get_base_commerce(game, alpha_high);
test.assert(commerce.total == 15);
test.assert(commerce.partners == [{
	player_id: 2, player_name: 'Beta', relation: 'pact', value: 15,
}]);
test.assert(values.f_economy_get_base_commerce(game, alpha_low).total == 7);
test.assert(values.f_economy_get_base_commerce(game, alpha_unmatched).total == 0);
const alpha_ledger = values.f_economy_get_player_commerce_ledger(game, alpha);
test.assert(alpha_ledger.b1.total == 15);
test.assert(values.f_economy_get_player_commerce(game, alpha) == 22);
test.assert(values.f_economy_get_player_commerce(game, beta) == 3);
test.assert(values.f_economy_get_player(game, alpha) == 124);

global_trade_pact = true;
test.assert(values.f_economy_get_base_commerce(game, alpha_high).total == 30);
test.assert(values.f_economy_get_player_commerce(game, alpha) == 44);
test.assert(values.f_economy_get_player_commerce(game, beta) == 6);
global_trade_pact = false;

governor_id = alpha.id;
test.assert(values.f_economy_get_player_commerce(game, alpha) == 24);
test.assert(values.f_economy_get_base_commerce(game, alpha_high).total == 16);
test.assert(values.f_economy_get_base_commerce(game, alpha_low).total == 8);
governor_id = -1;

alpha.set_sanction_turns(10);
test.assert(values.f_economy_get_player_commerce(game, alpha) == 0);
test.assert(values.f_economy_get_player_commerce(game, beta) == 0);
alpha.set_sanction_turns(0);
beta.set_sanction_turns(10);
test.assert(values.f_economy_get_player_commerce(game, alpha) == 0);
test.assert(values.f_economy_get_player_commerce(game, beta) == 0);
beta.set_sanction_turns(0);

alpha.set_relation(beta, 'treaty');
beta.set_relation(alpha, 'treaty');
test.assert(values.f_economy_get_player_commerce(game, alpha) == 10);
test.assert(values.f_economy_get_player_commerce(game, beta) == 1);

alpha.commerce_bonus = 1;
test.assert(values.f_economy_get_player_commerce(game, alpha) == 12);
alpha.commerce_bonus = 0;

alpha.set_relation(beta, 'neutral');
beta.set_relation(alpha, 'neutral');
test.assert(values.f_economy_get_player_commerce(game, alpha) == 0);

alpha.set_relation(beta, 'pact');
test.assert(values.f_economy_get_player_commerce(game, alpha) == 0);

beta.set_relation(alpha, 'pact');
beta.set_progenitor(true);
test.assert(values.f_economy_get_player_commerce(game, alpha) == 0);
test.assert(values.f_economy_get_player_commerce(game, beta) == 0);
