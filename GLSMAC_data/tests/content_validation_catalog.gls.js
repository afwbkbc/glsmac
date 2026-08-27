const content = #include('./_content_validation_common');
const catalog = content.make_catalog();
const result = content.validator.validate(catalog);

for (error of result.errors) {
	#print('CONTENT_VALIDATION_ERROR: ' + error);
}
test.assert(result.errors == []);
test.assert(content.get_facility(catalog, 'TreeFarm').data.forest_nutrient_bonus == 1);
test.assert(content.get_facility(catalog, 'HybridForest').data.forest_nutrient_bonus == 1);
test.assert(content.get_facility(catalog, 'HybridForest').data.forest_energy_bonus == 1);
test.assert(catalog.facility_coverage.status.TreeFarm == 'complete');
test.assert(catalog.facility_coverage.status.HybridForest == 'complete');
test.assert(content.get_facility(catalog, 'Headquarters').data.energy_bonus == 1);
test.assert(!#is_defined(content.get_facility(catalog, 'Headquarters').data.defender_morale_bonus));
test.assert(catalog.facility_coverage.status.Headquarters == 'complete');
test.assert(content.get_facility(catalog, 'ChildrenSCreche').data.efficiency_rating_bonus == 2);
test.assert(content.get_facility(catalog, 'ChildrenSCreche').data.defender_morale_minimum == 1);
test.assert(catalog.facility_coverage.status.ChildrenSCreche == 'complete');
test.assert(catalog.facility_coverage.status.BioenhancementCenter == 'complete');
test.assert(content.get_facility(catalog, 'PunishmentSphere').data.research_multiplier == -0.5);
test.assert(content.get_facility(catalog, 'PunishmentSphere').data.suppress_psych);
test.assert(catalog.facility_coverage.status.PunishmentSphere == 'complete');
test.assert(content.get_facility(catalog, 'GenejackFactory').data.mineral_multiplier == 0.5);
test.assert(content.get_facility(catalog, 'GenejackFactory').data.drone_modifier == 1);
test.assert(catalog.facility_coverage.status.GenejackFactory == 'complete');
test.assert(content.get_facility(catalog, 'CentauriPreserve').data.native_lifecycle_bonus == 1);
test.assert(content.get_facility(catalog, 'TempleOfPlanet').data.native_lifecycle_bonus == 1);
test.assert(catalog.facility_coverage.status.CentauriPreserve == 'complete');
test.assert(catalog.facility_coverage.status.TempleOfPlanet == 'complete');
test.assert(content.get_facility(catalog, 'PsiGate').data.psi_gate);
test.assert(content.get_facility(catalog, 'BiologyLab').data.full_repair_native);
test.assert(content.get_facility(catalog, 'Skunkworks').data.prototype_cost_waiver);
test.assert(catalog.facility_coverage.status.Skunkworks == 'complete');
test.assert(
	content.get_facility(catalog, 'StockpileEnergy').data.mineral_to_energy_divisor == 2
);
test.assert(catalog.facility_coverage.status.StockpileEnergy == 'complete');
test.assert(content.get_facility(catalog, 'CommandCenter').data.full_repair_land);
test.assert(content.get_facility(catalog, 'NavalYard').data.full_repair_water);
test.assert(content.get_facility(catalog, 'AerospaceComplex').data.full_repair_air);
test.assert(catalog.facility_coverage.status.AerospaceComplex == 'complete');
test.assert(
	content.get_facility(catalog, 'SkyHydroponicsLab').data.orbital_resource ==
	'NUTRIENTS'
);
test.assert(
	content.get_facility(catalog, 'NessusMiningStation').data.orbital_resource ==
	'MINERALS'
);
test.assert(
	content.get_facility(catalog, 'OrbitalPowerTransmitter').data.orbital_resource ==
	'ENERGY'
);
test.assert(content.get_facility(catalog, 'OrbitalDefensePod').data.orbital_defense);
for (id of [
	'SkyHydroponicsLab', 'NessusMiningStation', 'OrbitalPowerTransmitter',
	'OrbitalDefensePod',
]) {
	test.assert(content.get_facility(catalog, id).data.required_facility == 'AerospaceComplex');
}
test.assert(content.get_facility(catalog, 'TheAsceticVirtues').data.global_police_rating_bonus == 1);
test.assert(content.get_facility(catalog, 'TheSelfAwareColony').data.global_extra_police_units == 1);
test.assert(catalog.project_coverage.status.TheLongevityVaccine == 'complete');
test.assert(catalog.project_coverage.status.TheCloningVats == 'complete');
test.assert(catalog.project_coverage.status.TheTelepathicMatrix == 'complete');
test.assert(content.get_facility(catalog, 'TheVoiceOfPlanet').data.global_native_lifecycle_bonus == 1);
test.assert(catalog.project_coverage.status.TheVoiceOfPlanet == 'complete');
test.assert(catalog.project_coverage.status.TheHunterSeekerAlgorithm == 'complete');
test.assert(catalog.project_coverage.status.ThePlanetaryDatalinks == 'complete');
test.assert(content.get_facility(catalog, 'ThePholusMutagen').data.global_native_lifecycle_bonus == 1);
test.assert(catalog.project_coverage.status.ThePholusMutagen == 'complete');
test.assert(content.get_facility(catalog, 'TheXenoempathyDome').data.global_native_lifecycle_bonus == 1);
test.assert(catalog.project_coverage.status.TheXenoempathyDome == 'complete');
test.assert(catalog.project_coverage.status.TheNetworkBackbone == 'complete');
test.assert(catalog.facility_coverage.status.NetworkNode == 'complete');
test.assert(catalog.project_coverage.status.TheUniversalTranslator == 'complete');
test.assert(catalog.project_coverage.status.TheNanoFactory == 'complete');
test.assert(catalog.project_coverage.status.TheEmpathGuild == 'complete');
for (id of [
	'TheSpaceElevator',
]) {
	test.assert(catalog.project_coverage.status[id] == 'partial');
}
for (id of ['OrbitalDefensePod']) {
	test.assert(catalog.facility_coverage.status[id] == 'partial');
}
for (id of [
	'ResearchHospital', 'Nanohospital', 'PsiGate', 'PressureDome',
]) {
	test.assert(catalog.facility_coverage.status[id] == 'complete');
}
let planet = null;
for (faction of catalog.factions) {
	if (faction.id == 'PLANET') {
		planet = faction;
		break;
	}
}
test.assert(planet != null && planet.data.is_native);
test.assert(result.counts == {
	technologies: 77,
	facilities: 38,
	complete_facilities: 37,
	partial_facilities: 1,
	implemented_projects: 33,
	complete_projects: 32,
	partial_projects: 1,
	base_facilities: 38,
	projects: 33,
	units: #sizeof(content.units.definitions),
	chassis: 9,
	reactors: 4,
	weapons: 21,
	armors: 10,
	abilities: 24,
	predefined_units: 14,
	moralesets: 2,
	factions: 15,
});
