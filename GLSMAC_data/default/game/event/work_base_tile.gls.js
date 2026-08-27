return {

	validate: (e) => {
		if (e.caller != e.data.base.get_owner().id) {
			return 'Only base owner can control base pops';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (e.data.pop.get_base() != e.data.base) {
			return 'Population does not belong to this base';
		}
		let is_workable = false;
		for (tile of e.data.base.get_workable_tiles()) {
			if (tile == e.data.tile) {
				is_workable = true;
				break;
			}
		}
		if (!is_workable) {
			return 'Tile is outside this base\'s workable radius';
		}
		if (e.data.tile.get_base() != null) {
			return 'Base centers cannot be worked';
		}
		if (e.data.tile.has('working_pop')) {
			return 'Tile is already being worked';
		}
	},

	apply: (e) => {
		const old_tile = e.data.pop.get('worked_tile');
		let pop_type_snapshots = [];
		for (pop of e.data.base.get_pops()) {
			pop_type_snapshots :+{
				pop: pop,
				type: pop.get_type(),
			};
		}
		e.game.get('f_base_pop_work_tile')(e.data.base, e.data.pop, e.data.tile);
		const psych = e.game.get('f_economy_get_base_psych')(e.game, e.data.base);
		e.game.get('f_base_process_psych')(e.game, e.data.base, psych);
		return {
			old_tile: old_tile,
			pop_type_snapshots: pop_type_snapshots,
		};
	},

	rollback: (e) => {
		e.game.get('f_base_pop_unwork_tile')(
			e.data.base,
			e.data.pop
		);
		if (#is_defined(e.applied.old_tile)) {
			e.game.get('f_base_pop_work_tile')(
				e.data.base,
				e.data.pop,
				e.applied.old_tile
			);
		}
		for (snapshot of e.applied.pop_type_snapshots) {
			snapshot.pop.set_type(snapshot.type);
		}
	},

};
