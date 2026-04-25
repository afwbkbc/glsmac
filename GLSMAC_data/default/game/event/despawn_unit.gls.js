return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to despawn units';
		}
	},

	apply: (e) => {
		e.game.um.despawn_unit(e.data.unit);
		return {
			unit: e.data.unit,
		};
	},

	rollback: (e) => {
		const u = e.applied.unit;
		// TODO: make e.game.um.spawn_unit(u); possible
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
