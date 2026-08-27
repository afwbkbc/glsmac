const orders = #include('../../units/terraforming');

const WORKED_TILE_BONUS = 2000;

const get_order = (tile, prioritize_nutrients, player) => {
	const is_available = (type) => {
		return orders.get_unavailable_reason(tile, player, type) == null;
	};
	if (tile.features.xenofungus) {
		return is_available('remove_fungus') ? 'remove_fungus' : null;
	}
	if (tile.rockiness == 3) {
		if (is_available('mine')) {
			return 'mine';
		}
		if (is_available('road')) {
			return 'road';
		}
		return is_available('mag_tube') ? 'mag_tube' : null;
	}
	if (!tile.terraforming.forest && !tile.terraforming.farm) {
		if (prioritize_nutrients && tile.moisture > 0 && is_available('farm')) {
			return 'farm';
		}
		const basic = tile.moisture <= 1 || tile.rockiness >= 2 ? 'forest' : 'farm';
		return is_available(basic) ? basic : null;
	}
	if (is_available('road')) {
		return 'road';
	}
	if (tile.terraforming.farm) {
		if (prioritize_nutrients && is_available('condenser')) {
			return 'condenser';
		}
		if (is_available('soil_enricher')) {
			return 'soil_enricher';
		}
		if (is_available('solar')) {
			return 'solar';
		}
	}
	return is_available('mag_tube') ? 'mag_tube' : null;
};

const get_target_score = (tile, player, pending_growth, distance, is_worked) => {
	const resources = tile.get_resources(player);
	let score = resources.NUTRIENTS * 30 + resources.MINERALS * 20 + resources.ENERGY * 10;
	if (pending_growth <= 0) {
		score += 10000 + resources.NUTRIENTS * 100;
	}
	if (#is_defined(is_worked) && is_worked) {
		score += WORKED_TILE_BONUS;
	}
	return score - distance * 100;
};

return {
	get_order: get_order,
	get_target_score: get_target_score,
};
