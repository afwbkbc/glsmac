const snapshot_unit = #include('../entity_snapshots').snapshot_unit;

const snapshot_transport_tree = (unit, result, refs) => {
	result :+snapshot_unit(unit);
	refs :+unit;
	if (#is_defined(unit.get_cargo)) {
		for (cargo of unit.get_cargo()) {
			snapshot_transport_tree(cargo, result, refs);
		}
	}
};

return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to despawn units';
		}
	},

	apply: (e) => {
		const unit = e.data.unit;
		let backups = [];
		let refs = [];
		snapshot_transport_tree(unit, backups, refs);
		for (let i = #sizeof(backups) - 1; i >= 0; i--) {
			e.game.um.despawn_unit(refs[i]);
		}
		return {
			units: backups,
		};
	},

	rollback: (e) => {
		for (u of e.applied.units) {
			const unit = e.game.um.spawn_unit({
				id: u.id,
				def: u.def,
				owner: e.game.get_player(u.owner),
				tile: e.game.tm.get_tile(u.tile_x, u.tile_y),
				morale: u.morale,
				health: u.health,
				terraforming: u.terraforming,
				terraforming_turns_remaining: u.terraforming_turns_remaining,
				home_base_id: u.home_base_id,
				fuel: u.fuel,
				transport_id: u.transport_id,
			});
			unit.movement = u.movement;
			unit.moved_this_turn = u.moved_this_turn;
			unit.native_capture_attempted = u.native_capture_attempted;
		}
	},

};
