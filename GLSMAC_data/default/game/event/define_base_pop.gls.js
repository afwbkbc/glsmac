return {

	validate: (e) => {
		/* hmm if (e.game.is_started()) {
			return 'Game has already started';
		}*/
		if (e.caller != 0) {
			return 'Only master is allowed to define base pop';
		}
	},

	apply: (e) => {
		e.game.bm.define_pop(e.data.id, e.data.def);
	},

	rollback: (e) => {
		e.game.bm.undefine_pop(e.data.id);
	},

};
