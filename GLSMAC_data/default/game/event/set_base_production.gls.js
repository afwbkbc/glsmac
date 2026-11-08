return {

	validate: (e) => {
		if (e.caller != e.data.base.get_owner().id) {
			return 'Only base owner can control base production';
		}
	},

	apply: (e) => {
		const old_production_id = e.data.base.get_production().id;
		e.data.base.set_production(e.data.id);
		return {
			old_id: old_production_id,
		};
	},

	rollback: (e) => {
		e.data.base.set_production(e.applied.old_id);
	},

};
