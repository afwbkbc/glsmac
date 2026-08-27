const production = #include('../default/game/ai/production');

const unit = (id, offense, defense, movement, cost, can_found_base, can_terraform) => {
	return {
		id: id,
		production_kind: 'facility',
		offense: offense,
		defense: defense,
		movement_per_turn: movement,
		mineral_cost: cost,
		can_found_base: can_found_base,
		can_terraform: can_terraform,
		weapon: '',
		abilities: [],
	};
};
const facility = (id, nutrients, minerals, energy, psych, research, maintenance, cost, defense_multiplier, economy_multiplier, unit_morale_bonus, research_bonus, mineral_multiplier, psych_multiplier) => {
	return {
		id: id,
		nutrient_bonus: nutrients,
		mineral_bonus: minerals,
		energy_bonus: energy,
		psych_bonus: psych,
		research_multiplier: research,
		energy_maintenance: maintenance,
		mineral_cost: cost,
		defense_multiplier: #is_defined(defense_multiplier) ? defense_multiplier : 1.0,
		economy_multiplier: #is_defined(economy_multiplier) ? economy_multiplier : 0.0,
		unit_morale_bonus: #is_defined(unit_morale_bonus) ? unit_morale_bonus : 0,
		research_bonus: #is_defined(research_bonus) ? research_bonus : 0,
		mineral_multiplier: #is_defined(mineral_multiplier) ? mineral_multiplier : 0.0,
		psych_multiplier: #is_defined(psych_multiplier) ? psych_multiplier : 0.0,
		population_limit: 0,
		drone_modifier: 0,
		talent_bonus: 0,
		suppress_psych: false,
		unit_morale_land_bonus: 0,
		unit_morale_water_bonus: 0,
		unit_morale_air_bonus: 0,
		water_defense_multiplier: 1.0,
		air_defense_multiplier: 1.0,
		growth_rating_bonus: 0,
		native_lifecycle_bonus: 0,
		granted_facility: '',
		global_talent_bonus: 0,
		global_growth_rating_bonus: 0,
		global_population_limit_bonus: 0,
		global_mineral_bonus: 0,
		global_support_bonus: 0,
		global_maintenance_multiplier: 1.0,
		global_native_lifecycle_bonus: 0,
		network_node_drone_modifier: 0,
		network_node_research_bonus: 0,
		worked_tile_energy_bonus: 0,
		global_prevent_riots: false,
		global_terraforming_rate_multiplier: 1.0,
		new_base_population: 0,
		small_base_drone_modifier: 0,
		global_psi_attack_multiplier: 1.0,
		global_psi_defense_multiplier: 1.0,
		global_naval_movement_bonus: 0.0,
		global_full_repair: false,
		global_police_rating_bonus: 0,
		global_extra_police_units: 0,
		efficiency_rating_bonus: 0,
		defender_morale_minimum: 0,
		mineral_to_energy_divisor: 0,
		orbital_resource: '',
		orbital_defense: false,
	};
};

