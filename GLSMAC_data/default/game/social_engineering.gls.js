const RATING_LIMITS = {
	economy: {min: 0 - 3, max: 5},
	effic: {min: 0 - 4, max: 4},
	support: {min: 0 - 4, max: 3},
	talent: {min: 0 - 1, max: 1},
	morale: {min: 0 - 4, max: 4},
	police: {min: 0 - 5, max: 3},
	growth: {min: 0 - 3, max: 6},
	planet: {min: 0 - 3, max: 3},
	probe: {min: 0 - 2, max: 3},
	industry: {min: 0 - 3, max: 5},
	research: {min: 0 - 5, max: 5},
};

const UPHEAVAL_COSTS = [0, 8, 27, 64, 125];
const UPHEAVAL_DIFFICULTY_MULTIPLIERS = {
	Citizen: 0,
	Specialist: 1,
	Talent: 2,
	Librarian: 3,
	Thinker: 4,
	Transcend: 5,
};

const policy = (id, name, required_technology, ratings) => {
	return {
		id: id,
		name: name,
		required_technology: required_technology,
		ratings: ratings,
	};
};

const categories = [
	{
		id: 'politics',
		name: 'Politics',
		default_choice: 'Frontier',
		choices: [
			policy('Frontier', 'Frontier', '', {}),
			policy('PoliceState', 'Police State', 'DoctrineLoyalty', {
				police: 2, support: 2, effic: 0 - 2,
			}),
			policy('Democratic', 'Democratic', 'EthicalCalculus', {
				effic: 2, growth: 2, support: 0 - 2,
			}),
			policy('Fundamentalist', 'Fundamentalist', 'SecretsHumanBrain', {
				morale: 1, probe: 2, research: 0 - 2,
			}),
		],
	},
	{
		id: 'economics',
		name: 'Economics',
		default_choice: 'Simple',
		choices: [
			policy('Simple', 'Simple', '', {}),
			policy('FreeMarket', 'Free Market', 'IndustrialEconomics', {
				economy: 2, planet: 0 - 3, police: 0 - 5,
			}),
			policy('Planned', 'Planned', 'PlanetaryNetworks', {
				growth: 2, industry: 1, effic: 0 - 2,
			}),
			policy('Green', 'Green', 'CentauriEmpathy', {
				planet: 2, effic: 2, growth: 0 - 2,
			}),
		],
	},
	{
		id: 'values',
		name: 'Values',
		default_choice: 'Survival',
		choices: [
			policy('Survival', 'Survival', '', {}),
			policy('Power', 'Power', 'AdvancedMilitaryAlgorithms', {
				morale: 2, support: 2, industry: 0 - 2,
			}),
			policy('Knowledge', 'Knowledge', 'Cyberethics', {
				research: 2, effic: 1, probe: 0 - 2,
			}),
			policy('Wealth', 'Wealth', 'IndustrialAutomation', {
				industry: 1, economy: 1, morale: 0 - 2,
			}),
		],
	},
	{
		id: 'future_society',
		name: 'Future Society',
		default_choice: 'None',
		choices: [
			policy('None', 'None', '', {}),
			policy('Cybernetic', 'Cybernetic', 'DigitalSentience', {
				effic: 2, planet: 2, research: 2, police: 0 - 3,
			}),
			policy('Eudaimonic', 'Eudaimonic', 'Eudaimonia', {
				growth: 2, economy: 2, industry: 2, morale: 0 - 2,
			}),
			policy('ThoughtControl', 'Thought Control', 'TheWillToPower', {
				police: 2, morale: 2, probe: 2, support: 0 - 3,
			}),
		],
	},
];

