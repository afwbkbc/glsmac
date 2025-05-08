return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to add base pop';
		}
	},

	apply: (e) => {
		return {
			pop_id: e.data.base.create_pop({
				type: e.data.type,
			}),
		};
	},

	rollback: (e) => {
		e.data.base.remove_pop(e.applied.pop_id);
	},

};
