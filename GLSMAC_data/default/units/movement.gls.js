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

const result = {

	configure: (game) => {

		game.um.on('unit_move_validate', (e) => {
			if (e.unit.is_immovable) {
				return 'Unit is immovable';
			}
			if (e.unit.movement == 0.0) {
				return 'Unit is out of moves';
			}
			if (e.src_tile == e.dst_tile) {
				return 'Destination tile is same as source tile';
			}
			if (!e.src_tile.is_adjactent_to(e.dst_tile)) {
				return 'Destination tile is not adjactent to source tile';
			}
			if (e.unit.is_land && e.dst_tile.is_water) {
				return 'Land units can\'t move to water tile';
			}
			if (e.unit.is_water && e.dst_tile.is_land) {
				return 'Water units can\'t move to land tile';
			}

			let any_foreign_units_in_tile = false;
			for (unit of e.dst_tile.get_units()) {
				if (unit.get_owner() != e.unit.get_owner()) {
					any_foreign_units_in_tile = true;
					break;
				}
			}
			if (any_foreign_units_in_tile) {
				return 'Destination tile contains foreign units (combat not implemented yet)';
			}
			// TODO: ZOC

		});

		game.um.on('unit_move_resolve', (e) => {
			if (e.unit.movement == 0.0) {
				// no moves left
				return false;
			}
			let movement_cost = get_movement_cost(e.unit, e.src_tile, e.dst_tile);
			return {
				is_movement_successful:
					(e.unit.movement >= movement_cost) // unit has enough moves
					||
					(game.random.get_float(0.0, movement_cost) < e.unit.movement) // unit doesn't have enough moves but was lucky
			};
		});

		game.um.on('unit_move_apply', (e) => {
			let src_tile = e.unit.get_tile();

			game.tm.lock_tiles([src_tile, e.dst_tile], (unlock) => {

				let movement_cost = get_movement_cost(e.unit, src_tile, e.dst_tile) + get_movement_aftercost(e.unit, e.unit.get_tile(), e.dst_tile);

				let next = () => {
					// reduce remaining movement points (even if failed)
					if (e.unit.movement >= movement_cost) {
						e.unit.movement = e.unit.movement - movement_cost;
					} else {
						e.unit.movement = 0.0;
					}
					e.unit.moved_this_turn = true;

					unlock();
				};

				if (e.resolutions.is_movement_successful) {
					e.unit.move_to_tile(e.dst_tile, next);
				} else {
					next();
				}

			});

		});

	},

};

return result;
