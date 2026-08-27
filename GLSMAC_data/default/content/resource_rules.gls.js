return {
	resource_caps: {
		NUTRIENTS: {limit: 2, technology: 'GeneSplicing', bonus_key: 'nutrient'},
		MINERALS: {limit: 2, technology: 'EcologicalEngineering', bonus_key: 'minerals'},
		ENERGY: {limit: 2, technology: 'EnvironmentalEconomics', bonus_key: 'energy'},
	},
	fungus_technology_bonuses: [
		{technology: 'CentauriEcology', resource: 'NUTRIENTS', amount: 1},
		{technology: 'CentauriPsi', resource: 'NUTRIENTS', amount: 1},
		{technology: 'CentauriGenetics', resource: 'MINERALS', amount: 1},
		{technology: 'MatterTransmission', resource: 'MINERALS', amount: 1},
		{technology: 'ThresholdOfTranscendence', resource: 'MINERALS', amount: 1},
		{technology: 'CentauriMeditation', resource: 'ENERGY', amount: 1},
		{technology: 'SecretsOfAlphaCentauri', resource: 'ENERGY', amount: 1},
		{technology: 'TemporalMechanics', resource: 'ENERGY', amount: 1},
	],
	fungus_faction_bonuses: {
		GAIANS: {NUTRIENTS: 1, MINERALS: 0, ENERGY: 0},
	},
	base_yields: {NUTRIENTS: 2, MINERALS: 1, ENERGY: 1},
	monolith_yields: {NUTRIENTS: 2, MINERALS: 2, ENERGY: 2},
	borehole_yields: {NUTRIENTS: 0, MINERALS: 6, ENERGY: 6},
	forest_yields: {NUTRIENTS: 1, MINERALS: 2, ENERGY: 1},
	ocean_yields: {NUTRIENTS: 1, MINERALS: 0, ENERGY: 1},
	bonus_amount: 2,
};
