const orders = {
	road: {
		name: 'Road',
		turns: 1,
		required_technology: '',
		changes: {road: true},
	},
	mag_tube: {
		name: 'Mag Tube',
		turns: 3,
		required_technology: 'MonopoleMagnets',
		changes: {mag_tube: true},
	},
	forest: {
		name: 'Forest',
		turns: 4,
		required_technology: '',
		changes: {
			forest: true,
			farm: false,
			soil_enricher: false,
			mine: false,
			solar: false,
			condenser: false,
			mirror: false,
			borehole: false,
		},
	},
	farm: {
		name: 'Farm',
		turns: 4,
		required_technology: '',
		changes: {forest: false, borehole: false, farm: true},
	},
	soil_enricher: {
		name: 'Soil Enricher',
		turns: 8,
		required_technology: 'AdvancedEcologicalEngineering',
		changes: {soil_enricher: true},
	},
	mine: {
		name: 'Mine',
		turns: 8,
		required_technology: '',
		changes: {forest: false, solar: false, mirror: false, borehole: false, mine: true},
	},
	solar: {
		name: 'Solar Collector',
		turns: 4,
		required_technology: '',
		changes: {forest: false, mine: false, mirror: false, borehole: false, solar: true},
	},
	condenser: {
		name: 'Condenser',
		turns: 12,
		required_technology: 'EcologicalEngineering',
		changes: {forest: false, borehole: false, condenser: true},
	},
	mirror: {
		name: 'Echelon Mirror',
		turns: 12,
		required_technology: 'EcologicalEngineering',
		changes: {forest: false, mine: false, solar: false, borehole: false, mirror: true},
	},
	borehole: {
		name: 'Thermal Borehole',
		turns: 24,
		required_technology: 'EcologicalEngineering',
		changes: {
			forest: false,
			farm: false,
			soil_enricher: false,
			mine: false,
			solar: false,
			condenser: false,
			mirror: false,
			borehole: true,
		},
	},
	sensor: {
		name: 'Sensor Array',
		turns: 4,
		required_technology: '',
		changes: {sensor: true},
	},
	bunker: {
		name: 'Bunker',
		turns: 5,
		required_technology: 'AdvancedMilitaryAlgorithms',
		changes: {bunker: true},
	},
	airbase: {
		name: 'Airbase',
		turns: 10,
		required_technology: 'DoctrineAirPower',
		changes: {airbase: true},
	},
	remove_fungus: {
		name: 'Remove Fungus',
		turns: 6,
		required_technology: '',
		feature_changes: {xenofungus: false},
	},
	plant_fungus: {
		name: 'Plant Fungus',
		turns: 6,
		required_technology: 'EcologicalEngineering',
		changes: {
			forest: false,
			farm: false,
			soil_enricher: false,
			mine: false,
			solar: false,
			condenser: false,
			mirror: false,
			borehole: false,
		},
		feature_changes: {xenofungus: true},
	},
};

const order_ids = [
	'road',
	'mag_tube',
	'forest',
	'farm',
	'soil_enricher',
	'mine',
	'solar',
	'condenser',
	'mirror',
	'borehole',
	'sensor',
	'bunker',
	'airbase',
	'remove_fungus',
	'plant_fungus',
];

const get_order = (type) => {
	if (!#is_defined(orders[type])) {
		return null;
	}
	return orders[type];
};

const has_technology = (player, technology_id) => {
	return technology_id == '' || (
		#is_defined(player) &&
		#is_defined(player.has_technology) &&
		player.has_technology(technology_id)
	);
};

const get_unavailable_reason = (tile, player, type) => {
	const order = get_order(type);
	if (order == null) {
		return 'Unknown terraforming order';
	}
	if (tile.is_water) {
		return 'Land Formers cannot terraform sea squares';
	}
	if (tile.get_base() != null) {
		return 'This improvement cannot be built at a base';
	}
	if (tile.features.monolith) {
		return 'Monoliths cannot be terraformed';
	}
	if (!has_technology(player, order.required_technology)) {
		return 'Required technology has not been discovered';
	}
	if (type == 'plant_fungus' && tile.features.xenofungus) {
		return 'This square already contains xenofungus';
	}
	if (tile.features.xenofungus) {
		if (type == 'remove_fungus') {
			return null;
		}
		if (type != 'road' || !has_technology(player, 'CentauriEmpathy')) {
			return 'Xenofungus must be removed before building this improvement';
		}
	} else if (type == 'remove_fungus') {
		return 'This square has no xenofungus to remove';
	}
	if (type != 'remove_fungus' && type != 'plant_fungus' && tile.terraforming[type]) {
		return 'Tile already has this improvement';
	}
	if ((type == 'farm' || type == 'soil_enricher') && tile.rockiness == 3) {
		return 'Farms cannot be built in rocky squares';
	}
	if (type == 'soil_enricher' && !tile.terraforming.farm) {
		return 'A Soil Enricher requires a Farm';
	}
	if (type == 'mag_tube' && !tile.terraforming.road) {
		return 'A Mag Tube requires a Road';
	}
	if (type == 'borehole') {
		for (nearby of tile.get_surrounding_tiles()) {
			if (nearby.terraforming.borehole) {
				return 'Thermal Boreholes cannot be built in adjacent squares';
			}
		}
	}
	return null;
};

const advance_order = (unit) => {
	const type = unit.terraforming;
	const order = get_order(type);
	if (order == null) {
		throw Error('Unknown terraforming order: ' + type);
	}
	if (unit.terraforming_turns_remaining > 1) {
		unit.set_terraforming_order(type, unit.terraforming_turns_remaining - 1);
		unit.movement = 0.0;
		return true;
	}

	const tile = unit.get_tile();
	if (#is_defined(order.changes)) {
		tile.update_terraforming(order.changes);
	}
	if (#is_defined(order.feature_changes)) {
		tile.update_features(order.feature_changes);
	}
	unit.set_terraforming_order('none', 0);
	return false;
};

return {
	orders: orders,
	order_ids: order_ids,
	get_order: get_order,
	get_unavailable_reason: get_unavailable_reason,
	advance_order: advance_order,
};
