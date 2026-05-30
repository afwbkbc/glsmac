return {

	validate: (e) => {
		if (e.caller != e.data.base.get_owner().id) {
			return 'Only base owner can control base pops';
		}
		if (e.data.tile.has('working_pop')) {
			return 'Tile is already being worked';
		}
	},

	apply: (e) => {
		e.game.get('f_base_pop_work_tile')(e.data.base, e.data.pop, e.data.tile);
		return {};
	},

	rollback: (e) => {
		e.game.get('f_base_pop_unwork_tile')(e.data.base, e.data.pop);
	},

};
