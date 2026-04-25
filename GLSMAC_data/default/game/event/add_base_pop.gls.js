return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to add base pop';
		}
	},

	apply: (e) => {

		const base = e.data.base;

		// reset nutrients
		e.game.get('f_base_reset_nutrients')(base);

		// spawn population
		const pop_id = base.create_pop({
			type: e.data.type,
		});

		return {
			pop_id: pop_id,
		};
	},

	rollback: (e) => {
		e.data.base.remove_pop(e.applied.pop_id);
	},

};
