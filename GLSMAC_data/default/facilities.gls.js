const manifest = #include('content/base_facilities');

const effects = {
	Headquarters: {energy_bonus: 1},
	ChildrenSCreche: {
		growth_rating_bonus: 2,
		efficiency_rating_bonus: 2,
		defender_morale_minimum: 1,
	},
	RecyclingTanks: {nutrient_bonus: 1, mineral_bonus: 1, energy_bonus: 1},
	PerimeterDefense: {defense_multiplier: 2.0},
	TachyonField: {defense_multiplier: 2.0},
	RecreationCommons: {drone_modifier: -2},
	EnergyBank: {economy_multiplier: 0.5},
	NetworkNode: {research_multiplier: 0.5},
	BiologyLab: {research_bonus: 2, native_lifecycle_bonus: 1, full_repair_native: true},
	Skunkworks: {prototype_cost_waiver: true},
	HologramTheatre: {drone_modifier: -2, psych_multiplier: 0.5},
	ParadiseGarden: {talent_bonus: 2},
	TreeFarm: {
		economy_multiplier: 0.5,
		psych_multiplier: 0.5,
		forest_nutrient_bonus: 1,
	},
	HybridForest: {
		economy_multiplier: 0.5,
		psych_multiplier: 0.5,
		forest_nutrient_bonus: 1,
		forest_energy_bonus: 1,
	},
	FusionLab: {economy_multiplier: 0.5, research_multiplier: 0.5},
	QuantumLab: {economy_multiplier: 0.5, research_multiplier: 0.5},
	ResearchHospital: {drone_modifier: -1, psych_multiplier: 0.25, research_multiplier: 0.5},
	Nanohospital: {drone_modifier: -1, psych_multiplier: 0.25, research_multiplier: 0.5},
	RoboticAssemblyPlant: {mineral_multiplier: 0.5},
	Nanoreplicator: {mineral_multiplier: 0.5},
	QuantumConverter: {mineral_multiplier: 0.5},
	GenejackFactory: {mineral_multiplier: 0.5, drone_modifier: 1},
	PunishmentSphere: {research_multiplier: -0.5, suppress_psych: true},
	HabComplex: {population_limit: 14},
	HabitationDome: {population_limit: 1000000, required_facility: 'HabComplex'},
	PressureDome: {nutrient_bonus: 1, mineral_bonus: 1, energy_bonus: 1},
	CommandCenter: {unit_morale_land_bonus: 2, full_repair_land: true},
	NavalYard: {
		unit_morale_water_bonus: 2,
		water_defense_multiplier: 2.0,
		full_repair_water: true,
	},
	AerospaceComplex: {
		unit_morale_air_bonus: 2,
		air_defense_multiplier: 2.0,
		full_repair_air: true,
	},
	BioenhancementCenter: {unit_morale_bonus: 2, native_lifecycle_bonus: 1},
	CentauriPreserve: {native_lifecycle_bonus: 1},
	TempleOfPlanet: {native_lifecycle_bonus: 1},
	PsiGate: {psi_gate: true},
	SkyHydroponicsLab: {
		orbital_resource: 'NUTRIENTS',
		required_facility: 'AerospaceComplex',
	},
	NessusMiningStation: {
		orbital_resource: 'MINERALS',
		required_facility: 'AerospaceComplex',
	},
	OrbitalPowerTransmitter: {
		orbital_resource: 'ENERGY',
		required_facility: 'AerospaceComplex',
	},
	OrbitalDefensePod: {
		orbital_defense: true,
		required_facility: 'AerospaceComplex',
	},
	StockpileEnergy: {mineral_to_energy_divisor: 2},
};

const partial_effects = {
	OrbitalDefensePod: true,
};

