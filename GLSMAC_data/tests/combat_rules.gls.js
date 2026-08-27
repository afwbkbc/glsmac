const combat_rules = #include('../default/game/combat_rules');

const make_tile = () => {
	let base = null;
	let units = [];
	return {
		rockiness: 1,
		features: {xenofungus: false},
		terraforming: {bunker: false},
		get_base: () => { return base; },
		set_base: (value) => { base = value; },
		get_units: () => { return units; },
		add_unit: (unit) => { units :+unit; },
	};
};

const make_base = (tile, owner, facilities) => {
	const base = {
		get_owner: () => { return {id: owner}; },
		get_facilities: () => { return facilities; },
	};
	tile.set_base(base);
	return base;
};

const make_unit = (tile, owner, offense, defense, is_native, movement_type) => {
	const unit = {
		id: owner,
		owner: owner,
		morale: 2,
		health: 1.0,
		movement: 1.0,
		is_land: movement_type == 'land',
		is_water: movement_type == 'water',
		is_air: movement_type == 'air',
		get_owner: () => { return {id: owner}; },
		get_tile: () => { return tile; },
		get_def: () => {
			return {
				is_native: is_native,
				offense: offense,
				defense: defense,
			};
		},
	};
	tile.add_unit(unit);
	return unit;
};

const attack_tile = make_tile();
const attacker = make_unit(attack_tile, 1, 2, 1, false, 'land');
const base_tile = make_tile();
let facilities = [];
make_base(base_tile, 2, facilities);
const defender = make_unit(base_tile, 2, 1, 2, false, 'land');

test.assert(combat_rules.get_base_defense_multiplier(defender) == 1.0);
test.assert(combat_rules.get_combat_powers(attacker, defender).defence == 2.5);

const morale_facility_tile = make_tile();
make_base(morale_facility_tile, 2, [{defense_multiplier: 1.0, defender_morale_bonus: 1}]);
const morale_facility_defender = make_unit(morale_facility_tile, 2, 1, 2, false, 'land');
test.assert(combat_rules.get_base_defender_morale_bonus(morale_facility_defender) == 1);
test.assert(combat_rules.get_combat_powers(attacker, morale_facility_defender).defence == 2.8125);
const morale_facility_occupier = make_unit(morale_facility_tile, 3, 1, 2, false, 'land');
test.assert(combat_rules.get_base_defender_morale_bonus(morale_facility_occupier) == 0);

facilities :+{defense_multiplier: 2.0};
test.assert(combat_rules.get_base_defense_multiplier(defender) == 2.0);
test.assert(combat_rules.get_combat_powers(attacker, defender).defence == 5.0);

facilities :+{defense_multiplier: 1.5};
test.assert(combat_rules.get_base_defense_multiplier(defender) == 2.5);

const occupying_defender = make_unit(base_tile, 3, 1, 2, false, 'land');
test.assert(combat_rules.get_base_defense_multiplier(occupying_defender) == 1.0);
test.assert(combat_rules.get_combat_powers(attacker, occupying_defender).defence == 2.5);

const native_attacker = make_unit(attack_tile, 1, 1, 1, true, 'land');
test.assert(combat_rules.get_combat_powers(native_attacker, defender).defence == 2.5);

const fungus_tile = make_tile();
fungus_tile.features.xenofungus = true;
const fungus_defender = make_unit(fungus_tile, 2, 1, 2, false, 'land');
const normal_fungus_powers = combat_rules.get_combat_powers(attacker, fungus_defender);
const pholus_game = {
	get: (key) => {
		if (key == 'f_social_get_morale_bonus') {
			return #undefined;
		}
		test.assert(key == 'f_project_get_player_effects');
		return (player) => { return {
			psi_attack_multiplier: 1.0,
			psi_defense_multiplier: 1.0,
			native_fungus_combat: player.id == attacker.owner,
		}; };
	},
};
const pholus_fungus_powers = combat_rules.get_combat_powers(
	attacker,
	fungus_defender,
	pholus_game
);
test.assert(pholus_fungus_powers.attack == normal_fungus_powers.attack * 1.5);
test.assert(normal_fungus_powers.defence == pholus_fungus_powers.defence * 1.5);

