return {

	validate: (e) => {
		if (e.caller != e.data.base.get_owner().id) {
			return 'Only base owner can control base pops';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		const work_pop = e.data.tile.get('working_pop');
		if (!#is_defined(work_pop)) {
			return 'Tile is not being worked';
		}
		if (!e.data.base.is_tile_worked(e.data.tile)) {
			return 'Tile is not worked by this base';
		}
		if (work_pop.get_base() != e.data.base) {
			return 'Tile population does not belong to this base';
		}
	},

	apply: (e) => {
		const pop = e.data.tile.get('working_pop');
		let pop_type_snapshots = [];
		for (base_pop of e.data.base.get_pops()) {
			pop_type_snapshots :+{
				pop: base_pop,
				type: base_pop.get_type(),
			};
		}
		e.game.get('f_base_pop_unwork_tile')(e.data.base, pop, 'DOCTOR');
		const psych = e.game.get('f_economy_get_base_psych')(e.game, e.data.base);
		e.game.get('f_base_process_psych')(e.game, e.data.base, psych);
		return {
			pop: pop,
			pop_type_snapshots: pop_type_snapshots,
		};
	},

	rollback: (e) => {
		e.game.get('f_base_pop_work_tile')(e.data.base, e.applied.pop, e.data.tile);
		for (snapshot of e.applied.pop_type_snapshots) {
			snapshot.pop.set_type(snapshot.type);
		}
	},

};
