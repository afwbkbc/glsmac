const tile_has_friendly_unit = (unit, tile) => {
	for (other of tile.get_units()) {
		if (other.owner == unit.owner) {
			return true;
		}
	}
	return false;
};

const tile_is_in_enemy_zoc = (unit, tile) => {
	for (nearby of tile.get_surrounding_tiles()) {
		const base = nearby.get_base();
		if (base != null && base.get_owner().id != unit.owner) {
			return true;
		}
		for (other of nearby.get_units()) {
			if (other.owner != unit.owner && (other.is_land || other.is_air)) {
				return true;
			}
		}
	}
	return false;
};

const is_zoc_move_blocked = (unit, source, destination) => {
	if (!unit.is_land || destination.get_base() != null || tile_has_friendly_unit(unit, destination)) {
		return false;
	}
	return tile_is_in_enemy_zoc(unit, source) && tile_is_in_enemy_zoc(unit, destination);
};

return {
	is_zoc_move_blocked: is_zoc_move_blocked,
	tile_is_in_enemy_zoc: tile_is_in_enemy_zoc,
};