// Original-SMAC faction modifiers. Expansion factions deliberately remain neutral.
const faction_modifiers = {
	GAIANS: {morale: 0 - 1, police: 0 - 1, effic: 2, planet: 1},
	HIVE: {growth: 1, industry: 1, economy: 0 - 2},
	UNIVERSITY: {research: 2, probe: 0 - 2},
	MORGANITES: {economy: 1, support: 0 - 1},
	SPARTANS: {morale: 2, police: 1, industry: 0 - 1},
	BELIEVERS: {research: 0 - 2, probe: 1, support: 2, planet: 0 - 1},
	PEACEKEEPERS: {effic: 0 - 1},
};

const faction_immunities = {
	HIVE: {effic: true},
};

const faction_commerce_bonuses = {
	MORGANITES: 1,
};

const choices_by_category = {};
for (category of categories) {
	let choices = {};
	for (choice of category.choices) {
		choices[choice.id] = choice;
	}
	choices_by_category[category.id] = choices;
}

const make_ratings = () => {
	return {
		economy: 0, effic: 0, support: 0, talent: 0, morale: 0,
		police: 0, growth: 0, planet: 0, probe: 0, industry: 0, research: 0,
	};
};

const get_choices = (player) => {
	return player.get_social_engineering();
};

const get_faction_id = (player) => {
	const faction = player.get_faction();
	return #is_defined(faction) ? faction.id : '';
};

const get_faction_modifier = (player, name) => {
	const faction_id = get_faction_id(player);
	const modifiers = faction_modifiers[faction_id];
	return #is_defined(modifiers) && #is_defined(modifiers[name]) ? modifiers[name] : 0;
};

const add_ratings = (ratings, modifiers, ignore_negative) => {
	for (name in modifiers) {
		if (
			#is_defined(RATING_LIMITS[name]) &&
			(!#is_defined(ignore_negative) || !ignore_negative || modifiers[name] >= 0)
		) {
			ratings[name] = ratings[name] + modifiers[name];
		}
	}
};

const ignores_choice_penalties = (choice, project_effects) => {
	if (!#is_defined(project_effects)) {
		return false;
	}
	return (
		(
			choice.id == 'Power' &&
			#is_defined(project_effects.ignore_power_penalties) &&
			project_effects.ignore_power_penalties
		) ||
		(
			choice.id == 'ThoughtControl' &&
			#is_defined(project_effects.ignore_thought_control_penalties) &&
			project_effects.ignore_thought_control_penalties
		) ||
		(
			choice.id == 'Cybernetic' &&
			#is_defined(project_effects.ignore_cybernetic_penalties) &&
			project_effects.ignore_cybernetic_penalties
		)
	);
};

const get_ratings_for_choices = (player, choices, project_effects) => {
	const ratings = make_ratings();
	for (category of categories) {
		const choice = choices_by_category[category.id][choices[category.id]];
		if (#is_defined(choice)) {
			add_ratings(
				ratings,
				choice.ratings,
				ignores_choice_penalties(choice, project_effects)
			);
		}
	}
	const faction_id = get_faction_id(player);
	if (#is_defined(faction_modifiers[faction_id])) {
		add_ratings(ratings, faction_modifiers[faction_id]);
	}
	if (#is_defined(faction_immunities[faction_id])) {
		for (name in faction_immunities[faction_id]) {
			if (faction_immunities[faction_id][name] && ratings[name] < 0) {
				ratings[name] = 0;
			}
		}
	}
	for (name in RATING_LIMITS) {
		ratings[name] = #max(
			RATING_LIMITS[name].min,
			#min(RATING_LIMITS[name].max, ratings[name])
		);
	}
	return ratings;
};

const get_ratings = (player, project_effects) => {
	return get_ratings_for_choices(player, get_choices(player), project_effects);
};

const resolve_ratings = (player, ratings_resolver) => {
	return #is_defined(ratings_resolver)
		? ratings_resolver(player)
		: get_ratings(player, {});
};