const scout = unit('Scout', 1, 1, 1.0, 10, false, false);
const rover = unit('Rover', 1, 1, 2.0, 20, false, false);
const laser = unit('Laser', 2, 1, 1.0, 20, false, false);
const defender = unit('Defender', 1, 2, 1.0, 20, false, false);
const former = unit('Former', 0, 1, 1.0, 20, false, true);
const colony = unit('Colony', 0, 1, 1.0, 30, true, false);
const probe_team = unit('ProbeTeam', 0, 1, 2.0, 40, false, false);
probe_team.weapon = 'ProbeTeam';
const planet_buster_unit = unit('PlanetBuster', 99, 1, 12.0, 225, false, false);
planet_buster_unit.weapon = 'PlanetBuster';
const sea_colony = unit('SeaColony', 0, 1, 4.0, 70, true, false);
sea_colony.is_water = true;
const recycling = facility('Recycling', 1, 1, 1, 0, 0.0, 0, 40);
const headquarters = facility('Headquarters', 0, 0, 1, 0, 0.0, 0, 50);
const network = facility('Network', 0, 0, 0, 0, 0.5, 1, 80);
const recreation = facility('Recreation', 0, 0, 0, 4, 0.0, 1, 40);
const perimeter = facility('Perimeter', 0, 0, 0, 0, 0.0, 0, 50, 2.0);
const energy_bank = facility('EnergyBank', 0, 0, 0, 0, 0.0, 1, 80, 1.0, 0.5);
const stockpile = facility('StockpileEnergy', 0, 0, 0, 0, 0.0, 0, 0);
stockpile.mineral_to_energy_divisor = 2;
const sky_hydroponics = facility('SkyHydroponicsLab', 0, 0, 0, 0, 0.0, 0, 120);
sky_hydroponics.orbital_resource = 'NUTRIENTS';
const orbital_defense = facility('OrbitalDefensePod', 0, 0, 0, 0, 0.0, 0, 120);
orbital_defense.orbital_defense = true;
const command_center = facility('CommandCenter', 0, 0, 0, 0, 0.0, 1, 40);
command_center.unit_morale_land_bonus = 2;
const naval_yard = facility('NavalYard', 0, 0, 0, 0, 0.0, 2, 80);
naval_yard.unit_morale_water_bonus = 2;
naval_yard.water_defense_multiplier = 2.0;
const biology_lab = facility('BiologyLab', 0, 0, 0, 0, 0.0, 1, 60, 1.0, 0.0, 0, 2);
biology_lab.native_lifecycle_bonus = 1;
const childrens_creche = facility('ChildrenSCreche', 0, 0, 0, 0, 0.0, 1, 50);
childrens_creche.growth_rating_bonus = 2;
childrens_creche.efficiency_rating_bonus = 2;
childrens_creche.defender_morale_minimum = 1;
const command_nexus = facility('TheCommandNexus', 0, 0, 0, 0, 0.0, 0, 200);
command_nexus.production_kind = 'project';
command_nexus.granted_facility = 'CommandCenter';
const planetary_datalinks = facility('ThePlanetaryDatalinks', 0, 0, 0, 0, 0.0, 0, 300);
planetary_datalinks.production_kind = 'project';
const empath_guild = facility('TheEmpathGuild', 0, 0, 0, 0, 0.0, 0, 200);
empath_guild.production_kind = 'project';
const pholus_mutagen = facility('ThePholusMutagen', 0, 0, 0, 0, 0.0, 0, 400);
pholus_mutagen.production_kind = 'project';
pholus_mutagen.global_native_lifecycle_bonus = 1;
const xenoempathy_dome = facility('TheXenoempathyDome', 0, 0, 0, 0, 0.0, 0, 300);
xenoempathy_dome.production_kind = 'project';
xenoempathy_dome.global_native_lifecycle_bonus = 1;
const universal_translator = facility('TheUniversalTranslator', 0, 0, 0, 0, 0.0, 0, 300);
universal_translator.production_kind = 'project';
const network_backbone = facility('TheNetworkBackbone', 0, 0, 0, 0, 0.0, 0, 300);
network_backbone.production_kind = 'project';
const all_units = [scout, rover, laser, defender, former, colony];
const all_facilities = [network, recreation, recycling];
let locked = {};
const base = {
	can_set_production: (kind, id) => { return !#is_defined(locked[id]); },
};
const context = (garrison, needs_former, needs_colony, needs_psych, energy) => {
	return {
		needs_garrison: garrison,
		needs_former: needs_former,
		needs_colony: needs_colony,
		needs_probe: false,
		needs_military: true,
		needs_psych: needs_psych,
		needs_growth: false,
		can_expand: true,
		can_start_project: false,
		nutrient_surplus: 1,
		mineral_surplus: 2,
		supported_units: 0,
		free_support: 1,
		base_labs: 4,
		available_energy: energy,
		needs_population_capacity: false,
		base_size: 1,
		get_orbital_marginal_yield: (def) => { return 0; },
	};
};

test.assert(production.choose(base, all_units, all_facilities, context(true, true, true, true, 10)).id == 'Defender');
test.assert(production.choose(base, all_units, all_facilities, context(false, true, true, true, 10)).id == 'Former');
test.assert(production.choose(base, all_units, all_facilities, context(false, false, true, true, 10)).id == 'Colony');
let sea_expansion_context = context(false, false, true, false, 10);
sea_expansion_context.needs_sea_colony = true;
test.assert(production.choose(base, [colony, sea_colony], [], sea_expansion_context).id == 'SeaColony');
sea_expansion_context.needs_sea_colony = false;
test.assert(production.choose(base, [colony, sea_colony], [], sea_expansion_context).id == 'Colony');
test.assert(production.choose(base, all_units, all_facilities, context(false, false, false, true, 10)).id == 'Recreation');
test.assert(production.choose(base, all_units, all_facilities, context(false, false, false, false, 10)).id == 'Recycling');

let headquarters_context = context(false, false, false, false, 10);
headquarters_context.needs_headquarters = false;
test.assert(production.score_facility(headquarters, headquarters_context) == null);
headquarters_context.needs_headquarters = true;
test.assert(production.score_facility(headquarters, headquarters_context) > 120000);

let probe_context = context(false, false, false, false, 10);
probe_context.needs_military = false;
probe_context.needs_probe = true;
test.assert(production.score_unit(probe_team, probe_context) != null);
probe_context.needs_probe = false;
test.assert(production.score_unit(probe_team, probe_context) == null);
probe_context.needs_probe = true;
probe_context.needs_garrison = true;
test.assert(production.score_unit(probe_team, probe_context) == null);

test.assert(production.score_project(command_nexus, context(false, true, false, false, 10)) == null);
test.assert(production.score_project(command_nexus, context(false, false, true, false, 10)) == null);
test.assert(production.score_project(command_nexus, context(true, false, false, false, 10)) == null);
let project_ready_context = context(false, false, false, false, 10);
project_ready_context.can_start_project = true;
test.assert(production.score_project(command_nexus, project_ready_context) != null);
test.assert(production.score_project(planetary_datalinks, project_ready_context) != null);
test.assert(production.score_project(empath_guild, project_ready_context) != null);
test.assert(production.score_project(pholus_mutagen, project_ready_context) != null);
test.assert(production.score_project(xenoempathy_dome, project_ready_context) != null);
test.assert(production.score_project(universal_translator, project_ready_context) != null);
test.assert(production.score_project(network_backbone, project_ready_context) != null);
test.assert(
	production.score_project(universal_translator, project_ready_context) >
	production.score_project(command_nexus, project_ready_context)
);
test.assert(
	production.score_project(pholus_mutagen, project_ready_context) >
	production.score_project(command_nexus, project_ready_context)
);
test.assert(
	production.score_project(xenoempathy_dome, project_ready_context) >
	production.score_project(command_nexus, project_ready_context)
);
let strong_backbone_context = context(false, false, false, false, 10);
strong_backbone_context.can_start_project = true;
strong_backbone_context.network_backbone_research_bonus = 6;
test.assert(
	production.score_project(network_backbone, strong_backbone_context) >
	production.score_project(network_backbone, project_ready_context)
);
let no_datalinks_candidates_context = context(false, false, false, false, 10);
no_datalinks_candidates_context.can_start_project = true;
project_ready_context.planetary_datalinks_technology_count = 2;
test.assert(
	production.score_project(planetary_datalinks, project_ready_context) >
	production.score_project(planetary_datalinks, no_datalinks_candidates_context)
);
let no_empath_targets_context = context(false, false, false, false, 10);
no_empath_targets_context.can_start_project = true;
project_ready_context.empath_guild_infiltration_count = 2;
test.assert(
	production.score_project(empath_guild, project_ready_context) >
	production.score_project(empath_guild, no_empath_targets_context)
);
locked.Recycling = true;
locked.Recreation = true;
test.assert(production.choose(base, all_units, all_facilities, context(false, false, false, false, 0)).id == 'Laser');
test.assert(production.choose(base, all_units, all_facilities, context(false, false, false, false, 1)).id == 'Network');
locked.Network = true;
test.assert(production.choose(base, all_units, all_facilities, context(false, false, false, false, 10)).id == 'Laser');

let deficit_context = context(false, false, false, false, 0 - 3);
deficit_context.needs_military = false;
test.assert(production.score_facility(recycling, deficit_context) != null);
test.assert(production.score_facility(network, deficit_context) == null);
test.assert(production.get_remaining_maintenance_budget(recycling, 0 - 3) == 0);
test.assert(production.get_remaining_maintenance_budget(network, 1) == 0);
test.assert(production.get_remaining_maintenance_budget(network, 0) == null);

const alpha = unit('Alpha', 1, 1, 1.0, 10, false, false);
const beta = unit('Beta', 1, 1, 1.0, 10, false, false);
test.assert(production.choose(base, [beta, alpha], [], context(false, false, false, false, 0)).id == 'Alpha');

let peaceful_context = context(false, false, false, false, 0);
peaceful_context.needs_military = false;
test.assert(production.choose(base, [laser], [], peaceful_context) == null);
test.assert(production.choose(base, [laser], [stockpile], peaceful_context).id == 'StockpileEnergy');
test.assert(production.score_stockpile(stockpile, peaceful_context) != null);
peaceful_context.needs_growth = true;
test.assert(production.score_stockpile(stockpile, peaceful_context) == null);

let orbital_context = context(false, false, false, false, 10);
orbital_context.needs_military = false;
orbital_context.get_orbital_marginal_yield = (def) => {
	return def.id == 'SkyHydroponicsLab' ? 3 : 0;
};
test.assert(production.score_orbital(sky_hydroponics, orbital_context) != null);
test.assert(production.score_orbital(orbital_defense, orbital_context) == null);
orbital_context.needs_orbital_defense = true;
orbital_context.orbital_defense_threats = 2;
orbital_context.priorities = {defense: 100};
test.assert(production.score_orbital(orbital_defense, orbital_context) != null);
orbital_context.needs_orbital_defense = false;
test.assert(
	production.choose(base, [laser], [stockpile, sky_hydroponics], orbital_context).id ==
	'SkyHydroponicsLab'
);
orbital_context.get_orbital_marginal_yield = (def) => { return 0; };
test.assert(production.score_orbital(sky_hydroponics, orbital_context) == null);
orbital_context.needs_garrison = true;
orbital_context.get_orbital_marginal_yield = (def) => { return 3; };
test.assert(production.score_orbital(sky_hydroponics, orbital_context) == null);

let planet_buster_context = context(false, false, false, false, 10);
planet_buster_context.needs_planet_buster = false;
test.assert(production.score_unit(planet_buster_unit, planet_buster_context) == null);
planet_buster_context.needs_planet_buster = true;
planet_buster_context.planet_buster_target_value = 8;
planet_buster_context.priorities = {military: 100};
test.assert(production.score_unit(planet_buster_unit, planet_buster_context) != null);
planet_buster_context.needs_garrison = true;
test.assert(production.score_unit(planet_buster_unit, planet_buster_context) == null);

let blocked_expansion_context = context(false, false, true, false, 0);
blocked_expansion_context.can_expand = false;
test.assert(production.score_unit(colony, blocked_expansion_context) == null);

let supported_context = context(false, false, false, false, 0);
supported_context.supported_units = 2;
supported_context.free_support = 1;
test.assert(
	production.score_unit(laser, supported_context) <
	production.score_unit(laser, context(false, false, false, false, 0))
);
const clean_laser = unit('CleanLaser', 2, 1, 1.0, 20, false, false);
clean_laser.abilities = ['CleanReactor'];
test.assert(
	production.score_unit(clean_laser, supported_context) ==
	production.score_unit(laser, supported_context) + 5000
);
const trained_laser = unit('TrainedLaser', 2, 1, 1.0, 25, false, false);
trained_laser.abilities = ['HighMorale'];
test.assert(
	production.score_unit(trained_laser, context(false, false, false, false, 0)) >
	production.score_unit(laser, context(false, false, false, false, 0))
);
const sam_laser = unit('SAMLaser', 2, 1, 1.0, 25, false, false);
sam_laser.abilities = ['AirSuperiority'];
let air_threat_context = context(false, false, false, false, 0);
air_threat_context.needs_air_superiority = true;
air_threat_context.hostile_air_unit_count = 3;
test.assert(
	production.score_unit(sam_laser, air_threat_context) >
	production.score_unit(laser, air_threat_context)
);
air_threat_context.needs_air_superiority = false;
test.assert(
	production.score_unit(sam_laser, air_threat_context) <
	production.score_unit(laser, air_threat_context)
);
const amphibious_laser = unit('AmphibiousLaser', 2, 1, 1.0, 25, false, false);
amphibious_laser.abilities = ['AmphibiousPods'];
let coastal_assault_context = context(false, false, false, false, 0);
coastal_assault_context.needs_amphibious = true;
coastal_assault_context.hostile_coastal_base_count = 2;
test.assert(
	production.score_unit(amphibious_laser, coastal_assault_context) >
	production.score_unit(laser, coastal_assault_context)
);
coastal_assault_context.needs_garrison = true;
test.assert(
	production.get_unit_ability_score(amphibious_laser, coastal_assault_context) == 0
);
const super_former = unit('SuperFormer', 0, 1, 1.0, 25, false, true);
super_former.abilities = ['SuperFormer'];
const fungicidal_former = unit('FungicidalFormer', 0, 1, 1.0, 25, false, true);
fungicidal_former.abilities = ['FungicideTanks'];
let former_context = context(false, true, false, false, 0);
test.assert(
	production.score_unit(super_former, former_context) >
	production.score_unit(former, former_context)
);
test.assert(
	production.score_unit(fungicidal_former, former_context) >
	production.score_unit(former, former_context)
);

let growth_context = context(false, false, false, false, 10);
growth_context.needs_growth = true;
test.assert(
	production.score_facility(recycling, growth_context) >
	production.score_facility(recycling, context(false, false, false, false, 10))
);
test.assert(
	production.score_facility(childrens_creche, growth_context) >
	production.score_facility(childrens_creche, context(false, false, false, false, 10))
);

let defense_context = context(false, false, false, false, 10);
defense_context.priorities = {defense: 100, development: 25};
test.assert(
	production.score_facility(perimeter, defense_context) >
	production.score_facility(recycling, defense_context)
);
let low_defense_context = context(false, false, false, false, 10);
low_defense_context.priorities = {defense: 0, development: 25};
test.assert(
	production.score_facility(perimeter, defense_context) >
	production.score_facility(perimeter, low_defense_context)
);
let economy_context = context(false, false, false, false, 10);
economy_context.priorities = {development: 100};
let low_economy_context = context(false, false, false, false, 10);
low_economy_context.priorities = {development: 0};
test.assert(
	production.score_facility(energy_bank, economy_context) >
	production.score_facility(energy_bank, low_economy_context)
);
test.assert(
	production.score_facility(childrens_creche, economy_context) >
	production.score_facility(childrens_creche, low_economy_context)
);
test.assert(
	production.score_facility(biology_lab, economy_context) >
	production.score_facility(biology_lab, low_economy_context)
);
let military_context = context(false, false, false, false, 10);
military_context.priorities = {military: 100, development: 25};
let low_military_context = context(false, false, false, false, 10);
low_military_context.priorities = {military: 0, development: 25};
test.assert(
	production.score_facility(command_center, military_context) >
	production.score_facility(command_center, low_military_context)
);
test.assert(
	production.score_facility(naval_yard, military_context) >
	production.score_facility(naval_yard, low_military_context)
);

locked = {};
let expansion_context = context(false, true, true, false, 10);
expansion_context.priorities = {
	expansion: 100,
	terraforming: 25,
	military: 25,
	growth: 0,
	psych: 0,
	development: 25,
};
test.assert(production.choose(base, all_units, all_facilities, expansion_context).id == 'Colony');

let terraforming_context = context(false, true, true, false, 10);
terraforming_context.priorities = {
	expansion: 25,
	terraforming: 100,
	military: 25,
	growth: 0,
	psych: 0,
	development: 25,
};
test.assert(production.choose(base, all_units, all_facilities, terraforming_context).id == 'Former');

let development_context = context(false, false, false, false, 10);
development_context.needs_military = true;
development_context.priorities = {
	expansion: 0,
	terraforming: 0,
	military: 25,
	growth: 0,
	psych: 0,
	development: 100,
};
test.assert(production.choose(base, all_units, all_facilities, development_context).id == 'Recycling');
development_context.priorities.military = 100;
development_context.priorities.development = 25;
test.assert(production.choose(base, all_units, all_facilities, development_context).id == 'Laser');

let infrastructure_context = context(false, false, true, false, 10);
infrastructure_context.needs_infrastructure = true;
infrastructure_context.priorities = {
	expansion: 100,
	terraforming: 0,
	military: 75,
	growth: 100,
	psych: 0,
	development: 25,
};
test.assert(production.choose(base, all_units, all_facilities, infrastructure_context).id == 'Recycling');
infrastructure_context.needs_garrison = true;
test.assert(production.choose(base, all_units, all_facilities, infrastructure_context).id == 'Defender');

const hurry_context = (kind, cost, credits, accumulated, mineral_surplus) => {
	let result = context(false, false, false, false, 10);
	result.kind = kind;
	result.hurry_cost = cost;
	result.energy_credits = credits;
	result.energy_income = 4;
	result.accumulated_minerals = accumulated;
	result.mineral_surplus = mineral_surplus;
	result.production_score = 40000;
	return result;
};

let emergency_hurry = hurry_context('unit', 40, 40, 0, 2);
emergency_hurry.needs_garrison = true;
test.assert(production.score_hurry(scout, emergency_hurry) != null);

let colony_hurry = hurry_context('unit', 53, 100, 10, 2);
colony_hurry.needs_colony = true;
const colony_hurry_score = production.score_hurry(colony, colony_hurry);
test.assert(colony_hurry_score != null);
test.assert(production.score_hurry(scout, emergency_hurry) > colony_hurry_score);
colony_hurry.energy_credits = 52;
test.assert(production.score_hurry(colony, colony_hurry) == null);
colony_hurry.energy_credits = 60;
test.assert(production.score_hurry(colony, colony_hurry) == null);

let premature_hurry = hurry_context('facility', 2, 100, 39, 2);
premature_hurry.needs_growth = true;
test.assert(production.score_hurry(recycling, premature_hurry) == null);

let early_hurry = hurry_context('facility', 120, 200, 0, 2);
early_hurry.needs_growth = true;
test.assert(production.score_hurry(recycling, early_hurry) == null);

let defense_hurry = hurry_context('facility', 20, 200, 20, 2);
defense_hurry.priorities = {defense: 100};
defense_hurry.production_score = production.score_facility(perimeter, defense_hurry);
test.assert(production.score_hurry(perimeter, defense_hurry) != null);

let economy_hurry = hurry_context('facility', 20, 200, 20, 2);
economy_hurry.priorities = {development: 100};
economy_hurry.production_score = production.score_facility(energy_bank, economy_hurry);
test.assert(production.score_hurry(energy_bank, economy_hurry) != null);
test.assert(production.score_hurry(biology_lab, economy_hurry) != null);

let military_hurry = hurry_context('facility', 20, 200, 20, 2);
military_hurry.priorities = {military: 100};
military_hurry.production_score = production.score_facility(command_center, military_hurry);
test.assert(production.score_hurry(command_center, military_hurry) != null);

let lower_id = {base: {id: 2}, score: 100};
let higher_id = {base: {id: 3}, score: 100};
test.assert(production.choose_hurry([higher_id, lower_id]) == lower_id);
