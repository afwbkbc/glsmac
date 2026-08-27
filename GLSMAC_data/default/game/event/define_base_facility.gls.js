return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to define base facilities';
		}
	},

	apply: (e) => {
		e.game.bm.define_facility(e.data.id, e.data.data);
	},

	rollback: (e) => {
		e.game.bm.undefine_facility(e.data.id);
	},

};