const project_psi_game = {
	get: (key) => {
		if (key == 'f_social_get_morale_bonus') {
			return #undefined;
		}
		if (key == 'f_base_get_effective_facilities') {
			return (base) => { return base.get_facilities(); };
		}
		test.assert(key == 'f_project_get_player_effects');
		return (player) => {
			return player.id == native_attacker.owner
				? {psi_attack_multiplier: 1.5, psi_defense_multiplier: 1.0}
				: {psi_attack_multiplier: 1.0, psi_defense_multiplier: 1.5};
		};
	},
};
const project_psi_powers = combat_rules.get_combat_powers(
	native_attacker,
	defender,
	project_psi_game
);
test.assert(project_psi_powers.attack == 4.5);
test.assert(project_psi_powers.defence == 3.75);

const water_attacker = make_unit(attack_tile, 1, 2, 1, false, 'water');
const air_attacker = make_unit(attack_tile, 1, 2, 1, false, 'air');
facilities :+{defense_multiplier: 1.0, water_defense_multiplier: 2.0};
facilities :+{defense_multiplier: 1.0, air_defense_multiplier: 2.0};
test.assert(combat_rules.get_base_defense_multiplier(defender, attacker) == 2.5);
test.assert(combat_rules.get_base_defense_multiplier(defender, water_attacker) == 3.5);
test.assert(combat_rules.get_base_defense_multiplier(defender, air_attacker) == 3.5);
test.assert(combat_rules.get_combat_powers(water_attacker, defender).defence == 8.75);
test.assert(combat_rules.get_combat_powers(air_attacker, defender).defence == 8.75);

facilities = [];
const project_defense_game = {
	get: (key) => {
		if (key == 'f_social_get_morale_bonus') {
			return #undefined;
		}
		test.assert(key == 'f_base_get_effective_facilities');
		return (base) => { return [{defense_multiplier: 2.0}]; };
	},
};
test.assert(
	combat_rules.get_base_defense_multiplier(defender, attacker, project_defense_game) == 2.0
);
test.assert(combat_rules.get_combat_powers(attacker, defender, project_defense_game).defence == 5.0);

const social_attack_tile = make_tile();
const social_defense_tile = make_tile();
const social_attacker = make_unit(social_attack_tile, 1, 2, 1, false, 'land');
const social_defender = make_unit(social_defense_tile, 2, 1, 2, false, 'land');
const social_game = {
	get: (key) => {
		if (key == 'f_social_get_morale_bonus') {
			return (player, defending) => {
				return player.id == 1 ? 1 : (defending ? 2 : 1);
			};
		}
		return #undefined;
	},
};
const social_powers = combat_rules.get_combat_powers(
	social_attacker,
	social_defender,
	social_game
);
test.assert(social_powers.attack == 2.25);
test.assert(social_powers.defence == 2.5);

const creche_tile = make_tile();
make_base(creche_tile, 2, [{defender_morale_minimum: 1}]);
const creche_defender = make_unit(creche_tile, 2, 1, 2, false, 'land');
let creche_social_bonus = 0 - 2;
const creche_game = {
	get: (key) => {
		if (key == 'f_social_get_morale_bonus') {
			return (player, defending) => { return creche_social_bonus; };
		}
		if (key == 'f_base_get_effective_facilities') {
			return (base) => { return base.get_facilities(); };
		}
		return #undefined;
	},
};
test.assert(combat_rules.get_base_defender_morale_minimum(creche_defender, creche_game) == 1);
test.assert(combat_rules.get_combat_powers(attacker, creche_defender, creche_game).defence == 2.8125);
creche_social_bonus = 2;
test.assert(combat_rules.get_combat_powers(attacker, creche_defender, creche_game).defence == 3.125);
creche_social_bonus = 0 - 2;
const creche_occupier = make_unit(creche_tile, 3, 1, 2, false, 'land');
test.assert(combat_rules.get_base_defender_morale_minimum(creche_occupier, creche_game) == 0);
test.assert(combat_rules.get_social_morale_bonus(creche_occupier, creche_game, true) == 0 - 2);
const creche_occupier_defence = combat_rules.get_combat_powers(
	attacker,
	creche_occupier,
	creche_game
).defence;
test.assert(creche_occupier_defence == 1.875);
const creche_native = make_unit(creche_tile, 2, 1, 2, true, 'land');
test.assert(combat_rules.get_combat_powers(attacker, creche_native, creche_game).defence == 2.5);

