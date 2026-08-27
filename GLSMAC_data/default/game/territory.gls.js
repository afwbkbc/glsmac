const MAX_BASE_DISTANCE = 8;
const COASTAL_CLAIM_DISTANCE = 2;

const can_claim = (base_tile, tile, distance) => {
	if (base_tile.is_water == tile.is_water) {
		return true;
	}
	return !base_tile.is_water && tile.is_water && distance <= COASTAL_CLAIM_DISTANCE;
};

const get_claiming_base = (game, tile) => {
	let result = null;
	let result_distance = MAX_BASE_DISTANCE + 1;
	for (base of game.get_bm().get_bases()) {
		const base_tile = base.get_tile();
		const distance = game.get_tm().get_distance(tile, base_tile);
		if (
			distance > MAX_BASE_DISTANCE ||
			!can_claim(base_tile, tile, distance)
		) {
			continue;
		}
		if (
			result == null || distance < result_distance ||
			(distance == result_distance && base.id < result.id)
		) {
			result = base;
			result_distance = distance;
		}
	}
	return result;
};

return (game) => {
	game.on('start', (e) => {
		const get_base = (tile) => { return get_claiming_base(game, tile); };
		const get_owner = (tile) => {
			const base = get_base(tile);
			return base == null ? null : base.get_owner();
		};
		game.set('f_territory_get_base', get_base);
		game.set('f_territory_get_owner', get_owner);
		game.set('f_territory_is_friendly', (player, tile) => {
			const owner = get_owner(tile);
			return owner != null && owner.id == player.id;
		});
		game.set('f_territory_get_max_distance', () => { return MAX_BASE_DISTANCE; });
	});
};
