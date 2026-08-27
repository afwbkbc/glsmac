const planet_buster = #include('../default/game/event/planet_buster');

const key = (id) => { return 'i' + #to_string(id); };
const tile_key = (x, y) => { return #to_string(x) + ':' + #to_string(y); };

const make_world = (random_roll, pod_count, deployments, charter_repealed) => {
	let units = {};
	let bases = {};
	let tile_by_position = {};
	let base_snapshots = {};
	let messages = [];
	let triggers = [];
	let stopped_animation = 0;
	let crater_apply_count = 0;
	let crater_restore_count = 0;
	const terrain_snapshot = 'terrain:before-crater';

	const make_tile = (x, y) => {
		let tile = {
			x: x,
			y: y,
			locked: false,
			adjacent_positions: {},
			base: null,
			units: [],
			neighbours: [],
		};
		tile.get_surrounding_tiles = () => { return tile.neighbours; };
		tile.get_base = () => { return tile.base; };
		tile.get_units = (include_embarked) => {
			let result = [];
			for (unit of tile.units) {
				const unit_key = key(unit.id);
				if (
					units[unit_key] != null &&
					(include_embarked || unit.transport_id == 0)
				) {
					result :+unit;
				}
			}
			return result;
		};
		tile.is_adjactent_to = (other) => {
			const position_key = tile_key(other.x, other.y);
			return #is_defined(tile.adjacent_positions[position_key]);
		};
		tile.is_locked = () => { return tile.locked; };
		const position_key = tile_key(x, y);
		tile_by_position[position_key] = tile;
		return tile;
	};

	const center = make_tile(10, 10);
	let ring = [];
	for (let i = 0; i < 8; i++) {
		const tile = make_tile(11 + i, 11 + i);
		ring :+tile;
	}
	center.neighbours = ring;
	const outside = make_tile(30, 30);
	const center_position = tile_key(center.x, center.y);
	ring[0].adjacent_positions[center_position] = true;

	const make_player = (id, name) => {
		let player = {
			id: id,
			name: name,
			major_atrocities: 2,
			sanction_turns: 3,
			pods: id == 1 ? pod_count : 0,
			deployments: id == 1 ? deployments : 0,
			relations: {},
		};
		player.get_major_atrocities = () => { return player.major_atrocities; };
		player.set_major_atrocities = (value) => { player.major_atrocities = value; };
		player.get_sanction_turns = () => { return player.sanction_turns; };
		player.set_sanction_turns = (value) => { player.sanction_turns = value; };
		player.get_orbital_facility_count = (id) => {
			test.assert(id == 'OrbitalDefensePod');
			return player.pods;
		};
		player.set_orbital_facility_count = (id, value) => {
			test.assert(id == 'OrbitalDefensePod');
			player.pods = value;
		};
		player.get_orbital_defense_deployments = () => { return player.deployments; };
		player.set_orbital_defense_deployments = (value) => { player.deployments = value; };
		return player;
	};

	const actor = make_player(0, 'Attacker');
	const victim = make_player(1, 'Defender');
	const bystander = make_player(2, 'Bystander');
	const observer = make_player(3, 'Observer');
	let players = [actor, victim, bystander, observer];
	for (player of players) {
		for (other of players) {
			if (player.id != other.id) {
				const other_key = key(other.id);
				player.relations[other_key] = 'neutral';
			}
		}
	}

	let defs = {
		planet_buster: {
			id: 'TestPlanetBuster',
			is_missile: true,
			weapon: 'PlanetBuster',
			reactor_power: 1,
		},
		defender: {
			id: 'TestDefender',
			is_missile: false,
			weapon: 'Laser',
			reactor_power: 1,
		},
	};

	const make_unit = (id, definition, owner, tile, transport_id) => {
		let unit = {
			id: id,
			def: definition.id,
			owner: owner.id,
			movement: 1.0,
			morale: 2,
			health: 1.0,
			moved_this_turn: false,
			terraforming: 'none',
			terraforming_turns_remaining: 0,
			home_base_id: 0,
			fuel: 0,
			transport_id: #is_defined(transport_id) ? transport_id : 0,
			get_tile: () => { return tile; },
			get_def: () => { return definition; },
		};
		unit.set_home_base_id = (value) => { unit.home_base_id = value; };
		const unit_key = key(id);
		units[unit_key] = unit;
		tile.units :+unit;
		return unit;
	};

	const make_base = (id, name, owner, tile) => {
		let base = {
			id: id,
			name: name,
			get_owner: () => { return owner; },
			get_tile: () => { return tile; },
		};
		const base_key = key(id);
		bases[base_key] = base;
		tile.base = base;
		return base;
	};

	const missile = make_unit(100, defs.planet_buster, actor, ring[0]);
	const defender = make_unit(101, defs.defender, victim, center);
	const collateral = make_unit(102, defs.defender, bystander, ring[1]);
	const safe = make_unit(103, defs.defender, victim, outside);
	const target_base = make_base(200, 'Target Base', victim, center);
	const support_base = make_base(201, 'Support Base', victim, outside);
	safe.home_base_id = target_base.id;

	const game = {
		random: {get_int: (low, high) => {
			test.assert(low == 0 && high == 1);
			return random_roll;
		}},
		tm: {
			get_tile: (x, y) => {
				const position_key = tile_key(x, y);
				return tile_by_position[position_key];
			},
			get_distance: (first, second) => {
				return #abs(first.x - second.x) + #abs(first.y - second.y);
			},
			apply_crater: (tile, radius) => {
				test.assert(
					tile.x == center.x && tile.y == center.y &&
					radius == defs.planet_buster.reactor_power
				);
				crater_apply_count++;
				return terrain_snapshot;
			},
			restore_terrain: (snapshot) => {
				test.assert(snapshot == terrain_snapshot);
				crater_restore_count++;
			},
		},
		um: {
			get_units: () => {
				let result = [];
				for (unit_key in units) {
					const active_unit = units[unit_key];
					if (active_unit != null) {
						result :+active_unit;
					}
				}
				return result;
			},
			has_unit: (id) => {
				const unit_key = key(id);
				return #is_defined(units[unit_key]) && units[unit_key] != null;
			},
			get_unit: (id) => {
				const unit_key = key(id);
				return units[unit_key];
			},
			despawn_unit: (unit) => {
				const unit_key = key(unit.id);
				units[unit_key] = null;
			},
			spawn_unit: (info) => {
				const definition = info.def == defs.planet_buster.id
					? defs.planet_buster
					: defs.defender;
				const unit = make_unit(
					info.id,
					definition,
					info.owner,
					info.tile,
					info.transport_id
				);
				unit.morale = info.morale;
				unit.health = info.health;
				unit.terraforming = info.terraforming;
				unit.terraforming_turns_remaining = info.terraforming_turns_remaining;
				unit.home_base_id = info.home_base_id;
				unit.fuel = info.fuel;
				return unit;
			},
		},
		bm: {
			get_bases: () => {
				let result = [];
				for (base_key in bases) {
					const active_base = bases[base_key];
					if (active_base != null) {
						result :+active_base;
					}
				}
				return result;
			},
			snapshot_base: (base) => {
				const snapshot = 'base:' + #to_string(base.id);
				base_snapshots[snapshot] = base;
				return snapshot;
			},
			despawn_base: (id) => {
				const base_key = key(id);
				const base = bases[base_key];
				const tile = base.get_tile();
				tile.base = null;
				bases[base_key] = null;
			},
			restore_base: (snapshot) => {
				const base = base_snapshots[snapshot];
				const base_key = key(base.id);
				bases[base_key] = base;
				const tile = base.get_tile();
				tile.base = base;
				return base;
			},
		},
		am: {
			show_animations: (animations) => {
				test.assert(#sizeof(animations) == 1);
				return 44;
			},
			stop_animations: (id) => { stopped_animation = id; },
		},
		is_turn_complete: (id) => { return false; },
		get_player: (id) => { return players[id]; },
		get_players: () => { return players; },
		trigger: (name, data) => { triggers :+name; },
		message: (message) => { messages :+message; },
		get: (name) => {
			if (name == 'f_council_is_un_charter_repealed') {
				return () => { return charter_repealed == true; };
			}
			if (name == 'f_diplomacy_snapshot_pair') {
				return (player, other) => {
					const other_key = key(other.id);
					const player_key = key(player.id);
					return {
						player_relation: player.relations[other_key],
						other_relation: other.relations[player_key],
					};
				};
			}
			if (name == 'f_diplomacy_set_bilateral_relation') {
				return (player, other, relation) => {
					const other_key = key(other.id);
					const player_key = key(player.id);
					player.relations[other_key] = relation;
					other.relations[player_key] = relation;
				};
			}
			if (name == 'f_diplomacy_clear_offers') {
				return (player, other) => {};
			}
			if (name == 'f_diplomacy_restore_pair') {
				return (player, other, snapshot) => {
					const other_key = key(other.id);
					const player_key = key(player.id);
					player.relations[other_key] = snapshot.player_relation;
					other.relations[player_key] = snapshot.other_relation;
				};
			}
			throw Error('unexpected game function: ' + name);
		},
	};

	return {
		game: game,
		actor: actor,
		victim: victim,
		bystander: bystander,
		observer: observer,
		missile: missile,
		defender: defender,
		collateral: collateral,
		safe: safe,
		target_base: target_base,
		support_base: support_base,
		center: center,
		locked_test_tile: ring[2],
		outside: outside,
		units: units,
		bases: bases,
		messages: messages,
		get_message_count: () => { return #sizeof(messages); },
		get_stopped_animation: () => { return stopped_animation; },
		get_crater_apply_count: () => { return crater_apply_count; },
		get_crater_restore_count: () => { return crater_restore_count; },
	};
};

let world = make_world(1, 0, 0);
let event = {
	caller: world.actor.id,
	game: world.game,
	data: {unit: world.missile, tile: world.center},
};
test.assert(!#is_defined(planet_buster.validate(event)));
event.caller = world.victim.id;
test.assert(#is_defined(planet_buster.validate(event)));
event.caller = world.actor.id;
let missile_definition = world.missile.get_def();
missile_definition.weapon = 'ConventionalPayload';
test.assert(#is_defined(planet_buster.validate(event)));
missile_definition.weapon = 'PlanetBuster';
missile_definition.reactor_power = 5;
test.assert(#is_defined(planet_buster.validate(event)));
missile_definition.reactor_power = 1;
world.missile.movement = 0.0;
test.assert(#is_defined(planet_buster.validate(event)));
world.missile.movement = 1.0;
event.data.tile = world.outside;
test.assert(#is_defined(planet_buster.validate(event)));
event.data.tile = world.center;
let locked_blast_tile = world.locked_test_tile;
locked_blast_tile.locked = true;
test.assert(#is_defined(planet_buster.validate(event)));
locked_blast_tile.locked = false;

event.resolved = planet_buster.resolve(event);
test.assert(event.resolved.radius == 1);
test.assert(!event.resolved.defense.intercepted);
event.applied = planet_buster.apply(event);
test.assert(!world.game.um.has_unit(world.missile.id));
test.assert(!world.game.um.has_unit(world.defender.id));
test.assert(!world.game.um.has_unit(world.collateral.id));
test.assert(world.game.um.has_unit(world.safe.id));
test.assert(world.game.um.get_unit(world.safe.id).home_base_id == world.support_base.id);
test.assert(world.center.get_base() == null);
let live_actor = world.game.get_player(world.actor.id);
test.assert(live_actor.get_major_atrocities() == 3);
test.assert(live_actor.get_sanction_turns() == 23);
let victim_key = key(world.victim.id);
let bystander_key = key(world.bystander.id);
let observer_key = key(world.observer.id);
test.assert(live_actor.relations[victim_key] == 'vendetta');
test.assert(live_actor.relations[bystander_key] == 'vendetta');
test.assert(live_actor.relations[observer_key] == 'vendetta');
test.assert(world.get_message_count() == 2);
test.assert(world.get_crater_apply_count() == 1);

planet_buster.rollback(event);
test.assert(world.game.um.has_unit(world.missile.id));
test.assert(world.game.um.has_unit(world.defender.id));
test.assert(world.game.um.has_unit(world.collateral.id));
test.assert(world.game.um.has_unit(world.safe.id));
test.assert(world.game.um.get_unit(world.safe.id).home_base_id == world.target_base.id);
test.assert(world.center.get_base() != null);
live_actor = world.game.get_player(world.actor.id);
test.assert(live_actor.get_major_atrocities() == 2);
test.assert(live_actor.get_sanction_turns() == 3);
test.assert(live_actor.relations[victim_key] == 'neutral');
test.assert(live_actor.relations[bystander_key] == 'neutral');
test.assert(live_actor.relations[observer_key] == 'neutral');
test.assert(world.get_stopped_animation() == 44);
test.assert(world.get_crater_restore_count() == 1);

world = make_world(1, 0, 0, true);
event = {caller: world.actor.id, game: world.game, data: {unit: world.missile, tile: world.center}};
event.resolved = planet_buster.resolve(event);
event.applied = planet_buster.apply(event);
live_actor = world.game.get_player(world.actor.id);
victim_key = key(world.victim.id);
bystander_key = key(world.bystander.id);
observer_key = key(world.observer.id);
test.assert(live_actor.get_major_atrocities() == 3);
test.assert(live_actor.get_sanction_turns() == 3);
test.assert(live_actor.relations[victim_key] == 'vendetta');
test.assert(live_actor.relations[bystander_key] == 'vendetta');
test.assert(live_actor.relations[observer_key] == 'neutral');
test.assert(world.get_message_count() == 1);
planet_buster.rollback(event);
live_actor = world.game.get_player(world.actor.id);
test.assert(live_actor.get_major_atrocities() == 2);
test.assert(live_actor.get_sanction_turns() == 3);
test.assert(live_actor.relations[victim_key] == 'neutral');
test.assert(live_actor.relations[bystander_key] == 'neutral');
test.assert(live_actor.relations[observer_key] == 'neutral');

world = make_world(0, 1, 0);
event = {caller: world.actor.id, game: world.game, data: {unit: world.missile, tile: world.center}};
event.resolved = planet_buster.resolve(event);
test.assert(event.resolved.defense.attempted);
test.assert(event.resolved.defense.intercepted);
test.assert(!event.resolved.defense.sacrificed);
event.applied = planet_buster.apply(event);
let live_victim = world.game.get_player(world.victim.id);
test.assert(
	live_victim.get_orbital_facility_count('OrbitalDefensePod') == 1 &&
	live_victim.get_orbital_defense_deployments() == 1
);
test.assert(world.game.um.has_unit(world.defender.id));
test.assert(world.center.get_base() != null);
test.assert(!world.game.um.has_unit(world.missile.id));
test.assert(world.get_crater_apply_count() == 0);
planet_buster.rollback(event);
live_victim = world.game.get_player(world.victim.id);
test.assert(
	live_victim.get_orbital_facility_count('OrbitalDefensePod') == 1 &&
	live_victim.get_orbital_defense_deployments() == 0
);
test.assert(world.game.um.has_unit(world.missile.id));
test.assert(world.get_crater_restore_count() == 0);

world = make_world(1, 1, 0);
event = {caller: world.actor.id, game: world.game, data: {unit: world.missile, tile: world.center}};
event.resolved = planet_buster.resolve(event);
test.assert(event.resolved.defense.attempted && !event.resolved.defense.intercepted);
event.applied = planet_buster.apply(event);
live_victim = world.game.get_player(world.victim.id);
test.assert(live_victim.get_orbital_defense_deployments() == 1);
test.assert(world.center.get_base() == null);
planet_buster.rollback(event);
live_victim = world.game.get_player(world.victim.id);
test.assert(live_victim.get_orbital_defense_deployments() == 0);
test.assert(world.center.get_base() != null);

world = make_world(1, 1, 1);
event = {caller: world.actor.id, game: world.game, data: {unit: world.missile, tile: world.center}};
event.resolved = planet_buster.resolve(event);
test.assert(event.resolved.defense.intercepted && event.resolved.defense.sacrificed);
event.applied = planet_buster.apply(event);
live_victim = world.game.get_player(world.victim.id);
test.assert(
	live_victim.get_orbital_facility_count('OrbitalDefensePod') == 0 &&
	live_victim.get_orbital_defense_deployments() == 0
);
test.assert(world.center.get_base() != null);
planet_buster.rollback(event);
live_victim = world.game.get_player(world.victim.id);
test.assert(
	live_victim.get_orbital_facility_count('OrbitalDefensePod') == 1 &&
	live_victim.get_orbital_defense_deployments() == 1
);
