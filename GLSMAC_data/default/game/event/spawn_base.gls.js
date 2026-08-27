return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to spawn bases';
		}
		if (
			#is_defined(e.data.headquarters) &&
			#typeof(e.data.headquarters) != 'Bool'
		) {
			return 'Headquarters flag must be a boolean';
		}
	},

	apply: (e) => {
		const owner = e.data.owner;
		let info = {
			production: #is_defined(e.data.production)
				? e.data.production
				: (e.data.tile.is_water ? 'SeaLurk' : 'ScoutPatrol'),
		};
		if (#is_defined(e.data.name)) {
			info.name = e.data.name;
		}
		const base = e.game.bm.spawn_base(owner, e.data.tile, info);
		const get_new_base_minerals = #is_defined(e.game.get)
			? e.game.get('f_social_get_new_base_minerals')
			: #undefined;
		base.set_accumulated_minerals(
			#is_defined(get_new_base_minerals) ? get_new_base_minerals(owner) : 10
		);
		if (#is_defined(e.data.headquarters) && e.data.headquarters) {
			base.add_facility('Headquarters');
		}

		return {
			base: base,
		};
	},

	rollback: (e) => {
		e.game.bm.despawn_base(e.applied.base);
	},

};
