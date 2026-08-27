return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to process base growth';
		}
		if (#typeof(e.data.psych) != 'Int' || e.data.psych < 0) {
			return 'Base psych must be a non-negative whole number';
		}
	},

	apply: (e) => {
		e.game.get('f_base_process_growth')(e.game, e.data.base, e.data.psych);
		if (e.game.is_master()) {
			e.game.event('refresh_base_psych', {
				base: e.data.base,
			});
		}
	},

	rollback: (e) => {
		// Base growth events are host-authored and never applied speculatively.
	},

};
