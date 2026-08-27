return {
	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master can liquidate a facility for insolvency';
		}
		if (!e.data.base.has_facility(e.data.facility_id)) {
			return 'Base does not have this facility';
		}
		const facility = e.game.get_bm().get_facility_def(e.data.facility_id);
		if (facility.energy_maintenance <= 0) {
			return 'Facility has no maintenance cost';
		}
	},

	apply: (e) => {
		const base = e.data.base;
		let pop_type_snapshots = [];
		for (pop of base.get_pops()) {
			pop_type_snapshots :+{
				pop: pop,
				type: pop.get_type(),
			};
		}
		base.remove_facility(e.data.facility_id);
		const psych = e.game.get('f_economy_get_base_psych')(e.game, base);
		e.game.get('f_base_process_psych')(e.game, base, psych);
		return {
			facility_id: e.data.facility_id,
			pop_type_snapshots: pop_type_snapshots,
		};
	},

	rollback: (e) => {
		e.data.base.add_facility(e.applied.facility_id);
		for (snapshot of e.applied.pop_type_snapshots) {
			snapshot.pop.set_type(snapshot.type);
		}
	},
};
