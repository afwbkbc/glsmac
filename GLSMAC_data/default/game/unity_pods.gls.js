const technology_acquisition = #include('./technology_acquisition');
const native_life = #include('./native_life');

const MAX_ENERGY_CREDITS = 1000000000;
const TERRAFORMING_KEYS = [
	'road',
	'mag_tube',
	'forest',
	'farm',
	'soil_enricher',
	'solar',
	'mine',
	'condenser',
	'mirror',
	'borehole',
	'sensor',
	'bunker',
	'airbase',
	'remove_fungus',
	'plant_fungus',
];

const CLEAR_SURFACE_IMPROVEMENTS = {
	forest: false,
	farm: false,
	soil_enricher: false,
	solar: false,
	mine: false,
	condenser: false,
	mirror: false,
	borehole: false,
};

const get_bonus_name = (tile) => {
	if (tile.bonuses.nutrient) {
		return 'nutrient';
	}
	if (tile.bonuses.energy) {
		return 'energy';
	}
	if (tile.bonuses.minerals) {
		return 'minerals';
	}
	return 'none';
};

const snapshot_tile = (tile) => {
	let terraforming = {};
	for (key of TERRAFORMING_KEYS) {
		terraforming[key] = tile.terraforming[key];
	}
	return {
		tile: tile,
		features: {
			river: tile.features.river,
			monolith: tile.features.monolith,
			xenofungus: tile.features.xenofungus,
		},
		terraforming: terraforming,
		bonus: get_bonus_name(tile),
	};
};

const restore_tile = (snapshot) => {
	snapshot.tile.update_terraforming(snapshot.terraforming);
	snapshot.tile.update_features(snapshot.features);
	snapshot.tile.set_bonus(snapshot.bonus);
};

const get_unit_def = (game, id) => {
	for (definition of game.um.get_unit_defs()) {
		if (definition.id == id) {
			return definition;
		}
	}
	return null;
};

const get_player_energy = (player) => {
	return #typeof(player.get_energy_credits) == 'Callable'
		? player.get_energy_credits()
		: player.energy_credits;
};

const set_player_energy = (player, value) => {
	player.set_energy_credits(value);
};

const get_improvement_tiles = (center) => {
	let result = [];
	if (center.get_base() == null && !center.features.monolith) {
		result :+center;
	}
	for (tile of center.get_surrounding_tiles()) {
		if (
			tile.is_water == center.is_water &&
			tile.get_base() == null &&
			!tile.features.monolith
		) {
			result :+tile;
		}
	}
	return result;
};

const can_create_river = (tile) => {
	if (tile.is_water || tile.features.river || tile.get_base() != null) {
		return false;
	}
	for (nearby of tile.get_surrounding_tiles()) {
		if (nearby.is_water || nearby.features.river) {
			return false;
		}
	}
	return true;
};

const can_create_fungus = (game, tile) => {
	if (tile.is_water) {
		return false;
	}
	for (base of game.bm.get_bases()) {
		if (game.tm.get_distance(tile, base.get_tile()) < 3) {
			return false;
		}
	}
	return true;
};

const can_raise_terrain = (game, unit, tile) => {
	if (
		tile.is_water || tile.elevation >= 2000 || tile.get_base() != null ||
		#typeof(game.tm.apply_earthquake) != 'Callable'
	) {
		return false;
	}
	let base_count = 0;
	for (base of game.bm.get_bases()) {
		if (base.get_owner().id == unit.owner) {
			base_count++;
		}
	}
	if (base_count < 2) {
		return false;
	}
	for (nearby of tile.get_surrounding_tiles()) {
		if (nearby.is_water) {
			return false;
		}
	}
	return true;
};

const get_production_reward = (game, unit, tile) => {
	let best = null;
	let best_distance = 0;
	const get_cost = game.get('f_base_get_production_cost');
	for (base of game.bm.get_bases()) {
		if (base.get_owner().id != unit.owner) {
			continue;
		}
		const production = base.get_production();
		if (!#is_defined(production)) {
			continue;
		}
		const cost = #is_defined(get_cost)
			? get_cost(base, production)
			: production.mineral_cost;
		const accumulated = base.get_accumulated_minerals();
		if (cost <= 0 || accumulated >= #floor(#to_float(cost) * 0.75)) {
			continue;
		}
		const distance = game.tm.get_distance(tile, base.get_tile());
		if (
			best == null || distance < best_distance ||
			(distance == best_distance && base.id < best.base.id)
		) {
			best = {base: base, production: production, cost: cost};
			best_distance = distance;
		}
	}
	return best;
};

