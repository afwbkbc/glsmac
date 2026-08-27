const combat_rules = #include('./combat_rules');
const movement_rules = #include('./movement_rules');
const native_life = #include('./native_life');

const MAX_TARGET_DISTANCE = 12;

const get_um = (game) => {
	if (#is_defined(game.um)) {
		return game.um;
	}
	return game.get_um();
};

const get_tm = (game) => {
	if (#is_defined(game.tm)) {
		return game.tm;
	}
	return game.get_tm();
};

const get_transport_id = (unit) => {
	return #is_defined(unit.transport_id) ? unit.transport_id : 0;
};

const is_wild = (unit, native) => {
	return unit.owner == native.id;
};

const can_reach_surface = (unit, tile) => {
	if (unit.is_air) {
		return true;
	}
	return (unit.is_land && tile.is_land) || (unit.is_water && tile.is_water);
};

const can_enter = (unit, tile) => {
	if (!can_reach_surface(unit, tile) || tile.is_locked()) {
		return false;
	}
	const base = tile.get_base();
	if (base != null && base.get_owner().id != unit.owner) {
		return false;
	}
	for (other of tile.get_units()) {
		if (other.owner != unit.owner) {
			return false;
		}
	}
	return !movement_rules.is_zoc_move_blocked(unit, unit.get_tile(), tile);
};

const can_attack_tile = (unit, tile) => {
	if (unit.is_land && tile.is_water) {
		return false;
	}
	if (unit.is_water && tile.is_land && tile.get_base() == null) {
		return false;
	}
	return true;
};

const choose_adjacent_defender = (game, unit) => {
	let best = null;
	let best_score = 0.0;
	for (tile of unit.get_tile().get_surrounding_tiles()) {
		if (tile.is_locked() || !can_attack_tile(unit, tile)) {
			continue;
		}
		for (defender of tile.get_units()) {
			if (defender.owner == unit.owner || defender.health <= 0.0) {
				continue;
			}
			const score = combat_rules.get_attack_score(unit, defender);
			if (
				best == null || score > best_score ||
				(score == best_score && defender.id < best.id)
			) {
				best = defender;
				best_score = score;
			}
		}
	}
	return best;
};

const choose_target_tile = (game, unit, native) => {
	const tm = get_tm(game);
	let best = null;
	let best_distance = MAX_TARGET_DISTANCE + 1;
	for (other of get_um(game).get_units()) {
		if (
			is_wild(other, native) || other.health <= 0.0 ||
			get_transport_id(other) > 0 || !can_reach_surface(unit, other.get_tile())
		) {
			continue;
		}
		const tile = other.get_tile();
		const distance = tm.get_distance(unit.get_tile(), tile);
		if (
			distance < best_distance ||
			(
				distance == best_distance && best != null &&
				(tile.y < best.y || (tile.y == best.y && tile.x < best.x))
			)
		) {
			best = tile;
			best_distance = distance;
		}
	}
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id == native.id || !can_reach_surface(unit, base.get_tile())) {
			continue;
		}
		const tile = base.get_tile();
		const distance = tm.get_distance(unit.get_tile(), tile);
		if (
			distance < best_distance ||
			(
				distance == best_distance && best != null &&
				(tile.y < best.y || (tile.y == best.y && tile.x < best.x))
			)
		) {
			best = tile;
			best_distance = distance;
		}
	}
	if (best_distance > MAX_TARGET_DISTANCE) {
		return null;
	}
	return best;
};

const choose_step = (game, unit, target) => {
	const tm = get_tm(game);
	let best = null;
	let best_distance = target == null ? 0 : tm.get_distance(unit.get_tile(), target);
	let best_roll = 0;
	for (tile of unit.get_tile().get_surrounding_tiles()) {
		if (!can_enter(unit, tile)) {
			continue;
		}
		if (target == null) {
			const roll = game.random.get_int(0, 1000000);
			if (
				best == null || roll < best_roll ||
				(
					roll == best_roll &&
					(tile.y < best.y || (tile.y == best.y && tile.x < best.x))
				)
			) {
				best = tile;
				best_roll = roll;
			}
			continue;
		}
		const distance = tm.get_distance(tile, target);
		if (
			distance < best_distance ||
			(
				distance == best_distance && best != null &&
				(tile.y < best.y || (tile.y == best.y && tile.x < best.x))
			)
		) {
			best = tile;
			best_distance = distance;
		}
	}
	return best;
};

const choose_action = (game, unit) => {
	const native = game.get_native_player();
	const defender = choose_adjacent_defender(game, unit);
	if (defender != null) {
		return {kind: 'attack', defender_id: defender.id};
	}
	const target = choose_target_tile(game, unit, native);
	let step = choose_step(game, unit, target);
	if (step == null && target != null) {
		step = choose_step(game, unit, null);
	}
	if (step == null) {
		return null;
	}
	return {kind: 'move', tile_x: step.x, tile_y: step.y};
};

const select_ambient_spawn = (game) => {
	const life_level = native_life.get_life_level(game);
	if (life_level <= 0 || game.get_turn() <= 0) {
		return null;
	}
	const um = get_um(game);
	const native = game.get_native_player();
	let native_count = 0;
	for (unit of um.get_units()) {
		if (is_wild(unit, native)) {
			native_count++;
		}
	}
	const tm = get_tm(game);
	const tile_count = tm.get_map_width() * tm.get_map_height();
	const population_cap = #max(4, #floor(#to_float(tile_count * life_level) / 256.0));
	if (native_count >= population_cap || game.random.get_int(0, 99) >= life_level * 10) {
		return null;
	}
	let candidates = [];
	for (let y = 0; y < tm.get_map_height(); y++) {
		for (let x = 0; x < tm.get_map_width(); x++) {
			if (x % 2 != y % 2) {
				continue;
			}
			const tile = tm.get_tile(x, y);
			if (
				tile.features.xenofungus && tile.get_base() == null &&
				#sizeof(tile.get_units()) == 0 && !tile.is_locked()
			) {
				candidates :+tile;
			}
		}
	}
	if (#sizeof(candidates) == 0) {
		return null;
	}
	const candidate_index = game.random.get_int(0, #sizeof(candidates) - 1);
	const tile = candidates[candidate_index];
	let type = tile.is_water ? 'IsleOfTheDeep' : 'MindWorms';
	if (game.get_turn() >= 80 && game.random.get_int(0, 5) == 0) {
		type = 'LocustsOfChiron';
	}
	return {type: type, tile: tile};
};

const queue_ambient_spawn = (game) => {
	const spawn = select_ambient_spawn(game);
	if (spawn == null) {
		return false;
	}
	game.event('spawn_unit', {
		type: spawn.type,
		owner: game.get_native_player(),
		tile: spawn.tile,
		morale: 1,
		health: 1.0,
		home_base_id: 0,
		movement: 0.0,
		moved_this_turn: true,
	});
	return true;
};

return {
	can_enter: can_enter,
	choose_action: choose_action,
	select_ambient_spawn: select_ambient_spawn,
	queue_ambient_spawn: queue_ambient_spawn,
};
