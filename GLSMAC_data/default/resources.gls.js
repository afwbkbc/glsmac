const define = (game, id, coords) => {

	game.event('define_resource', {
		name: id,
		render: {
			type: 'sprites',
			file: 'newicons.pcx',
			coords: coords,
		}
	});

};

const rules = #include('content/resource_rules');

const empty_yields = () => {
	return {NUTRIENTS: 0, MINERALS: 0, ENERGY: 0};
};

const copy_yields = (values) => {
	return {
		NUTRIENTS: values.NUTRIENTS,
		MINERALS: values.MINERALS,
		ENERGY: values.ENERGY,
	};
};

const has_technology = (player, id) => {
	return #is_defined(player) &&
		#is_defined(player.has_technology) &&
		player.has_technology(id);
};

const add_fungus_yields = (result, player) => {
	for (bonus of rules.fungus_technology_bonuses) {
		if (has_technology(player, bonus.technology)) {
			result[bonus.resource] = result[bonus.resource] + bonus.amount;
		}
	}
	if (!#is_defined(player) || !#is_defined(player.get_faction)) {
		return;
	}
	const faction = player.get_faction();
	if (!#is_defined(rules.fungus_faction_bonuses[faction.id])) {
		return;
	}
	const faction_bonus = rules.fungus_faction_bonuses[faction.id];
	for (resource of ['NUTRIENTS', 'MINERALS', 'ENERGY']) {
		result[resource] = result[resource] + faction_bonus[resource];
	}
};

const add_resource_bonus = (result, tile) => {
	if (tile.bonuses.nutrient) {
		result.NUTRIENTS = result.NUTRIENTS + rules.bonus_amount;
	}
	if (tile.bonuses.minerals) {
		result.MINERALS = result.MINERALS + rules.bonus_amount;
		if (tile.terraforming.mine) {
			result.MINERALS = result.MINERALS + 1;
		}
	}
	if (tile.bonuses.energy) {
		result.ENERGY = result.ENERGY + rules.bonus_amount;
	}
};

const get_adjacent_mirror_bonus = (tile) => {
	if (!#is_defined(tile.get_surrounding_tiles)) {
		return 0;
	}
	let result = 0;
	for (nearby of tile.get_surrounding_tiles()) {
		if (nearby.terraforming.mirror) {
			result++;
		}
	}
	return result;
};

const get_land_yields = (tile) => {
	if (tile.terraforming.borehole) {
		return copy_yields(rules.borehole_yields);
	}
	if (tile.terraforming.forest) {
		return copy_yields(rules.forest_yields);
	}
	const result = empty_yields();
	if (tile.rockiness < 3) {
		result.NUTRIENTS = #max(tile.moisture - 1, 0);
	}
	if (tile.rockiness > 1) {
		result.MINERALS = 1;
	}
	if (tile.terraforming.farm) {
		result.NUTRIENTS = result.NUTRIENTS + 1;
	}
	if (tile.terraforming.soil_enricher) {
		result.NUTRIENTS = result.NUTRIENTS + 1;
	}
	if (tile.terraforming.condenser) {
		result.NUTRIENTS = result.NUTRIENTS + 1;
	}
	if (tile.terraforming.mine) {
		if (tile.rockiness == 3) {
			result.MINERALS = result.MINERALS + (tile.terraforming.road ? 3 : 2);
		} else {
			result.MINERALS = result.MINERALS + 1;
		}
		result.NUTRIENTS = #max(result.NUTRIENTS - 1, 0);
	}
	if (tile.terraforming.solar || tile.terraforming.mirror) {
		const sea_level = #is_defined(tile.sea_level) ? tile.sea_level : 0;
		result.ENERGY = result.ENERGY +
			#max(#floor(#to_float(tile.elevation - sea_level) / 1000.0), 0) + 1;
		if (tile.terraforming.solar) {
			result.ENERGY = result.ENERGY + get_adjacent_mirror_bonus(tile);
		}
	}
	return result;
};

const get_sea_yields = (tile, player) => {
	const result = copy_yields(rules.ocean_yields);
	if (tile.terraforming.farm) {
		result.NUTRIENTS = result.NUTRIENTS + 2;
	}
	if (tile.terraforming.mine) {
		result.MINERALS = result.MINERALS + (
			has_technology(player, 'AdvancedEcologicalEngineering') ? 2 : 1
		);
	}
	if (tile.terraforming.solar) {
		result.ENERGY = 3;
	}
	return result;
};

const apply_resource_caps = (result, tile, player) => {
	for (resource of ['NUTRIENTS', 'MINERALS', 'ENERGY']) {
		const cap = rules.resource_caps[resource];
		if (
			!has_technology(player, cap.technology) &&
			!tile.bonuses[cap.bonus_key]
		) {
			result[resource] = #min(result[resource], cap.limit);
		}
	}
};

const get_tile_yields = (tile, player) => {
	if (tile.features.monolith) {
		return copy_yields(rules.monolith_yields);
	}
	let result = empty_yields();
	if (tile.features.xenofungus) {
		add_fungus_yields(result, player);
	} else {
		result = tile.is_land ? get_land_yields(tile) : get_sea_yields(tile, player);
		if (tile.is_land && tile.features.jungle) {
			result.NUTRIENTS = result.NUTRIENTS + 1;
		}
		if (tile.is_land && tile.features.river) {
			result.ENERGY = result.ENERGY + 1;
		}
	}
	add_resource_bonus(result, tile);
	if (tile.get_base() != null) {
		result.NUTRIENTS = #max(result.NUTRIENTS, rules.base_yields.NUTRIENTS);
		result.MINERALS = #max(result.MINERALS, rules.base_yields.MINERALS);
		const base_energy = rules.base_yields.ENERGY + (
			tile.features.river ? 1 : 0
		);
		result.ENERGY = #max(result.ENERGY, base_energy);
	}
	apply_resource_caps(result, tile, player);
	return result;
};

const result = {
	rules: rules,
	get_tile_yields: get_tile_yields,

	configure: (game) => {

		game.get_tm().on('get_tile_resources', (e) => {
			return get_tile_yields(e.tile, e.player);
		});

	},

	define: (game) => {
		define(game, 'NUTRIENTS', [
			[184, 314, 201, 331],
			[223, 312, 245, 334],
			[262, 310, 288, 336],
			[301, 308, 331, 338],
			[339, 305, 376, 342],
			[380, 305, 417, 342],
			[421, 305, 458, 342],
			[462, 305, 499, 342],
		]);
		define(game, 'MINERALS', [
			[185, 356, 202, 373],
			[223, 355, 245, 377],
			[263, 353, 290, 378],
			[302, 350, 332, 380],
			[339, 346, 376, 383],
			[380, 346, 417, 383],
			[421, 346, 458, 383],
			[462, 346, 499, 383],
		]);
		define(game, 'ENERGY', [
			[186, 397, 202, 414],
			[224, 395, 245, 417],
			[262, 392, 289, 419],
			[301, 390, 332, 421],
			[339, 387, 376, 424],
			[380, 387, 417, 424],
			[421, 387, 458, 424],
			[462, 387, 499, 424],
		]);
		game.event('define_no_resource', {
			render: {
				type: 'sprites',
				file: 'newicons.pcx',
				coords: [2, 175, 22, 194],
			}
		});

	},

};

return result;
