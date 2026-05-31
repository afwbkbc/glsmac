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
		e.game.rm.define_no_resource({
			render: e.data.render,
		});
		return {};
	},

	rollback: (e) => {
		e.game.rm.undefine_no_resource();
	},

};