const validate_choices = (player, choices) => {
	if (#typeof(choices) != 'Object') {
		return 'Social engineering choices must be an object';
	}
	for (category of categories) {
		const id = choices[category.id];
		if (#typeof(id) != 'String') {
			return category.name + ' must identify a social engineering choice';
		}
		const choice = choices_by_category[category.id][id];
		if (!#is_defined(choice)) {
			return 'Unknown ' + category.name + ' choice: ' + id;
		}
		if (choice.required_technology != '' && !player.has_technology(choice.required_technology)) {
			return choice.name + ' requires ' + choice.required_technology;
		}
	}
};

const get_available_choices = (player, category_id) => {
	if (!#is_defined(choices_by_category[category_id])) {
		return [];
	}
	let result = [];
	for (choice of choices_by_category[category_id]) {
		if (choice.required_technology == '' || player.has_technology(choice.required_technology)) {
			result :+choice;
		}
	}
	return result;
};

const get_adoption_cost = (player, choices) => {
	const current = get_choices(player);
	let change_count = 0;
	for (category of categories) {
		if (current[category.id] != choices[category.id]) {
			change_count++;
		}
	}
	const difficulty = player.difficulty_level;
	const multiplier = #is_defined(UPHEAVAL_DIFFICULTY_MULTIPLIERS[difficulty])
		? UPHEAVAL_DIFFICULTY_MULTIPLIERS[difficulty]
		: UPHEAVAL_DIFFICULTY_MULTIPLIERS.Transcend;
	return UPHEAVAL_COSTS[change_count] * multiplier;
};

const get_mineral_cost = (player, base_cost, ratings_resolver) => {
	const industry = resolve_ratings(player, ratings_resolver).industry;
	return #max(1, #ceil(#to_float(base_cost * (10 - industry)) / 10.0));
};

const get_support_cost = (player, ratings_resolver) => {
	return resolve_ratings(player, ratings_resolver).support <= 0 - 4 ? 2 : 1;
};

const get_police_rules = (player, rating_bonus, ratings_resolver) => {
	const bonus = #is_defined(rating_bonus) ? rating_bonus : 0;
	const rating = #max(
		RATING_LIMITS.police.min,
		#min(
			RATING_LIMITS.police.max,
			resolve_ratings(player, ratings_resolver).police + bonus
		)
	);
	let unit_limit = 0;
	if (rating >= 2) {
		unit_limit = 3;
	} else if (rating == 1) {
		unit_limit = 2;
	} else if (rating >= 0 - 1) {
		unit_limit = 1;
	}
	return {
		rating: rating,
		unit_limit: unit_limit,
		unit_multiplier: rating >= 3 ? 2 : 1,
	};
};

const get_free_support = (player, base_size, ratings_resolver) => {
	const support = resolve_ratings(player, ratings_resolver).support;
	if (support <= 0 - 3) { return 0; }
	if (support <= 0 - 1) { return 1; }
	if (support == 0) { return 2; }
	if (support == 1) { return 3; }
	if (support == 2) { return 4; }
	return #max(4, base_size);
};

const get_new_base_minerals = (player, ratings_resolver) => {
	return resolve_ratings(player, ratings_resolver).support <= 0 - 2 ? 0 : 10;
};

const get_unit_training_morale_bonus = (player, bonus, ratings_resolver) => {
	if (resolve_ratings(player, ratings_resolver).morale > 0 - 2) {
		return bonus;
	}
	return #floor(#to_float(bonus) / 2.0);
};

const get_morale_bonus = (player, defending, ratings_resolver) => {
	const morale = resolve_ratings(player, ratings_resolver).morale;
	if (morale <= 0 - 4) { return 0 - 3; }
	if (morale == 0 - 3) { return 0 - 2; }
	if (morale <= 0 - 1) { return 0 - 1; }
	if (morale == 0) { return 0; }
	if (morale == 1) { return 1; }
	if (morale == 2) { return defending ? 2 : 1; }
	if (morale == 3) { return defending ? 3 : 2; }
	return 3;
};

