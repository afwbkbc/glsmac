const MAX_BASE_NAME_LENGTH = 64;
const snapshot_unit = #include('../entity_snapshots').snapshot_unit;

const restore_unit = (e, backup) => {
	const unit = e.game.um.spawn_unit({
		id: backup.id,
		def: backup.def,
		owner: e.game.get_player(backup.owner),
		tile: e.game.tm.get_tile(backup.tile_x, backup.tile_y),
		morale: backup.morale,
		health: backup.health,
		terraforming: backup.terraforming,
		terraforming_turns_remaining: backup.terraforming_turns_remaining,
		home_base_id: backup.home_base_id,
		fuel: backup.fuel,
		transport_id: backup.transport_id,
	});
	unit.movement = backup.movement;
	unit.moved_this_turn = backup.moved_this_turn;
	unit.native_capture_attempted = backup.native_capture_attempted;
};

const get_initial_production = (game, owner, tile) => {
	if (!tile.is_water) {
		return 'ScoutPatrol';
	}
	let best = null;
	for (def of game.um.get_unit_defs()) {
		if (
			!def.is_water || def.is_native || def.is_missile ||
			def.offense <= 0 || def.can_found_base || def.can_terraform ||
			def.cargo_capacity > 0 ||
			(
				def.required_technology != '' &&
				!owner.has_technology(def.required_technology)
			)
		) {
			continue;
		}
		if (best == null || def.mineral_cost < best.mineral_cost) {
			best = def;
		}
	}
	return best == null ? 'SeaLurk' : best.id;
};

return {
	get_initial_production: get_initial_production,

	validate: (e) => {
		const unit = e.data.unit;
		if (#is_defined(e.data.name)) {
			if (#typeof(e.data.name) != 'String' || e.data.name == '') {
				return 'Base name must be a non-empty string';
			}
			if (#sizeof(e.data.name) > MAX_BASE_NAME_LENGTH) {
				return 'Base name is too long';
			}
		}
		if (unit.owner != e.caller) {
			return 'A colony pod can only be ordered by its owner';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (unit.health <= 0.0) {
			return 'Dead unit cannot found a base';
		}
		if (#is_defined(unit.transport_id) && unit.transport_id > 0) {
			return 'Embarked unit cannot found a base';
		}
		if (!unit.get_def().can_found_base) {
			return 'Unit cannot found a base';
		}
		if (unit.movement <= 0.0) {
			return 'Colony pod is out of moves';
		}

		const tile = unit.get_tile();
		if (tile.is_locked()) {
			return 'Base site is locked';
		}
		if (!unit.is_land && !unit.is_water) {
			return 'Only land or sea colony pods can found bases';
		}
		if (tile.is_water && !unit.is_water) {
			return 'Only sea colony pods can found ocean bases';
		}
		if (!tile.is_water && !unit.is_land) {
			return 'Sea colony pods can only found ocean bases';
		}
		if (tile.get_base() != null) {
			return 'Tile already contains a base';
		}
		for (nearby of tile.get_surrounding_tiles()) {
			if (nearby.get_base() != null) {
				return 'Bases cannot be founded on adjacent tiles';
			}
		}
		for (occupant of tile.get_units()) {
			if (occupant.owner != e.caller) {
				return 'Base site contains an enemy unit';
			}
		}
	},

	resolve: (e) => {
		// Defer client-side creation until the server accepts the event so base IDs stay ordered.
		return {};
	},

	apply: (e) => {
		const unit = e.data.unit;
		const tile = unit.get_tile();
		const owner = unit.get_owner();
		const backup = snapshot_unit(unit);
		let info = {
			production: get_initial_production(e.game, owner, tile),
		};
		if (#is_defined(e.data.name)) {
			info.name = e.data.name;
		}

		const base = e.game.bm.spawn_base(owner, tile, info);
		const get_new_base_minerals = #is_defined(e.game.get)
			? e.game.get('f_social_get_new_base_minerals')
			: #undefined;
		base.set_accumulated_minerals(
			#is_defined(get_new_base_minerals) ? get_new_base_minerals(owner) : 10
		);
		e.game.um.despawn_unit(unit);

		const get_project_effects = #is_defined(e.game.get)
			? e.game.get('f_project_get_player_effects')
			: #undefined;
		const project_effects = #is_defined(get_project_effects)
			? get_project_effects(owner)
			: {new_base_population: 1};
		const initial_population = #max(project_effects.new_base_population, 1);
		let pops = [];
		for (let pop_index = 0; pop_index < initial_population; pop_index++) {
			pops :+base.create_pop({type: 'WORKER'});
		}
		let unoccupied = [];
		for (candidate of base.get_unworked_tiles()) {
			if (candidate.get_base() == null && !candidate.has('working_pop')) {
				unoccupied :+candidate;
			}
		}
		const workable = e.game.get('f_base_find_best_or_worst_tiles')(
			base,
			unoccupied,
			initial_population,
			1
		);
		for (
			let work_index = 0;
			work_index < #sizeof(workable) && work_index < #sizeof(pops);
			work_index++
		) {
			e.game.get('f_base_pop_work_tile')(
				base,
				pops[work_index],
				workable[work_index]
			);
		}

		return {
			base: base,
			unit: backup,
		};
	},

	rollback: (e) => {
		e.game.bm.despawn_base(e.applied.base);
		restore_unit(e, e.applied.unit);
	},

};
