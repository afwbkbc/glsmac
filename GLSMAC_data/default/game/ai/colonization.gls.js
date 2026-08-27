const MIN_BASE_DISTANCE = 3;
const IDEAL_BASE_DISTANCE = 4;
const TRAVEL_DISTANCE_PENALTY = 25;

const get_resource_value = (resources) => {
	return resources.NUTRIENTS * 4 + resources.MINERALS * 3 + resources.ENERGY * 2;
};

const get_nearest_base_distance = (tm, tile, bases) => {
	let result = 100000;
	for (base of bases) {
		result = #min(result, tm.get_distance(tile, base.get_tile()));
	}
	return result;
};

const is_valid_site = (tm, tile, owner_id, bases, is_water) => {
	const target_is_water = #is_defined(is_water) ? is_water : false;
	if (
		tile.is_locked() || tile.is_water != target_is_water ||
		tile.get_base() != null
	) {
		return false;
	}
	if (get_nearest_base_distance(tm, tile, bases) < MIN_BASE_DISTANCE) {
		return false;
	}
	for (unit of tile.get_units()) {
		if (unit.owner != owner_id) {
			return false;
		}
	}
	return true;
};

const get_site_score = (tm, tile, player, bases, is_water) => {
	if (!is_valid_site(tm, tile, player.id, bases, is_water)) {
		return null;
	}
	const distance = get_nearest_base_distance(tm, tile, bases);
	let score = #min(distance, IDEAL_BASE_DISTANCE) * 100;
	score += get_resource_value(tile.get_resources(player)) * 4;
	for (nearby of tile.get_surrounding_tiles()) {
		score += get_resource_value(nearby.get_resources(player));
	}
	return score;
};

const get_destination_score = (tm, tile, player, bases, travel_distance, is_water) => {
	const site_score = get_site_score(tm, tile, player, bases, is_water);
	return site_score == null ? null : site_score - travel_distance * TRAVEL_DISTANCE_PENALTY;
};

return {
	get_nearest_base_distance: get_nearest_base_distance,
	is_valid_site: is_valid_site,
	get_site_score: get_site_score,
	get_destination_score: get_destination_score,
};
