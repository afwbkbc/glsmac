const movement_rules = #include('../movement_rules');
const base_capture = #include('../base_capture');
const unity_pods = #include('../unity_pods');
const unit_abilities = #include('../unit_abilities');

const get_transport_id = (unit) => {
	return #is_defined(unit.transport_id) ? unit.transport_id : 0;
};

const get_boarding_transport = (unit, tile) => {
	for (candidate of tile.get_units()) {
		const def = candidate.get_def();
		if (
			candidate.owner == unit.owner && get_transport_id(candidate) == 0 &&
			def.cargo_capacity > 0 &&
			#sizeof(candidate.get_cargo()) < def.cargo_capacity
		) {
			return candidate;
		}
	}
	return null;
};

const is_amphibious_base_crossing = (unit, src_tile, dst_tile) => {
	if (!unit_abilities.has(unit, 'AmphibiousPods')) {
		return false;
	}
	return (
		(src_tile.is_water && src_tile.get_base() != null && dst_tile.is_land) ||
		(src_tile.is_land && dst_tile.is_water && dst_tile.get_base() != null)
	);
};

const has_fungus_road = (unit, game) => {
	if (unit.get_def().is_native) {
		return true;
	}
	if (!#is_defined(game) || !#is_defined(game.get)) {
		return false;
	}
	const get_effects = game.get('f_project_get_player_effects');
	if (!#is_defined(get_effects)) {
		return false;
	}
	const effects = get_effects(unit.get_owner());
	return #is_defined(effects.fungus_movement_as_road) &&
		effects.fungus_movement_as_road;
};

const get_movement_cost = (unit, src_tile, dst_tile, fungus_road) => {
	const is_native = unit.get_def().is_native;

	if (
		dst_tile.is_land &&
		(
			(src_tile.features.river && dst_tile.features.river) ||
			(src_tile.terraforming.road && dst_tile.terraforming.road)
		)
	) {
		return 1.0 / 3.0;
	}

	if (dst_tile.features.xenofungus) {
		if (is_native || fungus_road) {
			if (dst_tile.is_water) {
				return 1.0;
			} else {
				return 1.0 / 3.0;
			}
		}
		return 3.0;
	}
	return 1.0;
};

const get_movement_aftercost = (unit, src_tile, dst_tile, fungus_road) => {
	const is_native = unit.get_def().is_native;
	if (
		dst_tile.is_land &&
		(
			(src_tile.features.river && dst_tile.features.river) ||
			(src_tile.terraforming.road && dst_tile.terraforming.road)
		)
	) {
		return 0.0;
	}
	if ((is_native || fungus_road) && dst_tile.features.xenofungus) {
		return 0.0;
	}
	const has_forest = #is_defined(dst_tile.terraforming.forest) && dst_tile.terraforming.forest;
	if (dst_tile.is_land && (dst_tile.rockiness >= 3 || has_forest)) {
		return 1.0;
	}
	return 0.0;
};

