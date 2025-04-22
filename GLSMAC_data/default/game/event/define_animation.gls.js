return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to define animations';
		}
	},

	apply: (e) => {
		e.game.am.define(e.data.id, e.data.data);
		return {
			id: e.data.id,
		};
	},

	rollback: (e) => {
		e.game.am.undefine(e.applied.id);
	},

};
