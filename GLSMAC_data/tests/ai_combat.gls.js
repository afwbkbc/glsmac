const combat = #include('../default/game/ai/combat');

const player_id = 1;
const other_player_id = 2;
let next_base_id = 1;

const make_tile = (x, y) => {
	let base = null;
	let units = [];
	let surrounding = [];
	return {
		x: x,
		y: y,
		is_land: true,
		is_water: false,
		rockiness: 1,
		features: {xenofungus: false},
		terraforming: {bunker: false},
		get_base: () => { return base; },
		set_base: (value) => { base = value; },
		get_units: () => { return units; },
		add_unit: (unit) => { units :+unit; },
		get_surrounding_tiles: () => { return surrounding; },
		set_surrounding_tiles: (tiles) => { surrounding = tiles; },
	};
};

const make_base = (owner_id, tile, size) => {
	const base_size = #is_defined(size) ? size : 1;
	let custom = {};
	const base = {
		id: next_base_id++,
		get_owner: () => { return {id: owner_id}; },
		get_tile: () => { return tile; },
		get_size: () => { return base_size; },
		has: (key) => { return #is_defined(custom[key]); },
		set: (key, value) => { custom[key] = value; },
	};
	tile.set_base(base);
	return base;
};

const make_unit = (tile, health) => {
	return {
		health: health,
		get_tile: () => { return tile; },
	};
};

const tm = {
	get_distance: (first, second) => {
		return #abs(first.x - second.x) + #abs(first.y - second.y);
	},
};

const field = make_tile(5, 5);
const north_tile = make_tile(5, 3);
const west_tile = make_tile(3, 5);
const enemy_tile = make_tile(5, 4);
const north_base = make_base(player_id, north_tile);
const west_base = make_base(player_id, west_tile);
const enemy_base = make_base(other_player_id, enemy_tile);
const bases = [enemy_base, north_base, west_base];

test.assert(combat.find_nearest_friendly_base(tm, player_id, field, bases) == north_base);
test.assert(combat.find_nearest_friendly_base(tm, player_id, field, [enemy_base]) == null);

test.assert(combat.get_repair_destination(tm, make_unit(field, 0.49), player_id, bases) == north_base);
test.assert(combat.get_repair_destination(tm, make_unit(field, 0.5), player_id, bases) == null);

test.assert(combat.get_repair_destination(tm, make_unit(north_tile, 0.79), player_id, bases) == north_base);
test.assert(combat.get_repair_destination(tm, make_unit(north_tile, 0.8), player_id, bases) == null);
test.assert(combat.get_repair_destination(tm, make_unit(enemy_tile, 0.49), player_id, bases) == north_base);

let next_unit_id = 1;
const make_combat_unit = (owner_id, tile, offense, defense, health, morale, def_id) => {
	const unit_defense = #is_defined(defense) ? defense : 1;
	const unit_health = #is_defined(health) ? health : 1.0;
	const unit_morale = #is_defined(morale) ? morale : 2;
	const unit = {
		id: next_unit_id++,
		owner: owner_id,
		health: unit_health,
		morale: unit_morale,
		movement: 1.0,
		is_land: true,
		is_water: false,
		get_tile: () => { return tile; },
		get_def: () => {
			return {
				id: #is_defined(def_id) ? def_id : 'TestUnit',
				is_native: false,
				offense: offense,
				defense: unit_defense,
				movement_per_turn: 1.0,
			};
		},
	};
	tile.add_unit(unit);
	return unit;
};

const home_tile = make_tile(0, 0);
const home_base = make_base(player_id, home_tile);
const near_enemy_tile = make_tile(2, 0);
const second_enemy_tile = make_tile(0, 2);
const far_enemy_tile = make_tile(4, 0);
const near_enemy = make_combat_unit(other_player_id, near_enemy_tile, 1);
const second_enemy = make_combat_unit(other_player_id, second_enemy_tile, 2);
const far_enemy = make_combat_unit(other_player_id, far_enemy_tile, 2);
const friendly = make_combat_unit(player_id, near_enemy_tile, 2);
const colony = make_combat_unit(other_player_id, near_enemy_tile, 0);

test.assert(combat.get_force_power(second_enemy) == 5.5);
second_enemy.health = 0.5;
test.assert(combat.get_force_power(second_enemy) == 2.75);
second_enemy.health = 1.0;
test.assert(combat.get_force_power(colony) == 0.0);

test.assert(combat.get_required_garrison(tm, home_base, player_id, []) == 1);
test.assert(combat.get_required_garrison(tm, home_base, player_id, [near_enemy]) == 2);
test.assert(combat.get_required_garrison(tm, home_base, player_id, [near_enemy, second_enemy]) == 3);
test.assert(combat.get_required_garrison(tm, home_base, player_id, [near_enemy, second_enemy, far_enemy]) == 3);
test.assert(combat.get_required_garrison(tm, home_base, player_id, [friendly, colony, far_enemy]) == 1);
test.assert(combat.get_garrison_count(home_base, player_id) == 0);

const immovable_threat = make_combat_unit(other_player_id, near_enemy_tile, 3);
immovable_threat.is_immovable = true;
const sea_threat = make_combat_unit(other_player_id, near_enemy_tile, 3);
sea_threat.is_land = false;
sea_threat.is_water = true;
const artillery_threat = make_combat_unit(other_player_id, near_enemy_tile, 3, 1, 1.0, 2, 'SporeLauncher');
artillery_threat.is_land = false;
artillery_threat.is_water = true;
test.assert(!combat.can_threaten_tile(immovable_threat, home_tile));
test.assert(!combat.can_threaten_tile(sea_threat, home_tile));
test.assert(combat.can_threaten_tile(artillery_threat, home_tile));
test.assert(combat.get_required_garrison(tm, home_base, player_id, [immovable_threat, sea_threat]) == 1);
test.assert(combat.get_required_garrison(tm, home_base, player_id, [artillery_threat]) == 2);

const coastal_base_tile = make_tile(8, 8);
const coastal_water = make_tile(8, 9);
coastal_water.is_land = false;
coastal_water.is_water = true;
coastal_base_tile.set_surrounding_tiles([coastal_water]);
const coastal_base = make_base(player_id, coastal_base_tile);
const coastal_sea_threat = make_combat_unit(other_player_id, coastal_water, 3);
coastal_sea_threat.is_land = false;
coastal_sea_threat.is_water = true;
test.assert(combat.can_threaten_tile(coastal_sea_threat, coastal_base_tile));
test.assert(combat.get_required_garrison(tm, coastal_base, player_id, [coastal_sea_threat]) == 2);

const attack_origin = make_tile(10, 10);
const north_target_tile = make_tile(10, 9);
const east_target_tile = make_tile(11, 10);
const water_target_tile = make_tile(10, 11);
water_target_tile.is_land = false;
water_target_tile.is_water = true;
const attacker = make_combat_unit(player_id, attack_origin, 2, 1, 1.0, 2);
const healthy_defender = make_combat_unit(other_player_id, north_target_tile, 1, 1, 1.0, 2);
const wounded_defender = make_combat_unit(other_player_id, east_target_tile, 1, 2, 0.2, 2);
const east_stack_defender = make_combat_unit(other_player_id, east_target_tile, 1, 3, 1.0, 2);
make_combat_unit(other_player_id, water_target_tile, 1, 1, 0.01, 2);
make_combat_unit(player_id, east_target_tile, 1, 1, 0.01, 2);

test.assert(
	combat.choose_attack_target(
		attacker,
		player_id,
		[north_target_tile, water_target_tile, east_target_tile]
	) == healthy_defender
);
test.assert(combat.get_attack_score(attacker, wounded_defender) > combat.get_attack_score(attacker, healthy_defender));
test.assert(combat.get_attack_score(attacker, east_stack_defender) < combat.get_attack_score(attacker, healthy_defender));

const tie_north_tile = make_tile(20, 19);
const tie_east_tile = make_tile(21, 20);
const tie_north = make_combat_unit(other_player_id, tie_north_tile, 1, 1, 1.0, 2);
make_combat_unit(other_player_id, tie_east_tile, 1, 1, 1.0, 2);
test.assert(combat.choose_attack_target(attacker, player_id, [tie_east_tile, tie_north_tile]) == tie_north);

const assault_origin = make_tile(30, 30);
const assault_attacker = make_combat_unit(player_id, assault_origin, 2, 1, 1.0, 2);
const nearby_base_tile = make_tile(32, 30);
const farther_base_tile = make_tile(34, 30);
const nearby_base = make_base(other_player_id, nearby_base_tile);
const farther_base = make_base(other_player_id, farther_base_tile);
const strong_defender = make_combat_unit(other_player_id, nearby_base_tile, 1, 3, 1.0, 2);
test.assert(
	combat.choose_assault_target(
		tm,
		assault_attacker,
		player_id,
		[nearby_base, farther_base],
		[assault_attacker, strong_defender]
	) == farther_base
);

const incompatible_support = make_combat_unit(player_id, farther_base_tile, 8, 1, 1.0, 2);
incompatible_support.is_land = false;
incompatible_support.is_water = true;
const unsupported_assault_score = combat.get_assault_score(
	tm,
	assault_attacker,
	farther_base,
	player_id,
	[assault_attacker]
);
test.assert(combat.get_assault_score(
	tm,
	assault_attacker,
	farther_base,
	player_id,
	[assault_attacker, incompatible_support]
) == unsupported_assault_score);

const close_open_tile = make_tile(31, 30);
const distant_open_tile = make_tile(35, 30);
const close_open_base = make_base(other_player_id, close_open_tile);
const distant_open_base = make_base(other_player_id, distant_open_tile);
test.assert(
	combat.choose_assault_target(
		tm,
		assault_attacker,
		player_id,
		[distant_open_base, close_open_base],
		[assault_attacker]
	) == close_open_base
);
distant_open_base.set('economic_victory_turn', 40);
distant_open_base.set('economic_victory_cost', 1000);
test.assert(
	combat.choose_assault_target(
		tm,
		assault_attacker,
		player_id,
		[distant_open_base, close_open_base],
		[assault_attacker]
	) == distant_open_base
);

const water_base_tile = make_tile(30, 31);
water_base_tile.is_land = false;
water_base_tile.is_water = true;
const water_base = make_base(other_player_id, water_base_tile, 10);
test.assert(
	combat.choose_assault_target(tm, assault_attacker, player_id, [water_base], [assault_attacker]) == null
);

const reinforcement_origin = make_tile(40, 40);
const reinforcement_unit = make_combat_unit(player_id, reinforcement_origin, 2, 1, 1.0, 2);
const close_base_tile = make_tile(42, 40);
const threatened_base_tile = make_tile(45, 40);
const close_base = make_base(player_id, close_base_tile);
const threatened_base = make_base(player_id, threatened_base_tile);
const nearby_threat_tile = make_tile(46, 40);
const nearby_threat = make_combat_unit(other_player_id, nearby_threat_tile, 1, 1, 1.0, 2);
const reinforcement_units = [reinforcement_unit, nearby_threat];
test.assert(
	combat.choose_reinforcement_target(
		tm,
		reinforcement_unit,
		player_id,
		[close_base, threatened_base],
		reinforcement_units,
		{}
	) == threatened_base
);
let reservations = {};
const threatened_base_key = #to_string(threatened_base.id);
reservations[threatened_base_key] = 2;
test.assert(
	combat.choose_reinforcement_target(
		tm,
		reinforcement_unit,
		player_id,
		[close_base, threatened_base],
		reinforcement_units,
		reservations
	) == close_base
);
const close_defender = make_combat_unit(player_id, close_base_tile, 1, 1, 1.0, 2);
test.assert(combat.get_garrison_count(close_base, player_id) == 1);
test.assert(
	combat.choose_reinforcement_target(
		tm,
		reinforcement_unit,
		player_id,
		[close_base],
		[reinforcement_unit, close_defender],
		{}
	) == null
);

const commitment_origin = make_tile(60, 60);
const commitment_target_tile = make_tile(61, 60);
const commitment_support_tile = make_tile(61, 59);
make_base(other_player_id, commitment_target_tile);
const cautious_attacker = make_combat_unit(player_id, commitment_origin, 1, 1, 1.0, 2);
const fortified_defender = make_combat_unit(other_player_id, commitment_target_tile, 1, 1, 1.0, 2);
test.assert(combat.get_attack_score(cautious_attacker, fortified_defender) < 0.5);
test.assert(
	combat.choose_attack_target(
		cautious_attacker,
		player_id,
		[commitment_target_tile],
		tm,
		[cautious_attacker, fortified_defender]
	) == null
);
const ready_support = make_combat_unit(player_id, commitment_support_tile, 1, 1, 1.0, 2);
const supported_force = [cautious_attacker, ready_support, fortified_defender];
test.assert(
	combat.get_attack_commitment_score(
		tm,
		cautious_attacker,
		fortified_defender,
		player_id,
		supported_force
	) > 0.55
);
test.assert(
	combat.choose_attack_target(
		cautious_attacker,
		player_id,
		[commitment_target_tile],
		tm,
		supported_force
	) == fortified_defender
);
const second_fortified_defender = make_combat_unit(
	other_player_id,
	commitment_target_tile,
	1,
	1,
	1.0,
	2
);
test.assert(
	combat.choose_attack_target(
		cautious_attacker,
		player_id,
		[commitment_target_tile],
		tm,
		[cautious_attacker, ready_support, fortified_defender, second_fortified_defender]
	) == null
);
