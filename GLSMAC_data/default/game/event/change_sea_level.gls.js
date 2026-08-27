const entity_snapshots = #include('../entity_snapshots');
const MAX_SEA_LEVEL_CHANGE = 1000;
const PRESSURE_DOME = 'PressureDome';

const base_key = (base_id) => {
	return 'b' + #to_string(base_id);
};

const player_key = (player_id) => {
	return 'p' + #to_string(player_id);
};

const unit_key = (unit_id) => {
	return 'u' + #to_string(unit_id);
};

const get_tiles = (tm) => {
	let tiles = [];
	for (let y = 0; y < tm.get_map_height(); y++) {
		for (let x = 0; x < tm.get_map_width(); x++) {
			if (x % 2 == y % 2) {
				tiles :+tm.get_tile(x, y);
			}
		}
	}
	return tiles;
};

const find_base = (game, base_id) => {
	const bases = game.bm.get_bases();
	for (let i = 0; i < #sizeof(bases); i++) {
		if (bases[i].id == base_id) {
			return bases[i];
		}
	}
	return null;
};

const get_resolved_loss = (resolved, base_id) => {
	for (loss of resolved.base_losses) {
		if (loss.base_id == base_id) {
			return loss.population;
		}
	}
	return 0;
};

const snapshot_bases = (game) => {
	let result = [];
	const bases = game.bm.get_bases();
	for (let i = 0; i < #sizeof(bases); i++) {
		const id = bases[i].id;
		const name = bases[i].name;
		const owner_id = bases[i].get_owner().id;
		const was_water = bases[i].get_tile().is_water;
		const snapshot = game.bm.snapshot_base(bases[i]);
		result :+{
			id: id,
			name: name,
			owner_id: owner_id,
			was_water: was_water,
			base: bases[i],
			snapshot: snapshot,
		};
	}
	return result;
};

const collect_lost_units = (changed_tiles) => {
	let all_units = [];
	let seen = {};
	let lost_ids = {};
	for (let i = 0; i < #sizeof(changed_tiles); i++) {
		const units = changed_tiles[i].get_units(true);
		for (let j = 0; j < #sizeof(units); j++) {
			const key = unit_key(units[j].id);
			if (!#is_defined(seen[key])) {
				seen[key] = true;
				all_units :+units[j];
			}
			if (
				(units[j].is_land && changed_tiles[i].is_water) ||
				(units[j].is_water && changed_tiles[i].is_land)
			) {
				lost_ids[key] = true;
			}
		}
	}
	for (let k = 0; k < #sizeof(all_units); k++) {
		const transport_key = unit_key(all_units[k].transport_id);
		if (
			#is_defined(all_units[k].transport_id) && all_units[k].transport_id != 0 &&
			#is_defined(lost_ids[transport_key])
		) {
			const key = unit_key(all_units[k].id);
			lost_ids[key] = true;
		}
	}
	let result = [];
	for (let l = 0; l < #sizeof(all_units); l++) {
		const key = unit_key(all_units[l].id);
		if (#is_defined(lost_ids[key])) {
			result :+entity_snapshots.snapshot_unit(all_units[l]);
		}
	}
	return result;
};

