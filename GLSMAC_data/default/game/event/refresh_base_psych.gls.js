return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to refresh base psych';
		}
	},

	apply: (e) => {
		const base = e.data.base;
		const psych = e.game.get('f_economy_get_base_psych')(e.game, base);
		if (base.get_owner().type == 'ai') {
			const stable_workers = e.game.get('f_base_get_stable_worker_count')(base, psych);
			e.game.get('f_base_rebalance_workers')(base, stable_workers);
		}
		e.game.get('f_base_process_psych')(e.game, base, psych);
	},

	rollback: (e) => {
		// Host-authored base turn events are never applied speculatively.
	},

};
