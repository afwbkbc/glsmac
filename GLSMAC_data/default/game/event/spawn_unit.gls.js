return {

	validate: (e) => {
		#print('SPAWN UNIT validate', e);
		if (e.caller != 0) {
			return 'Only master is allowed to spawn units';
		}
	},

	resolve: (e) => {
		#print('SPAWN UNIT resolve', e);
		return {
			owner: e.game.get_player_by_id(e.data.owner),
			tile: e.game.tm.get_tile(e.data.tile.x, e.data.tile.y),
		};
	},

	apply: (e) => {
		#print('SPAWN UNIT apply', e);
		const unit = e.game.um.spawn_unit(
			e.data.type,
			e.resolved.owner,
			e.resolved.tile,
			e.data.morale,
			e.data.health
		);
		return {
			unit: unit,
		};
	},

	rollback: (e) => {
		#print('SPAWN UNIT rollback', e);
		e.game.um.despawn_unit(e.applied.unit);
	},

};