return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only the host can change sea level';
		}
		if (
			#typeof(e.data.amount) != 'Int' || e.data.amount == 0 ||
			e.data.amount < 0 - MAX_SEA_LEVEL_CHANGE ||
			e.data.amount > MAX_SEA_LEVEL_CHANGE
		) {
			return 'Sea-level change must be a non-zero whole number no greater than 1000 metres';
		}
		const next_level = e.game.tm.get_sea_level() + e.data.amount;
		if (next_level < -3500 || next_level > 3500) {
			return 'Sea level would exceed the supported elevation range';
		}
	},

	resolve: (e) => {
		let base_losses = [];
		const bases = e.game.bm.get_bases();
		for (let i = 0; i < #sizeof(bases); i++) {
			const base_id = bases[i].id;
			const population = #min(
				10,
				#max(bases[i].get_size() / 2, e.game.random.get_int(2, 4))
			);
			base_losses :+{
				base_id: base_id,
				population: population,
			};
		}
		return {base_losses: base_losses};
	},

	apply: (e) => {
		const tiles = get_tiles(e.game.tm);
		let tile_states = [];
		for (let i = 0; i < #sizeof(tiles); i++) {
			tile_states :+{tile: tiles[i], was_water: tiles[i].is_water};
		}
		const bases = snapshot_bases(e.game);
		let base_counts = {};
		for (base of bases) {
			const key = player_key(base.owner_id);
			base_counts[key] = #is_defined(base_counts[key]) ? base_counts[key] + 1 : 1;
		}

		let applied = {
			terrain_snapshot: e.game.tm.apply_sea_level_change(e.data.amount),
			bases: [],
			units: [],
			rehomed_units: [],
		};
		let changed_tiles = [];
		for (let j = 0; j < #sizeof(tile_states); j++) {
			if (tile_states[j].tile.is_water != tile_states[j].was_water) {
				changed_tiles :+tile_states[j].tile;
			}
		}

		applied.units = collect_lost_units(changed_tiles);
		entity_snapshots.despawn_unit_snapshots(e.game, applied.units);

		let death_candidates = {};
		let protected_base_ids = {};
		for (info of bases) {
			if (info.was_water || !info.base.get_tile().is_water || info.base.has_facility(PRESSURE_DOME)) {
				continue;
			}
			const loss = get_resolved_loss(e.resolved, info.id);
			if (loss >= info.base.get_size()) {
				const key = player_key(info.owner_id);
				if (!#is_defined(death_candidates[key])) {
					death_candidates[key] = {count: 0, minimum_id: info.id};
				}
				const candidate = death_candidates[key];
				death_candidates[key] = {
					count: candidate.count + 1,
					minimum_id: #min(candidate.minimum_id, info.id),
				};
			}
		}
		for (key in death_candidates) {
			if (death_candidates[key].count >= base_counts[key]) {
				const protected_key = base_key(death_candidates[key].minimum_id);
				protected_base_ids[protected_key] = true;
			}
		}

		let destroyed_bases = [];
		for (info of bases) {
			if (info.was_water || !info.base.get_tile().is_water || info.base.has_facility(PRESSURE_DOME)) {
				continue;
			}
			const old_size = info.base.get_size();
			let target_size = #max(0, old_size - get_resolved_loss(e.resolved, info.id));
			const protected_key = base_key(info.id);
			if (#is_defined(protected_base_ids[protected_key])) {
				target_size = #max(target_size, 1);
			}
			info.base.add_facility(PRESSURE_DOME);
			while (info.base.get_size() > target_size) {
				const pops = info.base.get_pops();
				info.base.destroy_pop(pops[#sizeof(pops) - 1]);
			}
			const base_result = {
				id: info.id,
				name: info.name,
				owner_id: info.owner_id,
				snapshot: info.snapshot,
				casualties: old_size - target_size,
				destroyed: target_size == 0,
			};
			applied.bases :+base_result;
			if (base_result.destroyed) {
				destroyed_bases :+base_result;
				e.game.bm.despawn_base(info.id);
			}
		}
		applied.rehomed_units = entity_snapshots.rehome_surviving_units(
			e.game,
			destroyed_bases,
			0
		);

		const direction = e.data.amount > 0 ? 'rose' : 'fell';
		e.game.message(
			'Sea levels ' + direction + ' by ' + #to_string(#abs(e.data.amount)) + ' metres.'
		);
		for (base of applied.bases) {
			if (base.destroyed) {
				e.game.message(base.name + ' was submerged and lost.');
			} else {
				e.game.message(
					base.name + ' suffered ' + #to_string(base.casualties) +
					' population casualties before an emergency Pressure Dome was erected.'
				);
			}
		}
		if (#sizeof(applied.units) > 0) {
			e.game.message(
				#to_string(#sizeof(applied.units)) + ' units were lost to changing coastlines.'
			);
		}
		e.game.trigger('sea_level_changed', {
			amount: e.data.amount,
			level: e.game.tm.get_sea_level(),
		});
		return applied;
	},

	rollback: (e) => {
		e.game.tm.restore_sea_level(e.applied.terrain_snapshot);
		for (snapshot of e.applied.bases) {
			const active = find_base(e.game, snapshot.id);
			if (active != null) {
				e.game.bm.despawn_base(active);
			}
			e.game.bm.restore_base(snapshot.snapshot);
		}
		entity_snapshots.spawn_unit_snapshots(e.game, e.applied.units);
		entity_snapshots.restore_rehomed_units(e.game, e.applied.rehomed_units);
	},

};