const get_artifact_transport_id = (unit, tile) => {
	if (!tile.is_water) {
		return 0;
	}
	const definition = unit.get_def();
	if (definition.cargo_capacity <= 0) {
		return 0 - 1;
	}
	return #sizeof(unit.get_cargo()) < definition.cargo_capacity
		? unit.id
		: 0 - 1;
};

const get_vehicle_definition = (game, unit, tile) => {
	if (tile.is_water) {
		return get_unit_def(game, 'UnityFoil');
	}
	const player = game.get_player(unit.owner);
	if (
		player.has_technology('SyntheticFossilFuels') &&
		game.random.get_int(0, 2) == 0
	) {
		const chopper = get_unit_def(game, 'UnityScoutChopper');
		if (chopper != null) {
			return chopper;
		}
	}
	return get_unit_def(game, 'UnityRover');
};

const make_resolution = (game, unit, tile, kind) => {
	if (kind == 'energy') {
		const late = game.get_turn() >= (tile.is_water ? 100 : 50);
		return {
			kind: kind,
			amount: game.random.get_int(1, tile.is_water ? 4 : 3) * (late ? 50 : 25),
		};
	}
	if (kind == 'river') {
		return can_create_river(tile) ? {kind: kind} : null;
	}
	if (kind == 'earthquake') {
		if (!can_raise_terrain(game, unit, tile)) {
			return null;
		}
		let broken_roads = [];
		let candidates = [tile];
		for (nearby of tile.get_surrounding_tiles()) {
			candidates :+nearby;
		}
		for (candidate of candidates) {
			if (candidate.terraforming.road && game.random.get_int(0, 2) == 0) {
				broken_roads :+candidate;
			}
		}
		return {kind: kind, elevation_steps: 2, broken_roads: broken_roads};
	}
	if (kind == 'production') {
		const reward = get_production_reward(game, unit, tile);
		return reward == null
			? null
			: {
				kind: kind,
				base: reward.base,
				production_name: reward.production.name,
				cost: reward.cost,
			};
	}
	if (kind == 'artifact') {
		const transport_id = get_artifact_transport_id(unit, tile);
		return get_unit_def(game, 'AlienArtifact') == null || transport_id < 0
			? null
			: {kind: kind, transport_id: transport_id};
	}
	if (kind == 'fungus') {
		return can_create_fungus(game, tile)
			? {kind: kind, tiles: get_improvement_tiles(tile)}
			: null;
	}
	if (kind == 'monolith') {
		return !tile.is_water && tile.get_base() == null && get_bonus_name(tile) == 'none'
			? {kind: kind}
			: null;
	}
	if (kind == 'vehicle') {
		const definition = get_vehicle_definition(game, unit, tile);
		return definition == null ? null : {kind: kind, unit_def: definition.id};
	}
	if (kind == 'technology') {
		return technology_acquisition.can_grant(game, game.get_player(unit.owner))
			? {kind: kind}
			: null;
	}
	if (kind == 'terraforming') {
		const tiles = get_improvement_tiles(tile);
		if (#sizeof(tiles) == 0) {
			return null;
		}
		const types = ['forest', 'farm', 'mine', 'solar'];
		let improvement = 'farm';
		if (!tile.is_water) {
			const type_index = game.random.get_int(0, #sizeof(types) - 1);
			improvement = types[type_index];
		}
		return {
			kind: kind,
			tiles: tiles,
			improvement: improvement,
		};
	}
	if (kind == 'clone') {
		const definition = unit.get_def();
		return definition.offense > 0 && definition.weapon != 'AlienArtifact'
			? {kind: kind, unit_def: definition.id}
			: null;
	}
	if (kind == 'native') {
		const life_level = native_life.get_life_level(game);
		if (life_level <= 0) {
			return null;
		}
		const outbreak = native_life.resolve_outbreak(
			game,
			tile,
			game.random.get_int(1, life_level),
			false
		);
		return #sizeof(outbreak.spawns) == 0
			? null
			: {kind: kind, outbreak: outbreak};
	}
	if (kind == 'resource') {
		if (
			tile.get_base() != null || tile.features.xenofungus ||
			get_bonus_name(tile) != 'none'
		) {
			return null;
		}
		const bonuses = ['nutrient', 'energy', 'minerals'];
		const bonus_index = game.random.get_int(0, #sizeof(bonuses) - 1);
		return {
			kind: kind,
			bonus: bonuses[bonus_index],
		};
	}
	return null;
};

const get_weighted_kind = (roll) => {
	if (roll < 8) { return 'energy'; }
	if (roll < 14) { return 'river'; }
	if (roll < 18) { return 'earthquake'; }
	if (roll < 26) { return 'production'; }
	if (roll < 34) { return 'artifact'; }
	if (roll < 42) { return 'fungus'; }
	if (roll < 50) { return 'monolith'; }
	if (roll < 60) { return 'vehicle'; }
	if (roll < 68) { return 'technology'; }
	if (roll < 82) { return 'terraforming'; }
	if (roll < 83) { return 'clone'; }
	if (roll < 91) { return 'native'; }
	return 'resource';
};

const resolve = (game, unit, tile) => {
	const selected = get_weighted_kind(game.random.get_int(0, 99));
	let result = make_resolution(game, unit, tile, selected);
	if (result != null) {
		return result;
	}
	const fallbacks = [
		'energy',
		'river',
		'earthquake',
		'production',
		'artifact',
		'fungus',
		'monolith',
		'vehicle',
		'technology',
		'terraforming',
		'native',
		'resource',
	];
	for (kind of fallbacks) {
		result = make_resolution(game, unit, tile, kind);
		if (result != null) {
			return result;
		}
	}
	throw Error('Unity Pod has no valid outcome');
};

const spawn_reward_unit = (game, source, tile, def_id, morale, health, transport_id) => {
	const unit = game.um.spawn_unit({
		def: def_id,
		owner: game.get_player(source.owner),
		tile: tile,
		morale: morale,
		health: health,
		home_base_id: 0,
		transport_id: transport_id,
	});
	unit.movement = 0.0;
	unit.moved_this_turn = true;
	return unit;
};

const get_terraforming_changes = (type) => {
	if (type == 'forest') {
		return {
			forest: true,
			farm: false,
			soil_enricher: false,
			solar: false,
			mine: false,
			condenser: false,
			mirror: false,
			borehole: false,
		};
	}
	if (type == 'farm') {
		return {forest: false, borehole: false, farm: true};
	}
	if (type == 'mine') {
		return {forest: false, solar: false, mirror: false, borehole: false, mine: true, road: true};
	}
	return {forest: false, mine: false, mirror: false, borehole: false, solar: true};
};

const apply = (game, unit, tile, resolved) => {
	const player = game.get_player(unit.owner);
	let applied = {
		kind: resolved.kind,
		tile: tile,
		player: player,
		pod_was_present: tile.features.unity_pod,
		tiles: [],
		spawned_unit_id: 0,
		native_outbreak: null,
		energy_credits: 0,
		base: null,
		base_minerals: 0,
		research: #undefined,
		terrain_snapshot: null,
		unit_state: null,
	};
	tile.update_features({unity_pod: false});

	if (resolved.kind == 'energy') {
		applied.energy_credits = get_player_energy(player);
		set_player_energy(player, #min(MAX_ENERGY_CREDITS, applied.energy_credits + resolved.amount));
		game.message(player.name + ' recovered ' + #to_string(resolved.amount) + ' energy credits from a Unity Pod.');
	} else if (resolved.kind == 'river') {
		applied.tiles :+snapshot_tile(tile);
		tile.update_features({river: true});
		game.message('A Unity hydrology pod tapped an underground river.');
	} else if (resolved.kind == 'earthquake') {
		applied.terrain_snapshot = game.tm.apply_earthquake(tile, resolved.elevation_steps);
		for (road_tile of resolved.broken_roads) {
			road_tile.update_terraforming({road: false, mag_tube: false});
		}
		game.message('A Unity Pod triggered a major earthquake and raised the surrounding terrain.');
	} else if (resolved.kind == 'production') {
		applied.base = resolved.base;
		applied.base_minerals = resolved.base.get_accumulated_minerals();
		resolved.base.set_accumulated_minerals(resolved.cost);
		game.trigger('update_base', {base: resolved.base});
		game.message('A Unity Pod completed ' + resolved.production_name + ' at ' + resolved.base.name + '.');
	} else if (resolved.kind == 'artifact') {
		const spawned = spawn_reward_unit(
			game,
			unit,
			tile,
			'AlienArtifact',
			2,
			1.0,
			resolved.transport_id
		);
		applied.spawned_unit_id = spawned.id;
		game.message(player.name + ' discovered an Alien Artifact in a Unity Pod.');
	} else if (resolved.kind == 'fungus') {
		for (fungus_tile of resolved.tiles) {
			applied.tiles :+snapshot_tile(fungus_tile);
			fungus_tile.update_terraforming(CLEAR_SURFACE_IMPROVEMENTS);
			fungus_tile.update_features({xenofungus: true});
		}
		game.message('A Unity Pod released an uncontrolled xenofungal bloom.');
	} else if (resolved.kind == 'monolith') {
		applied.tiles :+snapshot_tile(tile);
		applied.unit_state = {
			unit: unit,
			morale: unit.morale,
			health: unit.health,
		};
		tile.update_features({monolith: true, xenofungus: false});
		unit.health = 1.0;
		unit.morale = #min(6, unit.morale + 1);
		game.message('A Unity Pod revealed a monolith that repaired and trained the exploring unit.');
	} else if (resolved.kind == 'vehicle' || resolved.kind == 'clone') {
		const spawned = spawn_reward_unit(
			game,
			unit,
			tile,
			resolved.unit_def,
			resolved.kind == 'clone' ? unit.morale : 2,
			resolved.kind == 'clone' ? unit.health : 1.0,
			0
		);
		applied.spawned_unit_id = spawned.id;
		game.message(
			resolved.kind == 'clone'
				? 'A dimensional rift created a copy of ' + unit.get_def().name + '.'
				: player.name + ' recovered a ' + spawned.get_def().name + ' from a Unity Pod.'
		);
	} else if (resolved.kind == 'technology') {
		applied.research = technology_acquisition.apply(game, player, 1);
		if (!#is_defined(applied.research)) {
			throw Error('Unity Pod did not discover a technology');
		}
		for (name of applied.research.completed_names) {
			game.message(player.name + ' recovered ' + name + ' from a Unity data pod.');
		}
	} else if (resolved.kind == 'terraforming') {
		const changes = get_terraforming_changes(resolved.improvement);
		for (improved_tile of resolved.tiles) {
			applied.tiles :+snapshot_tile(improved_tile);
			improved_tile.update_features({xenofungus: false});
			improved_tile.update_terraforming(changes);
		}
		const label = tile.is_water
			? 'kelp farms'
			: resolved.improvement + ' improvements';
		game.message('A Unity terraforming pod established nearby ' + label + '.');
	} else if (resolved.kind == 'native') {
		applied.native_outbreak = native_life.apply_outbreak(game, resolved.outbreak);
		game.message('A Unity Pod disturbed a nest of native lifeforms.');
	} else if (resolved.kind == 'resource') {
		applied.tiles :+snapshot_tile(tile);
		tile.set_bonus(resolved.bonus);
		game.message('A Unity Pod revealed a permanent ' + resolved.bonus + ' resource deposit.');
	} else {
		throw Error('Unknown Unity Pod outcome: ' + resolved.kind);
	}

	game.trigger('unity_pod_opened', {
		player: player,
		unit: unit,
		tile: tile,
		outcome: resolved.kind,
	});
	return applied;
};

const rollback = (game, applied) => {
	if (applied.native_outbreak != null) {
		native_life.rollback_outbreak(game, applied.native_outbreak);
	}
	if (#is_defined(applied.research)) {
		technology_acquisition.rollback(game, applied.research);
	}
	if (applied.spawned_unit_id > 0 && game.um.has_unit(applied.spawned_unit_id)) {
		game.um.despawn_unit(game.um.get_unit(applied.spawned_unit_id));
	}
	if (applied.base != null) {
		applied.base.set_accumulated_minerals(applied.base_minerals);
		game.trigger('update_base', {base: applied.base});
	}
	if (applied.terrain_snapshot != null) {
		game.tm.restore_terrain(applied.terrain_snapshot);
	}
	for (let i = #sizeof(applied.tiles) - 1; i >= 0; i--) {
		restore_tile(applied.tiles[i]);
	}
	if (applied.unit_state != null) {
		const unit = applied.unit_state.unit;
		unit.morale = applied.unit_state.morale;
		unit.health = applied.unit_state.health;
	}
	if (applied.kind == 'energy') {
		set_player_energy(applied.player, applied.energy_credits);
	}
	applied.tile.update_features({unity_pod: applied.pod_was_present});
};

return {
	make_resolution: make_resolution,
	resolve: resolve,
	apply: apply,
	rollback: rollback,
};
