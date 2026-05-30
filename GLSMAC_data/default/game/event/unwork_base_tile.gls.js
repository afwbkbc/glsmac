return {

	validate: (e) => {
		if (e.caller != e.data.base.get_owner().id) {
			return 'Only base owner can control base pops';
		}
		const work_pop = e.data.tile.get('working_pop');
		if (!#is_defined(work_pop)) {
			return 'Tile is not being worked';
		}
	},

	apply: (e) => {
		const pop = e.data.tile.get('working_pop');
		e.game.get('f_base_pop_unwork_tile')(e.data.base, pop, 'DOCTOR');
		return {
			pop: pop,
		};
	},

	rollback: (e) => {
		e.game.get('f_base_pop_work_tile')(e.data.base, e.applied.pop, e.data.tile);
	},

};
