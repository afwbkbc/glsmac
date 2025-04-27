const get_movement_cost = (unit, src_tile, dst_tile) => {
	const is_native = true; // TODO: non-native units

	if (
		dst_tile.is_land &&
		src_tile.has_river &&
		dst_tile.has_river
		// TODO: roads
	) {
		return 1.0 / 3.0;
	}

	if (dst_tile.has_fungus) {
		if (is_native) {
			if (dst_tile.is_water) {
				return 1.0;
			}
			return 1.0 / 3.0;
		}
		return 3.0;
	}
	return 1.0;
};

const get_movement_aftercost = (unit, src_tile, dst_tile) => {
	const is_native = true; // TODO: non-native units
	if (is_native && dst_tile.has_fungus) {
		return 0.0;
	}
	if (dst_tile.is_land && dst_tile.is_rocky) {
		return 1.0;
	}
	return 0.0;
};

return {

	validate: (e) => {
		if (!e.game.um.has_unit(e.data.unit.id)) {
			return 'Unit id ' + #to_string(e.data.unit_id) + ' not found';
		}
		if (e.data.unit.owner != e.caller) {
			return 'Unit can only be moved by it\'s owner';
		}

		const src_tile = e.data.unit.get_tile();
		const dst_tile = e.data.tile;

		if (src_tile == dst_tile) {
			return 'Source tile is same as destination tile';
		}

		if (src_tile.is_locked()) {
			return 'Source tile is locked';
		}
		if (dst_tile.is_locked()) {
			return 'Source tile is locked';
		}

		if (e.data.unit.is_immovable) {
			return 'Unit is immovable';
		}
		if (e.data.unit.movement == 0.0) {
			return 'Unit is out of moves';
		}
		if (src_tile == dst_tile) {
			return 'Destination tile is same as source tile';
		}
		if (!src_tile.is_adjactent_to(dst_tile)) {
			return 'Destination tile is not adjactent to source tile';
		}
		if (e.data.unit.is_land && dst_tile.is_water) {
			return 'Land units can\'t move to water tile';
		}
		if (e.data.unit.is_water && dst_tile.is_land) {
			return 'Water units can\'t move to land tile';
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
		// TODO: ZOC
	},

	resolve: (e) => {
		if (e.data.unit.movement == 0.0) {
			// no moves left
			return false;
		}

		const src_tile = e.data.unit.get_tile();
		const dst_tile = e.data.tile;

		let movement_cost = get_movement_cost(e.data.unit, src_tile, dst_tile);

		return {
			is_movement_successful:
				(e.data.unit.movement >= movement_cost) // unit has enough moves
				||
				(e.game.random.get_float(0.0, movement_cost) < e.data.unit.movement) // unit doesn't have enough moves but was lucky
		};
	},

	apply: (e) => {
		const unit = e.data.unit;
		const src_tile = unit.get_tile();
		const dst_tile = e.data.tile;

		const result = {
			orig: {
				tile: src_tile,
				movement: unit.movement,
				moved_this_turn: unit.moved_this_turn,
			}
		};

		let movement_cost = get_movement_cost(unit, src_tile, dst_tile) + get_movement_aftercost(unit, src_tile, dst_tile);

		let next = () => {
			// reduce remaining movement points (even if failed)
			if (unit.movement >= movement_cost) {
				unit.movement = unit.movement - movement_cost;
			} else {
				unit.movement = 0.0;
			}
			unit.moved_this_turn = true;
		};

		if (e.resolved.is_movement_successful) {
			unit.move_to_tile(dst_tile, next);
		} else {
			next();
		}

		return result;
	},

	rollback: (e) => {
		const unit = e.data.unit;
		const orig = e.applied.orig;
		unit.move_to_tile(orig.tile, () => {
			unit.movement = orig.movement;
			unit.moved_this_turn = orig.moved_this_turn;
		});
	},

};
