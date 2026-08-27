const terraforming = #include('../../units/terraforming');
const native_life = #include('../native_life');

return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to trigger a fungal bloom';
		}
		if (
			#typeof(e.data.base) != 'Object' ||
			#typeof(e.data.base.get_workable_tiles) != 'Callable' ||
			#typeof(e.data.base.get_owner) != 'Callable'
		) {
			return 'Fungal bloom base must be a base object';
		}
		if (
			#typeof(e.data.tile) != 'Object' ||
			#typeof(e.data.tile.get_base) != 'Callable' ||
			#typeof(e.data.tile.update_features) != 'Callable' ||
			#typeof(e.data.tile.update_terraforming) != 'Callable' ||
			#typeof(e.data.tile.features) != 'Object' ||
			#typeof(e.data.tile.terraforming) != 'Object'
		) {
			return 'Fungal bloom tile must be a tile object';
		}
		if (
			#typeof(e.data.damage) != 'Int' ||
			e.data.damage < 0 ||
			e.data.damage > 1000000000
		) {
			return 'Ecological damage must be a non-negative whole number';
		}
		const owner = e.data.base.get_owner();
		if (
			#typeof(owner) != 'Object' ||
			#typeof(owner.get_ecological_damage_events) != 'Callable' ||
			#typeof(owner.set_ecological_damage_events) != 'Callable'
		) {
			return 'Fungal bloom base owner must be a player object';
		}
		let is_workable = false;
		for (tile of e.data.base.get_workable_tiles()) {
			if (tile == e.data.tile) {
				is_workable = true;
				break;
			}
		}
		if (!is_workable) {
			return 'Fungal bloom tile is outside the base workable radius';
		}
		if (e.data.tile.get_base() != null) {
			return 'Fungal blooms cannot replace a base';
		}
		if (e.data.tile.features.monolith) {
			return 'Fungal blooms cannot replace a monolith';
		}
		if (e.data.tile.features.xenofungus) {
			return 'Fungal bloom tile already contains xenofungus';
		}
		if (owner.get_ecological_damage_events() >= 1000000) {
			return 'Ecological damage event limit has been reached';
		}
	},

	resolve: (e) => {
		const life_level = native_life.get_life_level(e.game);
		if (life_level <= 0) {
			return {outbreak: {spawns: []}};
		}
		const severity = 1 + #min(
			2,
			#floor(#to_float(e.data.damage) / 40.0)
		);
		return {
			outbreak: native_life.resolve_outbreak(
				e.game,
				e.data.tile,
				#min(life_level, severity),
				true
			),
		};
	},

	apply: (e) => {
		const owner = e.data.base.get_owner();
		let old_terraforming = {};
		const order = terraforming.get_order('plant_fungus');
		const changes = order.changes;
		for (id in changes) {
			old_terraforming[id] = e.data.tile.terraforming[id];
		}
		const previous_events = owner.get_ecological_damage_events();
		e.data.tile.update_terraforming(changes);
		const old_xenofungus = e.data.tile.features.xenofungus;
		e.data.tile.update_features(order.feature_changes);
		owner.set_ecological_damage_events(previous_events + 1);
		let climate = null;
		if (#is_defined(e.game.get)) {
			const advance_climate = e.game.get('f_ecology_advance_climate_damage');
			if (#is_defined(advance_climate)) {
				climate = advance_climate(owner);
			}
		}
		e.game.trigger('ecological_damage', {
			base: e.data.base,
			tile: e.data.tile,
			damage: e.data.damage,
			warming_triggered: climate != null && climate.warming_triggered,
		});
		e.game.trigger('update_base', {base: e.data.base});
		e.game.message(
			'Uncontrolled xenofungus has erupted near ' + e.data.base.name + '.'
		);
		if (climate != null && climate.warming_triggered) {
			e.game.message(
				'Planetary warming has destabilized the polar ice caps.'
			);
		}
		const outbreak = #is_defined(e.resolved) &&
			#is_defined(e.resolved.outbreak) &&
			#sizeof(e.resolved.outbreak.spawns) > 0
			? native_life.apply_outbreak(e.game, e.resolved.outbreak)
			: null;
		return {
			xenofungus: old_xenofungus,
			terraforming: old_terraforming,
			ecological_damage_events: previous_events,
			climate: climate,
			outbreak: outbreak,
		};
	},

	rollback: (e) => {
		if (#is_defined(e.applied.outbreak) && e.applied.outbreak != null) {
			native_life.rollback_outbreak(e.game, e.applied.outbreak);
		}
		e.data.tile.update_features({xenofungus: e.applied.xenofungus});
		e.data.tile.update_terraforming(e.applied.terraforming);
		e.data.base.get_owner().set_ecological_damage_events(
			e.applied.ecological_damage_events
		);
		if (e.applied.climate != null) {
			const previous = e.applied.climate.previous;
			e.game.get_tm().set_climate_state(
				previous.level,
				previous.future_change,
				previous.progress
			);
		}
		e.game.trigger('update_base', {base: e.data.base});
	},

};