const stack_tile = make_tile();
const weak_defender = make_unit(stack_tile, 2, 1, 1, false, 'land');
weak_defender.id = 20;
const strong_defender = make_unit(stack_tile, 2, 1, 4, false, 'land');
strong_defender.id = 30;
test.assert(combat_rules.get_best_defender(attacker, stack_tile) == strong_defender);

strong_defender.health = 0.25;
test.assert(combat_rules.get_best_defender(attacker, stack_tile) == weak_defender);
weak_defender.health = 0.0;
test.assert(combat_rules.get_best_defender(attacker, stack_tile) == strong_defender);

const tied_defender = make_unit(stack_tile, 2, 1, 1, false, 'land');
tied_defender.id = 10;
test.assert(combat_rules.get_best_defender(attacker, stack_tile) == tied_defender);

const conventional_psi_attacker = make_unit(attack_tile, 1, 1, 1, false, 'land');
conventional_psi_attacker.get_def = () => {
	return {is_native: false, is_psi_attack: true, offense: 1, defense: 1};
};
test.assert(combat_rules.get_combat_powers(conventional_psi_attacker, defender).attack == 3.0);

const conventional_psi_defender = make_unit(base_tile, 2, 1, 1, false, 'land');
conventional_psi_defender.get_def = () => {
	return {is_native: false, is_psi_defense: true, offense: 1, defense: 1};
};
test.assert(combat_rules.get_combat_powers(attacker, conventional_psi_defender).defence == 2.5);

test.assert(combat_rules.is_artillery({id: 'TestArtillery', is_artillery: true}));
test.assert(!combat_rules.is_artillery({id: 'SporeLauncher', is_artillery: false}));
const artillery_attacker = make_unit(attack_tile, 1, 2, 1, false, 'land');
artillery_attacker.get_def = () => {
	return {id: 'TestArtillery', is_artillery: true, offense: 2, defense: 1};
};
test.assert(
	combat_rules.get_artillery_powers(artillery_attacker, morale_facility_defender).defence == 2.25
);

const ability_attacker = make_unit(attack_tile, 1, 2, 1, false, 'land');
ability_attacker.get_def = () => {
	return {
		is_native: false,
		is_psi_attack: false,
		offense: 2,
		defense: 1,
		movement_per_turn: 2.0,
		abilities: [],
	};
};
const ability_tile = make_tile();
const ability_defender = make_unit(ability_tile, 2, 1, 2, false, 'land');
ability_defender.get_def = () => {
	return {
		is_native: false,
		is_psi_defense: false,
		offense: 1,
		defense: 2,
		abilities: ['CommJammer'],
	};
};
test.assert(combat_rules.get_combat_powers(ability_attacker, ability_defender).defence == 3.0);

ability_attacker.is_land = false;
ability_attacker.is_air = true;
ability_defender.get_def = () => {
	return {
		is_native: false,
		is_psi_defense: false,
		offense: 1,
		defense: 2,
		abilities: ['AAATracking'],
	};
};
test.assert(combat_rules.get_combat_powers(ability_attacker, ability_defender).defence == 4.0);

ability_attacker.get_def = () => {
	return {
		is_native: false,
		is_psi_attack: false,
		offense: 2,
		defense: 1,
		movement_per_turn: 8.0,
		abilities: ['BlinkDisplacer'],
	};
};
ability_defender.get_def = () => {
	return {
		is_native: false,
		is_psi_defense: false,
		offense: 1,
		defense: 2,
		abilities: [],
	};
};
const blink_defender = make_unit(base_tile, 2, 1, 2, false, 'land');
blink_defender.get_def = ability_defender.get_def;
test.assert(
	combat_rules.get_combat_powers(ability_attacker, blink_defender, project_defense_game).defence == 2.5
);

const empath_attacker = make_unit(attack_tile, 1, 1, 1, false, 'land');
empath_attacker.get_def = () => {
	return {
		is_native: false,
		is_psi_attack: true,
		offense: 1,
		defense: 1,
		abilities: ['EmpathSong'],
	};
};
const trance_defender = make_unit(base_tile, 2, 1, 1, false, 'land');
trance_defender.get_def = () => {
	return {
		is_native: false,
		is_psi_defense: true,
		offense: 1,
		defense: 1,
		abilities: ['HypnoticTrance'],
	};
};
const ability_psi_powers = combat_rules.get_combat_powers(empath_attacker, trance_defender);
test.assert(ability_psi_powers.attack == 4.5);
test.assert(ability_psi_powers.defence == 3.75);
