return {

	validate: (e) => {
		#print('SPAWN UNIT validate', e);
		if (e.caller != 0) {
			return 'Only master is allowed to spawn units';
		}
	},

	apply: (e) => {
		#print('SPAWN UNIT apply', e);
		const unit = e.game.um.spawn_unit({
			def: e.data.type,
			owner: e.game.get_player(e.data.owner),
			tile: e.game.tm.get_tile(e.data.tile.x, e.data.tile.y),
			morale: e.data.morale,
			health: e.data.health,
		});

		return {
			unit: unit,
		};
	},

	rollback: (e) => {
		#print('SPAWN UNIT rollback', e);
		e.game.um.despawn_unit(e.applied.unit);
	},

};
