const f_get_movement_cost = (unit, src_tile, dst_tile) => {
    const is_native = true; // TODO: non-native units

    if (src_tile.has_river && dst_tile.has_river) { // TODO: roads
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

const f_get_movement_aftercost = (unit, src_tile, dst_tile) => {
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

    init: () => {

        #game.on.unit_move_resolve((e) => {
            if (e.unit.movement == 0.0) {
                // no moves left
                return false;
            }
            let movement_cost = f_get_movement_cost(e.unit, e.src_tile, e.dst_tile);
            return {
                is_movement_successful:
                    (e.unit.movement >= movement_cost) // unit has enough moves
                    ||
                    (#game.random.get_float(0.0, movement_cost) < e.unit.movement) // unit doesn't have enough moves but was lucky
            };
        });

        #game.on.unit_move((e) => {
            let movement_cost = f_get_movement_cost(e.unit, e.src_tile, e.dst_tile) + f_get_movement_aftercost(e.unit, e.src_tile, e.dst_tile);

            // reduce remaining movement points (even if failed)
            if (e.unit.movement >= movement_cost) {
                e.unit.movement = e.unit.movement - movement_cost;
            } else {
                e.unit.movement = 0.0;
            }

            return e.resolutions.is_movement_successful;
        });

    },

}
