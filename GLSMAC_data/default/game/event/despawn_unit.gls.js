return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to despawn units';
		}
		if (!e.game.um.has_unit(e.data.unit_id)) {
			return 'Unit id ' + #to_string(e.data.unit_id) + ' not found';
		}

	},

	apply: (e) => {
		const unit = e.game.um.get_unit(e.data.unit_id);
		e.game.um.despawn_unit(e.data.unit_id);
		return {
			unit: unit,
		};
	},

	rollback: (e) => {
		const u = e.applied.unit;
		e.game.um.spawn_unit({
			id: u.id,
			def: u.def,
			owner: e.game.get_player(u.owner),
			tile: e.game.tm.get_tile(u.tile.x, u.tile.y),
			morale: u.morale,
			health: u.health,
		});
	},

};
