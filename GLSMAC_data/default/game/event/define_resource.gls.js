return {

	validate: (e) => {
		/* hmm if (e.game.is_started()) {
			return 'Game has already started';
		}*/
		if (e.caller != 0) {
			return 'Only master is allowed to define resources';
		}
	},

	apply: (e) => {
		e.game.rm.define(e.data.name, {
			name: e.data.name,
			render: e.data.render,
		});
		return {
			id: e.data.name,
		};
	},

	rollback: (e) => {
		e.game.rm.undefine(e.applied.id);
	},

};