return {

	validate: (e) => {

		if (e.data.unit.owner != e.caller) {
			return 'Unit can only be moved by it\'s owner';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (e.data.unit.health <= 0.0) {
			return 'Dead unit cannot move';
		}
		if (
			get_transport_id(e.data.unit) > 0 &&
			e.data.unit.get_transport() == null
		) {
			return 'Embarked unit has no transport';
		}

		const src_tile = e.data.unit.get_tile();
		const dst_tile = e.data.tile;
		const dst_base = dst_tile.get_base();
		if (
			#typeof(e.data.unit.get_owner) == 'Callable' &&
			e.data.unit.get_owner().type == 'native' &&
			dst_base != null && dst_base.get_owner().id != e.data.unit.owner
		) {
			return 'Wild native life cannot capture bases';
		}

		if (src_tile == dst_tile) {
			return 'Source tile is same as destination tile';
		}

		if (src_tile.is_locked()) {
			return 'Source tile is locked';
		}
		if (dst_tile.is_locked()) {
			return 'Destination tile is locked';
		}

		if (e.data.unit.is_immovable) {
			return 'Unit is immovable';
		}
		if (e.data.unit.terraforming != 'none') {
			return 'Cancel the unit\'s terraforming order before moving';
		}
		if (e.data.unit.movement <= 0.0) {
			return 'Unit is out of moves';
		}
		if (src_tile == dst_tile) {
			return 'Destination tile is same as source tile';
		}
		if (!src_tile.is_adjactent_to(dst_tile)) {
			return 'Destination tile is not adjactent to source tile';
		}
		const amphibious_base_crossing = is_amphibious_base_crossing(
			e.data.unit,
			src_tile,
			dst_tile
		);
		if (get_transport_id(e.data.unit) > 0) {
			if (!e.data.unit.is_land || !dst_tile.is_land) {
				return 'Embarked land units can only disembark onto land';
			}
		} else if (
			e.data.unit.is_land && src_tile.is_water && dst_tile.is_land &&
			!amphibious_base_crossing && get_boarding_transport(e.data.unit, src_tile) == null
		) {
			return 'Land units need Amphibious Pods or a friendly transport to leave a sea base';
		} else if (
			e.data.unit.is_land && dst_tile.is_water &&
			!amphibious_base_crossing && get_boarding_transport(e.data.unit, dst_tile) == null
		) {
			return 'Land units need a friendly transport with free capacity to enter water';
		}
		if (e.data.unit.is_water && dst_tile.is_land && dst_tile.get_base() == null) {
			return 'Water units can only enter land tiles containing a base';
		}

		let any_foreign_units_in_tile = false;
		for (unit of dst_tile.get_units()) {
			if (unit.owner != e.data.unit.owner) {
				any_foreign_units_in_tile = true;
				break;
			}
		}
		if (any_foreign_units_in_tile) {
			return 'Destination tile contains foreign units (combat not implemented yet)';
		}
		if (movement_rules.is_zoc_move_blocked(e.data.unit, src_tile, dst_tile)) {
			return 'Unit cannot move directly between enemy zones of control';
		}
	},

	resolve: (e) => {

		const unit = e.data.unit;
		const movement = e.data.unit.movement + 0.0;

		const src_tile = e.data.unit.get_tile();
		const dst_tile = e.data.tile;

		const fungus_road = has_fungus_road(e.data.unit, e.game);
		let movement_cost = get_movement_cost(
			e.data.unit,
			src_tile,
			dst_tile,
			fungus_road
		);

		const amphibious_base_crossing = is_amphibious_base_crossing(
			unit,
			src_tile,
			dst_tile
		);
		const transport =
			get_transport_id(e.data.unit) == 0 &&
			#is_defined(e.data.unit.is_land) && e.data.unit.is_land &&
			#is_defined(dst_tile.is_water) && dst_tile.is_water &&
			!amphibious_base_crossing
				? get_boarding_transport(e.data.unit, dst_tile)
				: null;

		const is_movement_successful =
				(movement >= movement_cost) // unit has enough moves
				||
				(e.game.random.get_float(0.0, movement_cost) < movement); // unit doesn't have enough moves but was lucky
		return {
			is_movement_successful: is_movement_successful,
			transport_id: transport == null ? 0 : transport.id,
			unity_pod:
				is_movement_successful &&
				#is_defined(dst_tile.features.unity_pod) &&
				dst_tile.features.unity_pod
					? unity_pods.resolve(e.game, e.data.unit, dst_tile)
					: null,
		};
	},

	apply: (e) => {

		const unit = e.data.unit;
		const src_tile = unit.get_tile();
		const dst_tile = e.data.tile;
		const dst_base = #is_defined(dst_tile.get_base) ? dst_tile.get_base() : null;

		const movement = unit.movement;

		const result = {
			orig: {
				tile: src_tile,
				movement: movement,
				moved_this_turn: unit.moved_this_turn == true,
				transport_id: get_transport_id(unit) + 0,
				base_owner: dst_base == null ? null : dst_base.get_owner(),
			},
			movement_started: e.resolved.is_movement_successful,
			base_capture: null,
			rehomed_units: [],
			unity_pod: null,
		};

		const fungus_road = has_fungus_road(unit, e.game);
		let movement_cost = get_movement_cost(unit, src_tile, dst_tile, fungus_road) +
			get_movement_aftercost(unit, src_tile, dst_tile, fungus_road);

		const finish_movement = () => {
			// reduce remaining movement points (even if failed)
			if (movement >= movement_cost) {
				unit.movement = movement - movement_cost;
			} else {
				unit.movement = 0.0;
			}
			unit.moved_this_turn = true;
		};

		if (e.resolved.is_movement_successful) {
			unit.move_to_tile(dst_tile, () => {});
			if (#is_defined(e.resolved.transport_id) && e.resolved.transport_id > 0) {
				unit.embark(e.game.um.get_unit(e.resolved.transport_id));
			} else if (result.orig.transport_id > 0) {
				unit.disembark();
			}
			if (
				get_transport_id(unit) == 0 && dst_base != null &&
				dst_base.get_owner().id != unit.owner
			) {
				result.base_capture = base_capture.capture_base(e.game, dst_base, unit.get_owner());
				result.rehomed_units = result.base_capture.rehomed_units;
			}
			finish_movement();
			if (#is_defined(e.resolved.unity_pod) && e.resolved.unity_pod != null) {
				result.unity_pod = unity_pods.apply(
					e.game,
					unit,
					dst_tile,
					e.resolved.unity_pod
				);
			}
		} else {
			// No native move is started on a failed roll, so update state synchronously.
			if (movement >= movement_cost) {
				unit.movement = movement - movement_cost;
			} else {
				unit.movement = 0.0;
			}
			unit.moved_this_turn = true;
		}

		return result;
	},

	rollback: (e) => {

		const unit = e.data.unit;
		const orig = e.applied.orig;
		if (e.applied.unity_pod != null) {
			unity_pods.rollback(e.game, e.applied.unity_pod);
		}
		if (e.applied.movement_started) {
			if (get_transport_id(unit) > 0) {
				unit.disembark();
			}
			unit.move_to_tile(orig.tile, () => {});
			if (orig.transport_id > 0) {
				unit.embark(e.game.um.get_unit(orig.transport_id));
			}
		}
		if (e.applied.base_capture != null) {
			base_capture.restore_base(e.data.tile.get_base(), e.applied.base_capture);
		}
		unit.movement = orig.movement;
		unit.moved_this_turn = orig.moved_this_turn;
	},

};
