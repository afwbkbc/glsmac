return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to remove base pop';
		}
	},

	apply: (e) => {

		const base = e.data.base;
		const pop = e.data.pop;

		const old_pop_type = pop.get_type();
		const old_worked_tile = pop.get('worked_tile');

		// reset nutrients
		const old_nutrients = base.get('accumulated_nutrients');
		e.game.get('f_base_reset_nutrients')(e.game, base);

		if (#is_defined(old_worked_tile)) {
			e.game.get('f_base_pop_unwork', base, pop);
		}

		// remove population
		base.destroy_pop(pop);

		return {
			base: base,
			old_pop_type: old_pop_type,
			old_nutrients: old_nutrients,
			old_worked_tile: old_worked_tile,
		};
	},

	rollback: (e) => {
		const pop = base.create_pop({
			type: e.applied.old_pop_type,
			worked_tile: e.applied.old_worked_tile,
		});
		if (#is_defined(e.applied.old_worked_tile)) {
			e.game.get('f_base_pop_work_tile')(e.data.base, pop, e.applied.old_worked_tile);
		}
		e.data.base.set('accumulated_nutrients', e.applied.old_nutrients);
	},

};
