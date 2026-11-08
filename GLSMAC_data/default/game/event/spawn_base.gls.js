return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to spawn bases';
		}
	},

	apply: (e) => {
		const base = e.game.bm.spawn_base(e.data.owner, e.data.tile, {
			// name: e.data.name,
			facilities: e.data.facilities,
			production: 'STOCKPILE_ENERGY', // TODO: some logic to pick something important
		});

		return {
			base: base,
		};
	},

	rollback: (e) => {
		e.game.bm.despawn_base(e.applied.base);
	},

};
