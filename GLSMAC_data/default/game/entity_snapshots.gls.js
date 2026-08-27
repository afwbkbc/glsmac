const snapshot_unit = (unit) => {
	const tile = unit.get_tile();
	return {
		id: unit.id + 0,
		def: '' + unit.def,
		owner: unit.owner + 0,
		tile_x: tile.x + 0,
		tile_y: tile.y + 0,
		movement: unit.movement + 0.0,
		morale: unit.morale + 0,
		health: unit.health + 0.0,
		moved_this_turn: unit.moved_this_turn == true,
		terraforming: #is_defined(unit.terraforming) ? '' + unit.terraforming : 'none',
		terraforming_turns_remaining: #is_defined(unit.terraforming_turns_remaining)
			? unit.terraforming_turns_remaining + 0 : 0,
		home_base_id: #is_defined(unit.home_base_id) ? unit.home_base_id + 0 : 0,
		fuel: #is_defined(unit.fuel) ? unit.fuel + 0 : 0,
		transport_id: #is_defined(unit.transport_id) ? unit.transport_id + 0 : 0,
		native_capture_attempted: #is_defined(unit.native_capture_attempted)
			? unit.native_capture_attempted == true : false,
	};
};

const spawn_unit_snapshot_as = (game, snapshot, owner, transferred) => {
	const unit = game.um.spawn_unit({
		id: snapshot.id,
		def: snapshot.def,
		owner: owner,
		tile: game.tm.get_tile(snapshot.tile_x, snapshot.tile_y),
		morale: snapshot.morale,
		health: snapshot.health,
		terraforming: transferred ? 'none' : snapshot.terraforming,
		terraforming_turns_remaining: transferred ? 0 : snapshot.terraforming_turns_remaining,
		home_base_id: transferred ? 0 : snapshot.home_base_id,
		fuel: snapshot.fuel,
		transport_id: snapshot.transport_id,
	});
	unit.movement = transferred ? 0.0 : snapshot.movement;
	unit.moved_this_turn = transferred ? true : snapshot.moved_this_turn;
	unit.native_capture_attempted = transferred ? false : snapshot.native_capture_attempted;
	return unit;
};

const spawn_unit_snapshot = (game, snapshot) => {
	return spawn_unit_snapshot_as(game, snapshot, game.get_player(snapshot.owner), false);
};

const despawn_unit_snapshots = (game, snapshots) => {
	for (snapshot of snapshots) {
		if (snapshot.transport_id != 0 && game.um.has_unit(snapshot.id)) {
			game.um.despawn_unit(game.um.get_unit(snapshot.id));
		}
	}
	for (let i = #sizeof(snapshots) - 1; i >= 0; i--) {
		const snapshot = snapshots[i];
		if (snapshot.transport_id == 0 && game.um.has_unit(snapshot.id)) {
			game.um.despawn_unit(game.um.get_unit(snapshot.id));
		}
	}
};

const spawn_unit_snapshots = (game, snapshots) => {
	for (snapshot of snapshots) {
		if (snapshot.transport_id == 0) {
			spawn_unit_snapshot(game, snapshot);
		}
	}
	for (snapshot of snapshots) {
		if (snapshot.transport_id != 0) {
			spawn_unit_snapshot(game, snapshot);
		}
	}
};

const spawn_unit_snapshots_as = (game, snapshots, owner, transferred) => {
	for (snapshot of snapshots) {
		if (snapshot.transport_id == 0) {
			spawn_unit_snapshot_as(game, snapshot, owner, transferred);
		}
	}
	for (snapshot of snapshots) {
		if (snapshot.transport_id != 0) {
			spawn_unit_snapshot_as(game, snapshot, owner, transferred);
		}
	}
};

const base_key = (base_id) => {
	return 'b' + #to_string(base_id);
};

const rehome_surviving_units = (game, destroyed_bases, excluded_unit_id) => {
	let destroyed_ids = {};
	for (snapshot of destroyed_bases) {
		const destroyed_key = base_key(snapshot.id);
		destroyed_ids[destroyed_key] = true;
	}
	let result = [];
	const units = game.um.get_units();
	for (let i = 0; i < #sizeof(units); i++) {
		const home_key = base_key(units[i].home_base_id);
		if (
			units[i].id == excluded_unit_id || units[i].home_base_id == 0 ||
			!#is_defined(destroyed_ids[home_key])
		) {
			continue;
		}
		let destination = null;
		let best_distance = 0;
		const bases = game.bm.get_bases();
		for (let j = 0; j < #sizeof(bases); j++) {
			const candidate_key = base_key(bases[j].id);
			if (
				bases[j].get_owner().id != units[i].owner ||
				#is_defined(destroyed_ids[candidate_key])
			) {
				continue;
			}
			const distance = game.tm.get_distance(units[i].get_tile(), bases[j].get_tile());
			if (
				destination == null || distance < best_distance ||
				(distance == best_distance && bases[j].id < destination.id)
			) {
				destination = bases[j];
				best_distance = distance;
			}
		}
		result :+{
			unit_id: units[i].id,
			home_base_id: units[i].home_base_id,
		};
		units[i].set_home_base_id(destination == null ? 0 : destination.id);
	}
	return result;
};

const restore_rehomed_units = (game, snapshots) => {
	for (snapshot of snapshots) {
		if (game.um.has_unit(snapshot.unit_id)) {
			game.um.get_unit(snapshot.unit_id).set_home_base_id(snapshot.home_base_id);
		}
	}
};

return {
	snapshot_unit: snapshot_unit,
	spawn_unit_snapshot: spawn_unit_snapshot,
	spawn_unit_snapshot_as: spawn_unit_snapshot_as,
	despawn_unit_snapshots: despawn_unit_snapshots,
	spawn_unit_snapshots: spawn_unit_snapshots,
	spawn_unit_snapshots_as: spawn_unit_snapshots_as,
	rehome_surviving_units: rehome_surviving_units,
	restore_rehomed_units: restore_rehomed_units,
};
