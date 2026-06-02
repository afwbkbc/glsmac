return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to add base pop';
		}
	},

	apply: (e) => {

		const base = e.data.base;

		// reset nutrients
		const old_nutrients = base.get('accumulated_nutrients');
		e.game.get('f_base_reset_nutrients')(e.game, base);

		// spawn population
		const pop = base.create_pop({
			type: e.data.type,
			worked_tile: e.data.worked_tile,
		});

		let worked_tile = #undefined;
		if (#is_defined(e.data.worked_tile)) {
			worked_tile = e.data.worked_tile;
			e.game.get('f_base_pop_work_tile')(base, pop, worked_tile);
		}

		return {
			base: base,
			pop: pop,
			old_nutrients: old_nutrients,
			worked_tile: worked_tile,
		};
	},

	rollback: (e) => {
		if (#is_defined(e.applied.worked_tile)) {
			e.game.get('f_base_pop_unwork', e.applied.base, e.applied.pop);
		}
		e.data.base.destroy_pop(e.applied.pop);
		if (#is_defined(e.applied.worked_tile)) {
			e.data.base.base.remove_worked_tile(e.applied.worked_tile);
		}
		e.data.base.set('accumulated_nutrients', e.applied.old_nutrients);
	},

};
