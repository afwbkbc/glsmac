const define_economy = #include('../default/game/economy');
const process_player_economy = #include('../default/game/event/process_player_economy');
const settle_player_economy = #include('../default/game/event/settle_player_economy');
const hurry_base_production = #include('../default/game/event/hurry_base_production');
const liquidate_base_facility = #include('../default/game/event/liquidate_base_facility');

let player = null;
let player_sanction_turns = 0;
player = {
	id: 1,
	energy_credits: 5,
	set_energy_credits: (value) => { player.energy_credits = value; },
	get_research_state: () => { return {technologies: []}; },
	get_faction: () => { return {is_progenitor: false}; },
	get_diplomatic_relation: (other) => { return 'neutral'; },
	get_sanction_turns: () => { return player_sanction_turns; },
};
let other_player = null;
other_player = {
	id: 2,
	energy_credits: 0,
	set_energy_credits: (value) => { other_player.energy_credits = value; },
	get_research_state: () => { return {technologies: []}; },
	get_faction: () => { return {is_progenitor: false}; },
	get_diplomatic_relation: (other) => { return 'neutral'; },
	get_sanction_turns: () => { return 0; },
};
let positive_economy_multiplier = 0.0;
let positive_psych_multiplier = 0.0;
let stockpiling_energy = false;
let stockpile_minerals = 7;
const stockpile_energy = {
	production_kind: 'facility',
	mineral_to_energy_divisor: 2,
};
const positive_tile = {distance: 0};
const deficit_tile = {distance: 1};
const positive_base = {
	id: 1,
	get_owner: () => { return player; },
	get_tile: () => { return positive_tile; },
	get_intake: () => { return {ENERGY: 10, MINERALS: 7}; },
	get_consumption: () => { return {ENERGY: 0, MINERALS: 0}; },
	get_production: () => { return stockpiling_energy ? stockpile_energy : #undefined; },
	has_facility: (id) => { return id == 'Headquarters'; },
	get_facilities: () => { return [{
		psych_bonus: 4,
		psych_multiplier: positive_psych_multiplier,
		economy_multiplier: positive_economy_multiplier,
	}]; },
};
const deficit_base = {
	id: 2,
	get_owner: () => { return player; },
	get_tile: () => { return deficit_tile; },
	get_intake: () => { return {ENERGY: 2, MINERALS: 0}; },
	get_consumption: () => { return {ENERGY: 5, MINERALS: 0}; },
	get_production: () => { return #undefined; },
	get_facilities: () => { return []; },
	has_facility: (id) => { return false; },
};

let callbacks = {};
let efficiency_rating = 0;
let project_economy_multiplier = 0.0;
let values = {
	f_technology_get_base_labs: (base) => {
		return {
			allocation: 0.4,
			value: base == positive_base ? 4 : 0,
			bonus: 2,
			total: base == positive_base ? 6 : 2,
		};
	},
	f_social_get_ratings: (owner) => { return {effic: efficiency_rating}; },
	f_social_get_commerce_bonus: (owner) => { return 0; },
	f_technology_get_definition: (id) => { return null; },
	f_technology_get_total_commerce_bonus: () => { return 6; },
	f_project_get_effects: (base) => {
		return {economy_multiplier: project_economy_multiplier};
	},
	f_base_get_pending_production: (base) => {
		return base.id == positive_base.id
			? stockpile_minerals
			: #max(base.get_intake().MINERALS - base.get_consumption().MINERALS, 0);
	},
};
let events = [];
let economy_bases = [positive_base, deficit_base];
const game = {
	on: (name, callback) => { callbacks[name] = callback; },
	set: (key, value) => { values[key] = value; },
	get: (key) => { return values[key]; },
	get_bm: () => { return {get_bases: () => { return economy_bases; }}; },
	get_tm: () => { return {get_distance: (from, to) => { return from.distance; }}; },
	get_players: () => { return [player, other_player]; },
	is_master: () => { return true; },
	event: (name, data) => { events :+{name: name, data: data}; },
};

define_economy(game);
callbacks.start({});
test.assert(values.f_economy_get_base(game, positive_base) == 4);
test.assert(values.f_economy_get_base(game, deficit_base) == 0 - 3);
test.assert(values.f_economy_get_player(game, player) == 1);
stockpiling_energy = true;
test.assert(values.f_economy_get_base_stockpile_energy(game, positive_base) == 3);
test.assert(values.f_economy_get_player(game, player) == 4);
stockpile_minerals = 0 - 1;
test.assert(values.f_economy_get_base_stockpile_energy(game, positive_base) == 0);
stockpile_minerals = 7;
stockpiling_energy = false;
const deficit_allocation = values.f_economy_get_base_allocation(game, deficit_base);
test.assert(deficit_allocation.economy.value == 0 - 3);
test.assert(deficit_allocation.labs.value == 0);
test.assert(deficit_allocation.psych.value == 0);
const positive_allocation = values.f_economy_get_base_allocation(game, positive_base);
test.assert(positive_allocation.psych.value == 2);
test.assert(positive_allocation.psych.bonus == 4);
test.assert(values.f_economy_get_base_psych(game, positive_base) == 6);
positive_psych_multiplier = 0.5;
test.assert(values.f_economy_get_base_psych(game, positive_base) == 7);
positive_psych_multiplier = 0.0;
positive_economy_multiplier = 0.5;
const bank_allocation = values.f_economy_get_base_allocation(game, positive_base);
test.assert(bank_allocation.economy.value == 4);
test.assert(bank_allocation.economy.bonus == 2);
test.assert(values.f_economy_get_base(game, positive_base) == 6);
positive_economy_multiplier = 0.0;
project_economy_multiplier = 0.5;
const vaccine_allocation = values.f_economy_get_base_allocation(game, positive_base);
test.assert(vaccine_allocation.economy.value == 4);
test.assert(vaccine_allocation.economy.bonus == 2);
test.assert(values.f_economy_get_base(game, positive_base) == 6);
project_economy_multiplier = 0.0;

let has_creche = false;
const creche = {efficiency_rating_bonus: 2};
const remote_tile = {distance: 8};
const remote_base = {
	id: 3,
	get_owner: () => { return player; },
	get_tile: () => { return remote_tile; },
	get_intake: () => { return {ENERGY: 32}; },
	has_facility: (id) => {
		return id == 'ChildrenSCreche' && has_creche;
	},
	get_facilities: () => { return has_creche ? [creche] : []; },
};
economy_bases = [positive_base, remote_base];
let energy = values.f_economy_get_base_energy(remote_base);
test.assert(energy == {
	gross: 32, inefficiency: 8, net: 24,
	efficiency: 0, distance: 8, denominator: 32,
});
efficiency_rating = 0 - 4;
energy = values.f_economy_get_base_energy(remote_base);
test.assert(energy.inefficiency == 32);
test.assert(energy.net == 0);
test.assert(energy.denominator == 0);
has_creche = true;
energy = values.f_economy_get_base_energy(remote_base);
test.assert(energy.efficiency == 0 - 2);
test.assert(energy.inefficiency == 16);
test.assert(energy.net == 16);
test.assert(energy.denominator == 16);
has_creche = false;
efficiency_rating = 4;
energy = values.f_economy_get_base_energy(remote_base);
test.assert(energy.inefficiency == 4);
test.assert(energy.net == 28);
efficiency_rating = 0;
economy_bases = [remote_base];
energy = values.f_economy_get_base_energy(remote_base);
test.assert(energy.distance == 16);
test.assert(energy.inefficiency == 16);
economy_bases = [positive_base, deficit_base];

let hurry_minerals = 10;
let hurry_production = {production_kind: 'unit', mineral_cost: 20};
const hurry_base = {
	get_owner: () => { return player; },
	get_production: () => { return hurry_production; },
	get_accumulated_minerals: () => { return hurry_minerals; },
	set_accumulated_minerals: (value) => { hurry_minerals = value; },
};
test.assert(values.f_economy_get_hurry_cost(hurry_base) == 25);
hurry_minerals = 0;
test.assert(values.f_economy_get_hurry_cost(hurry_base) == 120);
hurry_minerals = 10;
hurry_production = {production_kind: 'facility', mineral_cost: 40};
test.assert(values.f_economy_get_hurry_cost(hurry_base) == 60);
hurry_minerals = 40;
test.assert(values.f_economy_get_hurry_cost(hurry_base) == 0);

callbacks.turn({});
test.assert(#sizeof(events) == 2);
test.assert(events[0].name == 'settle_player_economy');
test.assert(events[1].name == 'settle_player_economy');
for (let settlement_index = 0; settlement_index < 2; settlement_index++) {
	const settlement = {caller: 0, game: game, data: events[settlement_index].data};
	test.assert(!#is_defined(settle_player_economy.validate(settlement)));
	settle_player_economy.apply(settlement);
}
test.assert(#sizeof(events) == 4);
test.assert(events[2].name == 'process_player_economy');
test.assert(events[2].data.player == player);
test.assert(events[2].data.energy_credits == 6);
test.assert(events[3].data.energy_credits == 0);

events = [];
player_sanction_turns = 1;
callbacks.turn({});
test.assert(#sizeof(events) == 3);
test.assert(events[0].name == 'settle_player_economy');
test.assert(events[1].name == 'settle_player_economy');
test.assert(events[2].name == 'process_diplomatic_sanctions');
player_sanction_turns = 0;

let trigger_count = 0;
const event_game = {trigger: (name, data) => {
	test.assert(name == 'economy_updated' && data.player.id == player.id);
	trigger_count++;
}};
let event = {
	caller: 0,
	game: event_game,
	data: {player: player, energy_credits: 12},
};
test.assert(!#is_defined(process_player_economy.validate(event)));
event.applied = process_player_economy.apply(event);
test.assert(player.energy_credits == 12);
test.assert(event.applied.energy_credits == 5);
process_player_economy.rollback(event);
test.assert(player.energy_credits == 5);
test.assert(trigger_count == 2);

event.caller = 1;
test.assert(#is_defined(process_player_economy.validate(event)));
event.caller = 0;
event.data.energy_credits = 0 - 1;
test.assert(#is_defined(process_player_economy.validate(event)));
event.data.energy_credits = 1000000001;
test.assert(#is_defined(process_player_economy.validate(event)));

player.energy_credits = 100;
hurry_minerals = 10;
hurry_production = {production_kind: 'unit', mineral_cost: 20};
let hurry_trigger_count = 0;
const hurry_game = {
	is_turn_complete: (player_id) => { return false; },
	get: (key) => { return values[key]; },
	trigger: (name, data) => { hurry_trigger_count++; },
};
let hurry_event = {caller: 1, game: hurry_game, data: {base: hurry_base}};
test.assert(!#is_defined(hurry_base_production.validate(hurry_event)));
hurry_event.applied = hurry_base_production.apply(hurry_event);
test.assert(player.energy_credits == 75);
test.assert(hurry_minerals == 20);
test.assert(hurry_event.applied.energy_credits == 100);
test.assert(hurry_event.applied.minerals == 10);
hurry_base_production.rollback(hurry_event);
test.assert(player.energy_credits == 100);
test.assert(hurry_minerals == 10);
test.assert(hurry_trigger_count == 2);

hurry_event.caller = 2;
test.assert(#is_defined(hurry_base_production.validate(hurry_event)));
hurry_event.caller = 1;
player.energy_credits = 24;
test.assert(#is_defined(hurry_base_production.validate(hurry_event)));

let poor_has_node = true;
let poor_player = null;
poor_player = {
	id: 3,
	energy_credits: 0,
	set_energy_credits: (value) => { poor_player.energy_credits = value; },
	get_research_state: () => { return {technologies: []}; },
	get_faction: () => { return {is_progenitor: false}; },
	get_diplomatic_relation: (other) => { return 'neutral'; },
	get_sanction_turns: () => { return 0; },
};
const network_node = {id: 'NetworkNode', energy_maintenance: 1, psych_bonus: 0};
const poor_base = {
	id: 9,
	get_owner: () => { return poor_player; },
	get_tile: () => { return {distance: 0}; },
	get_intake: () => { return {ENERGY: 0}; },
	get_consumption: () => { return {ENERGY: poor_has_node ? 2 : 1}; },
	get_facilities: () => { return poor_has_node ? [network_node] : []; },
	get_pops: () => { return []; },
	has_facility: (id) => { return poor_has_node && id == 'NetworkNode'; },
	remove_facility: (id) => { poor_has_node = false; },
	add_facility: (id) => { poor_has_node = true; },
};
let poor_callbacks = {};
let poor_values = {
	f_technology_get_base_labs: (base) => {
		return {allocation: 0.4, value: 0, bonus: 0, total: 0};
	},
	f_economy_get_base_psych: (game, base) => { return 0; },
	f_base_process_psych: (game, base, psych) => {},
	f_social_get_commerce_bonus: (owner) => { return 0; },
	f_technology_get_definition: (id) => { return null; },
	f_technology_get_total_commerce_bonus: () => { return 6; },
};
let poor_events = [];
let poor_game = null;
poor_game = {
	on: (name, callback) => { poor_callbacks[name] = callback; },
	set: (key, value) => { poor_values[key] = value; },
	get: (key) => { return poor_values[key]; },
	get_bm: () => { return {
		get_bases: () => { return [poor_base]; },
		get_facility_def: (id) => { return network_node; },
	}; },
	get_players: () => { return [poor_player]; },
	get_tm: () => { return {get_distance: (from, to) => { return 0; }}; },
	is_master: () => { return true; },
	trigger: (name, data) => {
		test.assert(name == 'economy_updated' && data.player == poor_player);
	},
	event: (name, data) => {
		poor_events :+{name: name, data: data};
	},
};
define_economy(poor_game);
poor_callbacks.start({});
poor_callbacks.turn({});

let poor_event_index = 0;
while (poor_event_index < #sizeof(poor_events)) {
	const queued = poor_events[poor_event_index];
	const queued_event = {caller: 0, game: poor_game, data: queued.data};
	if (queued.name == 'settle_player_economy') {
		test.assert(!#is_defined(settle_player_economy.validate(queued_event)));
		settle_player_economy.apply(queued_event);
	} else if (queued.name == 'liquidate_base_facility') {
		test.assert(!#is_defined(liquidate_base_facility.validate(queued_event)));
		liquidate_base_facility.apply(queued_event);
	} else if (queued.name == 'process_player_economy') {
		test.assert(!#is_defined(process_player_economy.validate(queued_event)));
		process_player_economy.apply(queued_event);
	} else {
		throw Error('Unexpected queued economy event: ' + queued.name);
	}
	poor_event_index++;
}

test.assert(!poor_has_node);
test.assert(#sizeof(poor_events) == 4);
test.assert(poor_events[0].name == 'settle_player_economy');
test.assert(poor_events[1].name == 'liquidate_base_facility');
test.assert(poor_events[2].name == 'settle_player_economy');
test.assert(poor_events[3].name == 'process_player_economy');
test.assert(poor_events[3].data.energy_credits == 0);

poor_has_node = true;
let liquidation = {
	caller: 0,
	game: poor_game,
	data: {base: poor_base, facility_id: 'NetworkNode'},
};
test.assert(!#is_defined(liquidate_base_facility.validate(liquidation)));
liquidation.applied = liquidate_base_facility.apply(liquidation);
test.assert(!poor_has_node);
liquidate_base_facility.rollback(liquidation);
test.assert(poor_has_node);
liquidation.caller = 1;
test.assert(#is_defined(liquidate_base_facility.validate(liquidation)));
