const define_social_engineering = #include('../default/game/social_engineering');
const set_social_engineering = #include('../default/game/event/set_social_engineering');

const callbacks = {};
const values = {};
let bases = [];
let triggers = [];
const game = {
	on: (name, callback) => { callbacks[name] = callback; },
	set: (name, value) => { values[name] = value; },
	get: (name) => { return values[name]; },
	get_bm: () => { return {get_bases: () => { return bases; }}; },
	is_turn_complete: (player_id) => { return false; },
	trigger: (name, data) => { triggers :+{name: name, data: data}; },
};
define_social_engineering(game);
callbacks.start({});

let choices = {
	politics: 'Frontier',
	economics: 'Simple',
	values: 'Survival',
	future_society: 'None',
};
let technologies = {};
let faction_id = 'GAIANS';
let energy_credits = 100;
const player = {
	id: 1,
	difficulty_level: 'Transcend',
	get_social_engineering: () => { return choices; },
	set_social_engineering: (value) => { choices = value; },
	get_energy_credits: () => { return energy_credits; },
	set_energy_credits: (value) => { energy_credits = value; },
	get_faction: () => { return {id: faction_id}; },
	has_technology: (id) => { return #is_defined(technologies[id]); },
};

const categories = values.f_social_get_categories();
test.assert(#sizeof(categories) == 4);
for (category of categories) {
	test.assert(#sizeof(category.choices) == 4);
}

let ratings = values.f_social_get_ratings(player);
test.assert(ratings.effic == 2);
test.assert(ratings.planet == 1);
test.assert(ratings.morale == 0 - 1);
test.assert(ratings.police == 0 - 1);
test.assert(values.f_social_get_faction_modifier(player, 'planet') == 1);
test.assert(values.f_social_get_faction_modifier(player, 'economy') == 0);

let error = values.f_social_validate_choices(player, {
	politics: 'Democratic',
	economics: 'Simple',
	values: 'Survival',
	future_society: 'None',
});
test.assert(#is_defined(error));
technologies.EthicalCalculus = true;
test.assert(!#is_defined(values.f_social_validate_choices(player, {
	politics: 'Democratic',
	economics: 'Simple',
	values: 'Survival',
	future_society: 'None',
})));
test.assert(#sizeof(values.f_social_get_available_choices(player, 'politics')) == 2);
test.assert(values.f_social_get_adoption_cost(player, choices) == 0);

const adoption_choices = [
	{
		politics: 'PoliceState', economics: 'Simple',
		values: 'Survival', future_society: 'None',
	},
	{
		politics: 'PoliceState', economics: 'Planned',
		values: 'Survival', future_society: 'None',
	},
	{
		politics: 'PoliceState', economics: 'Planned',
		values: 'Power', future_society: 'None',
	},
	{
		politics: 'PoliceState', economics: 'Planned',
		values: 'Power', future_society: 'Cybernetic',
	},
];
const adoption_costs = {
	Citizen: [0, 0, 0, 0],
	Specialist: [8, 27, 64, 125],
	Talent: [16, 54, 128, 250],
	Librarian: [24, 81, 192, 375],
	Thinker: [32, 108, 256, 500],
	Transcend: [40, 135, 320, 625],
};
for (difficulty in adoption_costs) {
	player.difficulty_level = difficulty;
	for (let i = 0; i < 4; i++) {
		test.assert(
			values.f_social_get_adoption_cost(player, adoption_choices[i]) ==
			adoption_costs[difficulty][i]
		);
	}
}
player.difficulty_level = 'Transcend';

faction_id = 'NEUTRAL';
choices = {
	politics: 'Frontier',
	economics: 'Simple',
	values: 'Power',
	future_society: 'ThoughtControl',
};
values.f_project_get_player_effects = (target_player) => {
	test.assert(target_player == player);
	return {
		ignore_power_penalties: true,
		ignore_thought_control_penalties: true,
		ignore_cybernetic_penalties: false,
	};
};
ratings = values.f_social_get_ratings(player);
test.assert(ratings.industry == 0);
test.assert(ratings.support == 2);
test.assert(ratings.morale == 4);
test.assert(ratings.police == 2);
test.assert(values.f_social_get_free_support(player, 6) == 4);

choices.values = 'Survival';
choices.future_society = 'Cybernetic';
values.f_project_get_player_effects = (target_player) => {
	return {
		ignore_power_penalties: false,
		ignore_thought_control_penalties: false,
		ignore_cybernetic_penalties: true,
	};
};
ratings = values.f_social_get_ratings(player);
test.assert(ratings.effic == 2);
test.assert(ratings.planet == 2);
test.assert(ratings.research == 2);
test.assert(ratings.police == 0);
values.f_project_get_player_effects = #undefined;

technologies = {
	EthicalCalculus: true,
	PlanetaryNetworks: true,
	AdvancedMilitaryAlgorithms: true,
	DigitalSentience: true,
};
choices = {
	politics: 'Democratic',
	economics: 'Planned',
	values: 'Power',
	future_society: 'Cybernetic',
};
faction_id = 'HIVE';
ratings = values.f_social_get_ratings(player);
test.assert(ratings.economy == 0 - 2);
test.assert(ratings.effic == 2);
test.assert(ratings.support == 0);
test.assert(ratings.morale == 2);
test.assert(ratings.police == 0 - 3);
test.assert(ratings.growth == 5);
test.assert(ratings.planet == 2);
test.assert(ratings.industry == 0);
test.assert(ratings.research == 2);

test.assert(values.f_social_get_mineral_cost(player, 40) == 40);
test.assert(values.f_social_get_support_cost(player) == 1);
test.assert(values.f_social_get_free_support(player, 6) == 2);
test.assert(values.f_social_get_new_base_minerals(player) == 10);
test.assert(values.f_social_get_unit_training_morale_bonus(player, 4) == 4);
test.assert(values.f_social_get_morale_bonus(player, false) == 1);
test.assert(values.f_social_get_morale_bonus(player, true) == 2);
test.assert(values.f_social_get_economy_base_bonus(player, false) == 0 - 1);
test.assert(values.f_social_get_tile_energy_bonus(player) == 0);
test.assert(values.f_social_get_commerce_bonus(player) == 0);
test.assert(values.f_social_get_research_multiplier(player) == 1.2);

choices = {
	politics: 'Frontier',
	economics: 'FreeMarket',
	values: 'Wealth',
	future_society: 'Eudaimonic',
};
faction_id = 'MORGANITES';
ratings = values.f_social_get_ratings(player);
test.assert(ratings.economy == 5);
test.assert(ratings.industry == 3);
test.assert(ratings.morale == 0 - 4);
test.assert(values.f_social_get_mineral_cost(player, 40) == 28);
test.assert(values.f_social_get_economy_base_bonus(player, false) == 4);
test.assert(values.f_social_get_tile_energy_bonus(player) == 1);
test.assert(values.f_social_get_commerce_bonus(player) == 4);
test.assert(values.f_social_get_unit_training_morale_bonus(player, 4) == 2);
test.assert(values.f_social_get_unit_training_morale_bonus(player, 1) == 0);

choices = {
	politics: 'Democratic',
	economics: 'Simple',
	values: 'Survival',
	future_society: 'None',
};
faction_id = 'PEACEKEEPERS';
test.assert(values.f_social_get_ratings(player).support == 0 - 2);
test.assert(values.f_social_get_new_base_minerals(player) == 0);

choices.politics = 'Frontier';
test.assert(values.f_social_get_new_base_minerals(player) == 10);

let police = values.f_social_get_police_rules(player, 0 - 100);
test.assert(police == {rating: 0 - 5, unit_limit: 0, unit_multiplier: 1});
police = values.f_social_get_police_rules(player, 0 - 1);
test.assert(police == {rating: 0 - 1, unit_limit: 1, unit_multiplier: 1});
police = values.f_social_get_police_rules(player, 0);
test.assert(police == {rating: 0, unit_limit: 1, unit_multiplier: 1});
police = values.f_social_get_police_rules(player, 1);
test.assert(police == {rating: 1, unit_limit: 2, unit_multiplier: 1});
police = values.f_social_get_police_rules(player, 2);
test.assert(police == {rating: 2, unit_limit: 3, unit_multiplier: 1});
police = values.f_social_get_police_rules(player, 100);
test.assert(police == {rating: 3, unit_limit: 3, unit_multiplier: 2});

technologies.DoctrineLoyalty = true;
const original_choices = choices;
const owned_base = {get_owner: () => { return player; }};
const rival_base = {get_owner: () => { return {id: 2}; }};
bases = [owned_base, rival_base];
let refreshed_ratings = [];
values.f_economy_get_base_psych = (target_game, target_base) => {
	test.assert(target_game == game && target_base == owned_base);
	return 3;
};
values.f_base_process_psych = (target_game, target_base, psych) => {
	test.assert(target_game == game && target_base == owned_base && psych == 3);
	refreshed_ratings :+values.f_social_get_ratings(player).police;
};
const social_event = {
	caller: player.id,
	game: game,
	data: {
		player: player,
		choices: {
			politics: 'PoliceState',
			economics: 'Simple',
			values: 'Survival',
			future_society: 'None',
		},
	},
};
energy_credits = 39;
test.assert(#is_defined(set_social_engineering.validate(social_event)));
energy_credits = 100;
test.assert(!#is_defined(set_social_engineering.validate(social_event)));
social_event.applied = set_social_engineering.apply(social_event);
test.assert(choices.politics == 'PoliceState');
test.assert(energy_credits == 60);
test.assert(refreshed_ratings == [2]);
set_social_engineering.rollback(social_event);
test.assert(choices == original_choices);
test.assert(energy_credits == 100);
test.assert(refreshed_ratings == [2, 0]);
test.assert(#sizeof(triggers) == 4);
test.assert(triggers[0].name == 'social_engineering_updated');
test.assert(triggers[1].name == 'economy_updated');
test.assert(triggers[2].name == 'social_engineering_updated');
test.assert(triggers[3].name == 'economy_updated');