const get_economy_base_bonus = (player, has_headquarters, ratings_resolver) => {
	const economy = resolve_ratings(player, ratings_resolver).economy;
	if (economy <= 0 - 3) { return 0 - 2; }
	if (economy == 0 - 2) { return 0 - 1; }
	if (economy == 0 - 1) { return has_headquarters ? 0 - 1 : 0; }
	if (economy == 1) { return 1; }
	if (economy == 4) { return 2; }
	if (economy >= 5) { return 4; }
	return 0;
};

const get_tile_energy_bonus = (player, ratings_resolver) => {
	return resolve_ratings(player, ratings_resolver).economy >= 2 ? 1 : 0;
};

const get_commerce_bonus = (player, ratings_resolver) => {
	const faction_id = get_faction_id(player);
	let bonus = #is_defined(faction_commerce_bonuses[faction_id])
		? faction_commerce_bonuses[faction_id]
		: 0;
	const economy = resolve_ratings(player, ratings_resolver).economy;
	if (economy >= 5) {
		bonus += 3;
	} else if (economy == 4) {
		bonus += 2;
	} else if (economy == 3) {
		bonus += 1;
	}
	return bonus;
};

const get_research_multiplier = (player, ratings_resolver) => {
	return 1.0 + #to_float(resolve_ratings(player, ratings_resolver).research) * 0.1;
};

return (game) => {
	game.on('start', (e) => {
		const get_project_effects = (player) => {
			const resolver = #is_defined(game.get)
				? game.get('f_project_get_player_effects')
				: #undefined;
			return #is_defined(resolver) ? resolver(player) : {};
		};
		const get_game_ratings_for_choices = (player, choices) => {
			return get_ratings_for_choices(player, choices, get_project_effects(player));
		};
		const get_game_ratings = (player) => {
			return get_ratings(player, get_project_effects(player));
		};
		game.set('f_social_get_categories', () => { return categories; });
		game.set('f_social_get_ratings', get_game_ratings);
		game.set('f_social_get_ratings_for_choices', get_game_ratings_for_choices);
		game.set('f_social_get_faction_modifier', get_faction_modifier);
		game.set('f_social_validate_choices', validate_choices);
		game.set('f_social_get_available_choices', get_available_choices);
		game.set('f_social_get_adoption_cost', get_adoption_cost);
		game.set('f_social_get_mineral_cost', (player, base_cost) => {
			return get_mineral_cost(player, base_cost, get_game_ratings);
		});
		game.set('f_social_get_support_cost', (player) => {
			return get_support_cost(player, get_game_ratings);
		});
		game.set('f_social_get_police_rules', (player, rating_bonus) => {
			return get_police_rules(player, rating_bonus, get_game_ratings);
		});
		game.set('f_social_get_free_support', (player, base_size) => {
			return get_free_support(player, base_size, get_game_ratings);
		});
		game.set('f_social_get_new_base_minerals', (player) => {
			return get_new_base_minerals(player, get_game_ratings);
		});
		game.set('f_social_get_unit_training_morale_bonus', (player, bonus) => {
			return get_unit_training_morale_bonus(player, bonus, get_game_ratings);
		});
		game.set('f_social_get_morale_bonus', (player, defending) => {
			return get_morale_bonus(player, defending, get_game_ratings);
		});
		game.set('f_social_get_economy_base_bonus', (player, has_headquarters) => {
			return get_economy_base_bonus(player, has_headquarters, get_game_ratings);
		});
		game.set('f_social_get_tile_energy_bonus', (player) => {
			return get_tile_energy_bonus(player, get_game_ratings);
		});
		game.set('f_social_get_commerce_bonus', (player) => {
			return get_commerce_bonus(player, get_game_ratings);
		});
		game.set('f_social_get_research_multiplier', (player) => {
			return get_research_multiplier(player, get_game_ratings);
		});
	});
};