const project_effects = {
	TheHumanGenomeProject: {global_talent_bonus: 1},
	TheCommandNexus: {granted_facility: 'CommandCenter'},
	TheWeatherParadigm: {global_terraforming_rate_multiplier: 1.5},
	TheMerchantExchange: {worked_tile_energy_bonus: 1},
	TheEmpathGuild: {},
	TheCitizensDefenseForce: {granted_facility: 'PerimeterDefense'},
	TheVirtualWorld: {network_node_drone_modifier: -2},
	ThePlanetaryTransitSystem: {new_base_population: 3, small_base_drone_modifier: -1},
	TheXenoempathyDome: {global_native_lifecycle_bonus: 1},
	TheNeuralAmplifier: {global_psi_defense_multiplier: 1.5},
	TheMaritimeControlCenter: {
		granted_facility: 'NavalYard',
		global_naval_movement_bonus: 2.0,
	},
	ThePlanetaryDatalinks: {},
	TheSupercollider: {research_multiplier: 1.0},
	TheAsceticVirtues: {
		global_population_limit_bonus: 2,
		global_police_rating_bonus: 1,
	},
	TheLongevityVaccine: {},
	TheHunterSeekerAlgorithm: {},
	ThePholusMutagen: {global_native_lifecycle_bonus: 1},
	TheUniversalTranslator: {},
	TheCyborgFactory: {granted_facility: 'BioenhancementCenter'},
	TheTheoryOfEverything: {research_multiplier: 1.0},
	TheDreamTwister: {global_psi_attack_multiplier: 1.5},
	TheNetworkBackbone: {},
	TheNanoFactory: {global_full_repair: true},
	TheLivingRefinery: {global_support_bonus: 2},
	TheCloningVats: {global_growth_rating_bonus: 10},
	TheSelfAwareColony: {
		global_maintenance_multiplier: 0.5,
		global_extra_police_units: 1,
	},
	ClinicalImmortality: {global_talent_bonus: 1},
	TheSpaceElevator: {economy_multiplier: 1.0},
	TheSingularityInductor: {granted_facility: 'QuantumConverter'},
	TheBulkMatterTransmitter: {global_mineral_bonus: 2},
	TheTelepathicMatrix: {global_prevent_riots: true},
	TheVoiceOfPlanet: {global_native_lifecycle_bonus: 1},
	TheAscentToTranscendence: {},
};

const partial_project_effects = {
	TheSpaceElevator: true,
};

const facilities = [];
const coverage = {complete: 0, partial: 0, status: {}};
const project_coverage = {complete: 0, partial: 0, status: {}};
for (entry of manifest) {
	if (entry.kind == 'facility' && !#is_defined(effects[entry.id])) {
		continue;
	}
	const is_project = entry.kind == 'project';
	let data = {
		name: entry.name,
		mineral_cost: entry.mineral_cost,
		nutrient_bonus: 0,
		mineral_bonus: 0,
		energy_bonus: 0,
		energy_maintenance: entry.energy_maintenance,
		required_technology: entry.required_technology,
		required_project: #is_defined(entry.required_project) ? entry.required_project : '',
		is_project: is_project,
	};
	const implemented_effects = is_project ? project_effects[entry.id] : effects[entry.id];
	if (#is_defined(implemented_effects)) {
		for (key in implemented_effects) {
			data[key] = implemented_effects[key];
		}
	}
	facilities :+{id: entry.id, data: data};
	if (is_project) {
		const status = #is_defined(implemented_effects) &&
			!#is_defined(partial_project_effects[entry.id])
			? 'complete'
			: 'partial';
		project_coverage.status[entry.id] = status;
		project_coverage[status] = project_coverage[status] + 1;
	} else {
		const status = #is_defined(partial_effects[entry.id]) ? 'partial' : 'complete';
		coverage.status[entry.id] = status;
		coverage[status] = coverage[status] + 1;
	}
}

return {
	definitions: facilities,
	manifest: manifest,
	coverage: coverage,
	project_coverage: project_coverage,

	define: (game) => {
		for (facility of facilities) {
			game.event('define_base_facility', facility);
		}
	},
};
