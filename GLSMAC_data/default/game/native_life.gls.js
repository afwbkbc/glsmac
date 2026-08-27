const is_empty = (tile) => {
	if (tile.get_base() != null) {
		return false;
	}
	return #typeof(tile.get_units) != 'Callable' || #sizeof(tile.get_units()) == 0;
};

const get_candidates = (center, include_center) => {
	let candidates = [];
	if (include_center && is_empty(center)) {
		candidates :+center;
	}
	for (tile of center.get_surrounding_tiles()) {
		if (tile.is_water == center.is_water && is_empty(tile)) {
			candidates :+tile;
		}
	}
	return candidates;
};

const rotate_candidates = (game, candidates) => {
	if (#sizeof(candidates) < 2) {
		return candidates;
	}
	const start = game.random.get_int(0, #sizeof(candidates) - 1);
	let result = [];
	for (let i = 0; i < #sizeof(candidates); i++) {
		result :+candidates[(start + i) % #sizeof(candidates)];
	}
	return result;
};

const get_life_level = (game) => {
	if (#typeof(game.get_settings) == 'Callable') {
		const settings = game.get_settings();
		if (
			#typeof(settings.global) != 'Object' ||
			#typeof(settings.global.map) != 'Object' ||
			!#is_defined(settings.global.map.native_lifeforms)
		) {
			return 2;
		}
		const density = settings.global.map.native_lifeforms;
		if (density <= 0.0) {
			return 0;
		}
		return #max(1, #min(3, #round(density * 4.0)));
	}
	return 2;
};

const resolve_outbreak = (game, center, requested_count, include_center) => {
	const candidates = rotate_candidates(game, get_candidates(center, include_center));
	if (#sizeof(candidates) == 0 || requested_count <= 0) {
		return {spawns: []};
	}
	const count = #min(requested_count, center.is_water ? 5 : #sizeof(candidates));
	let spawns = [];
	if (center.is_water) {
		spawns :+{
			def: 'IsleOfTheDeep',
			tile: candidates[0],
			morale: 1,
			transport_index: 0 - 1,
		};
		for (let i = 1; i < count; i++) {
			spawns :+{
				def: 'MindWorms',
				tile: candidates[0],
				morale: 1,
				transport_index: 0,
			};
		}
	} else {
		for (let i = 0; i < count; i++) {
			spawns :+{
				def: 'MindWorms',
				tile: candidates[i],
				morale: 1,
				transport_index: 0 - 1,
			};
		}
	}
	return {spawns: spawns};
};

const apply_outbreak = (game, outbreak) => {
	const owner = game.get_native_player();
	let units = [];
	for (spawn of outbreak.spawns) {
		let transport_id = 0;
		if (spawn.transport_index >= 0) {
			if (spawn.transport_index >= #sizeof(units)) {
				throw Error('Native outbreak transport index is out of bounds');
			}
			transport_id = units[spawn.transport_index].id;
		}
		const unit = game.um.spawn_unit({
			def: spawn.def,
			owner: owner,
			tile: spawn.tile,
			morale: spawn.morale,
			health: 1.0,
			home_base_id: 0,
			transport_id: transport_id,
		});
		unit.movement = 0.0;
		unit.moved_this_turn = true;
		units :+unit;
	}
	let ids = [];
	for (unit of units) {
		ids :+unit.id;
	}
	return {unit_ids: ids};
};

const rollback_outbreak = (game, applied) => {
	for (let i = #sizeof(applied.unit_ids) - 1; i >= 0; i--) {
		const id = applied.unit_ids[i];
		if (game.um.has_unit(id)) {
			game.um.despawn_unit(game.um.get_unit(id));
		}
	}
};

return {
	get_life_level: get_life_level,
	resolve_outbreak: resolve_outbreak,
	apply_outbreak: apply_outbreak,
	rollback_outbreak: rollback_outbreak,
};
