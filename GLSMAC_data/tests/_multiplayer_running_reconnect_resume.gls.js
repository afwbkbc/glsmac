#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let exit_scheduled = false;
	const initial_nutrient_stamp = 37;
	const initial_mineral_stamp = 23;
	const initial_energy_stamp = 137;
	const loan_balance_stamp = 91;
	const loan_payment_stamp = 7;
	const sanction_turns_stamp = 3;
	const integrity_blemishes_stamp = 4;
	const sky_hydroponics_stamp = 3;
	const orbital_defense_pods_stamp = 2;
	const orbital_defense_deployments_stamp = 1;
	const prototyped_components_stamp = [
		'ColonyModule', 'HandWeapons', 'Infantry', 'Laser', 'NoArmor', 'Speeder',
	];
	// The Believers' +2 SUPPORT rating covers the two snapshot units for free.
	const processed_turn_unit_support = 0;
	const defeated_snapshot_unit_id = 3;
	const expansion_snapshot_unit_id = 4;
	const former_snapshot_unit_id = 5;
	const air_snapshot_unit_id = 6;
	const conquered_snapshot_base_name = 'Reconnect Conquest Probe';
	const expansion_snapshot_base_name = 'Reconnect Expansion Probe';
	const terraform_order = 'forest';

	glsmac.on('configure_game', (e) => {
		const game = e.game;
		let handled_turns = {};

		const get_snapshot_production_ids = (base) => {
			return base.get_tile().is_water
				? ['SeaLurk', 'SeaLurk']
				: ['SporeLauncher', 'MindWorms'];
		};

		const find_base_for_player = (player_id) => {
			for (base of game.get_bm().get_bases()) {
				if (base.get_owner().id == player_id) {
					return base;
				}
			}
			return null;
		};

		const get_research_state_error = (player) => {
			const starting_technologies = player.get_faction().get_starting_technologies();
			let starts_with_ecology = false;
			for (id of starting_technologies) {
				if (id == 'CentauriEcology') {
					starts_with_ecology = true;
				}
			}
			const state = player.get_research_state();
			if (state.technologies != starting_technologies) {
				return 'faction starting technologies were not restored';
			}
			if (starts_with_ecology) {
				if (
					!player.has_technology('CentauriEcology') ||
					state.target != 'Biogenetics' ||
					state.progress <= 0
				) {
					return 'starting Centauri Ecology progression was not restored';
				}
				return #undefined;
			}
			const base = find_base_for_player(player.id);
			if (
				player.has_technology('CentauriEcology') ||
				state.target != 'Biogenetics' ||
				state.progress <= 0 ||
				base == null ||
				base.can_set_production('unit', 'Former')
			) {
				return 'Centauri Ecology progress or Former production gate was not restored';
			}
			return #undefined;
		};

		const get_terraform_state_error = (turns_remaining, moved_this_turn) => {
			if (!game.get_um().has_unit(former_snapshot_unit_id)) {
				return 'Former was not restored from the snapshot';
			}
			const former = game.get_um().get_unit(former_snapshot_unit_id);
			const def = former.get_def();
			if (
				former.owner != game.get_player().id ||
				def.id != 'Former' ||
				def.required_technology != 'CentauriEcology' ||
				def.is_native ||
				def.offense != 0 ||
				def.defense != 1 ||
				def.morale_set != 'STANDARD' ||
				def.can_found_base ||
				!def.can_terraform
			) {
				return 'Former definition or owner was not restored';
			}
			const tile = former.get_tile();
			if (
				!tile.is_land ||
				tile.get_base() != null ||
				tile.features.monolith ||
				tile.features.xenofungus ||
				tile.terraforming[terraform_order]
			) {
				return 'Former tile state was not restored';
			}
			if (
				former.terraforming != terraform_order ||
				former.terraforming_turns_remaining != turns_remaining ||
				former.movement != 0.0 ||
				former.moved_this_turn != moved_this_turn
			) {
				return 'Former order state was not restored';
			}
			return #undefined;
		};

		const get_base_state_error = () => {
			if (game.get_player().energy_credits != initial_energy_stamp) {
				return
					'energy credits are ' + #to_string(game.get_player().energy_credits) +
					', expected ' + #to_string(initial_energy_stamp);
			}
			let lender = null;
			for (player of game.get_players()) {
				if (player.id != game.get_player().id) {
					lender = player;
					break;
				}
			}
			const loan = lender == null ? null : game.get_player().get_diplomatic_loan(lender);
			if (
				loan == null ||
				loan.balance != loan_balance_stamp ||
				loan.payment != loan_payment_stamp
			) {
				return 'active diplomatic loan was not restored';
			}
			if (game.get_player().get_sanction_turns() != sanction_turns_stamp) {
				return 'economic sanction duration was not restored';
			}
			if (game.get_player().get_integrity_blemishes() != integrity_blemishes_stamp) {
				return 'diplomatic integrity was not restored';
			}
			if (game.get_player().get_prototyped_components() != prototyped_components_stamp) {
				return 'prototyped unit components were not restored';
			}
			if (
				game.get_player().get_orbital_facility_count('SkyHydroponicsLab') !=
				sky_hydroponics_stamp
			) {
				return 'orbital facility counts were not restored';
			}
			if (
				game.get_player().get_orbital_facility_count('OrbitalDefensePod') !=
					orbital_defense_pods_stamp ||
				game.get_player().get_orbital_defense_deployments() !=
					orbital_defense_deployments_stamp
			) {
				return 'orbital defense deployment state was not restored';
			}
			if (game.get_um().has_unit(defeated_snapshot_unit_id)) {
				return 'defeated unit was restored from the snapshot';
			}
			if (game.get_um().has_unit(expansion_snapshot_unit_id)) {
				return 'consumed Colony Pod was restored from the snapshot';
			}
			if (!game.get_um().has_unit(air_snapshot_unit_id)) {
				return 'partially fueled Needlejet was not restored from the snapshot';
			}
			const air_unit = game.get_um().get_unit(air_snapshot_unit_id);
			const air_def = air_unit.get_def();
			if (
				air_unit.owner != game.get_player().id || air_unit.fuel != 1 ||
				air_def.chassis != 'Needlejet' || air_def.operational_range != 2 ||
				air_def.is_missile || !air_def.is_air
			) {
				return 'Needlejet fuel or definition metadata was not restored';
			}
			const restored_unit = game.get_um().get_unit(1);
			if (restored_unit.get_def().id != restored_unit.def) {
				return 'unit definition link is inconsistent';
			}
			const terraform_state_error = get_terraform_state_error(4, true);
			if (#is_defined(terraform_state_error)) {
				return terraform_state_error;
			}
			const colony_pod_def = game.get_um().get_unit_def('ColonyPod');
			if (
				colony_pod_def.is_native ||
				colony_pod_def.offense != 0 ||
				colony_pod_def.defense != 1 ||
				colony_pod_def.morale_set != 'STANDARD' ||
				!colony_pod_def.can_found_base ||
				colony_pod_def.can_terraform
			) {
				return 'Colony Pod definition metadata was not restored';
			}
			let base = null;
			for (candidate of game.get_bm().get_bases()) {
				if (candidate.get_owner().id == game.get_player().id) {
					base = candidate;
					break;
				}
			}
			if (base == null) {
				return 'base is missing';
			}
			if (base.get('network_node_artifact_linked') != true) {
				return 'Network Node artifact state was not restored';
			}
			if (game.get_um().get_unit(former_snapshot_unit_id).home_base_id != base.id) {
				return 'Former home base was not restored from the snapshot';
			}
			const expected_production_ids = get_snapshot_production_ids(base);
			const production = base.get_production();
			const production_queue = base.get_production_queue();
			if (!#is_defined(production)) {
				return 'production target is missing';
			}
			if (
				production.production_kind != 'unit' ||
				production.id != expected_production_ids[0] ||
				production.mineral_cost <= 0 ||
				#sizeof(production_queue) != 2
			) {
				return 'production target is invalid';
			}
			for (let i = 0; i < #sizeof(production_queue); i++) {
				if (
					production_queue[i].production_kind != 'unit' ||
					production_queue[i].id != expected_production_ids[i]
				) {
					return 'production queue entry ' + #to_string(i) + ' is invalid';
				}
			}
			const facilities = base.get_facilities();
			if (
				!base.has_facility('Headquarters') ||
				!base.has_facility('RecyclingTanks') ||
				#sizeof(facilities) != 2
			) {
				return 'starting Headquarters or built Recycling Tanks state is missing';
			}
			const recycling_tanks = game.get_bm().get_facility_def('RecyclingTanks');
			const headquarters = game.get_bm().get_facility_def('Headquarters');
			const recreation_commons = game.get_bm().get_facility_def('RecreationCommons');
			const perimeter_defense = game.get_bm().get_facility_def('PerimeterDefense');
			const energy_bank = game.get_bm().get_facility_def('EnergyBank');
			const command_center = game.get_bm().get_facility_def('CommandCenter');
			const childrens_creche = game.get_bm().get_facility_def('ChildrenSCreche');
			const naval_yard = game.get_bm().get_facility_def('NavalYard');
			const aerospace_complex = game.get_bm().get_facility_def('AerospaceComplex');
			const biology_lab = game.get_bm().get_facility_def('BiologyLab');
			const skunkworks = game.get_bm().get_facility_def('Skunkworks');
			const stockpile_energy = game.get_bm().get_facility_def('StockpileEnergy');
			const sky_hydroponics = game.get_bm().get_facility_def('SkyHydroponicsLab');
			const nessus_mining = game.get_bm().get_facility_def('NessusMiningStation');
			const orbital_power = game.get_bm().get_facility_def('OrbitalPowerTransmitter');
			const orbital_defense = game.get_bm().get_facility_def('OrbitalDefensePod');
			const hologram_theatre = game.get_bm().get_facility_def('HologramTheatre');
			const research_hospital = game.get_bm().get_facility_def('ResearchHospital');
			const robotic_assembly = game.get_bm().get_facility_def('RoboticAssemblyPlant');
			const hab_complex = game.get_bm().get_facility_def('HabComplex');
			const habitation_dome = game.get_bm().get_facility_def('HabitationDome');
			const paradise_garden = game.get_bm().get_facility_def('ParadiseGarden');
			const genejack_factory = game.get_bm().get_facility_def('GenejackFactory');
			const punishment_sphere = game.get_bm().get_facility_def('PunishmentSphere');
			const centauri_preserve = game.get_bm().get_facility_def('CentauriPreserve');
			const temple_of_planet = game.get_bm().get_facility_def('TempleOfPlanet');
			const tree_farm = game.get_bm().get_facility_def('TreeFarm');
			const hybrid_forest = game.get_bm().get_facility_def('HybridForest');
			const voice = game.get_bm().get_facility_def('TheVoiceOfPlanet');
			const ascent = game.get_bm().get_facility_def('TheAscentToTranscendence');
			const ascetic_virtues = game.get_bm().get_facility_def('TheAsceticVirtues');
			const self_aware_colony = game.get_bm().get_facility_def('TheSelfAwareColony');
			if (
				recycling_tanks.id != 'RecyclingTanks' ||
				recycling_tanks.production_kind != 'facility' ||
				recycling_tanks.mineral_cost != 40 ||
				recycling_tanks.required_technology != 'Biogenetics' ||
				recycling_tanks.nutrient_bonus != 1 ||
				recycling_tanks.mineral_bonus != 1 ||
				recycling_tanks.energy_bonus != 1 ||
				recycling_tanks.psych_bonus != 0 ||
				recycling_tanks.defense_multiplier != 1.0 ||
				recycling_tanks.economy_multiplier != 0.0 ||
				recycling_tanks.unit_morale_bonus != 0 ||
				recycling_tanks.research_bonus != 0 ||
				recycling_tanks.mineral_multiplier != 0.0 ||
				recycling_tanks.psych_multiplier != 0.0 ||
				recycling_tanks.population_limit != 0 ||
				recycling_tanks.required_facility != '' ||
				recycling_tanks.required_project != '' ||
				recycling_tanks.drone_modifier != 0 ||
				recycling_tanks.talent_bonus != 0 ||
				recycling_tanks.suppress_psych ||
				recycling_tanks.unit_morale_land_bonus != 0 ||
				recycling_tanks.unit_morale_water_bonus != 0 ||
				recycling_tanks.unit_morale_air_bonus != 0 ||
				recycling_tanks.water_defense_multiplier != 1.0 ||
				recycling_tanks.air_defense_multiplier != 1.0 ||
				recycling_tanks.growth_rating_bonus != 0 ||
				recycling_tanks.native_lifecycle_bonus != 0 ||
				recycling_tanks.defender_morale_bonus != 0 ||
				headquarters.defender_morale_bonus != 0 ||
				ascetic_virtues.global_police_rating_bonus != 1 ||
				self_aware_colony.global_extra_police_units != 1 ||
				recreation_commons.psych_bonus != 0 ||
				recreation_commons.drone_modifier != -2 ||
				recreation_commons.required_technology != 'SocialPsych' ||
				perimeter_defense.required_technology != 'DoctrineLoyalty' ||
				perimeter_defense.defense_multiplier != 2.0 ||
				energy_bank.required_technology != 'IndustrialEconomics' ||
				energy_bank.economy_multiplier != 0.5 ||
				command_center.required_technology != 'DoctrineMobility' ||
				command_center.unit_morale_bonus != 0 ||
				command_center.unit_morale_land_bonus != 2 ||
				!command_center.full_repair_land ||
				childrens_creche.growth_rating_bonus != 2 ||
				childrens_creche.efficiency_rating_bonus != 2 ||
				childrens_creche.defender_morale_minimum != 1 ||
				naval_yard.unit_morale_water_bonus != 2 ||
				naval_yard.water_defense_multiplier != 2.0 ||
				!naval_yard.full_repair_water ||
				aerospace_complex.unit_morale_air_bonus != 2 ||
				aerospace_complex.air_defense_multiplier != 2.0 ||
				!aerospace_complex.full_repair_air ||
				biology_lab.required_technology != 'CentauriEmpathy' ||
				biology_lab.research_bonus != 2 ||
				biology_lab.native_lifecycle_bonus != 1 ||
				!biology_lab.full_repair_native ||
				!skunkworks.prototype_cost_waiver ||
				stockpile_energy.mineral_cost != 0 ||
				stockpile_energy.mineral_to_energy_divisor != 2 ||
				sky_hydroponics.orbital_resource != 'NUTRIENTS' ||
				nessus_mining.orbital_resource != 'MINERALS' ||
				orbital_power.orbital_resource != 'ENERGY' ||
				!orbital_defense.orbital_defense ||
				sky_hydroponics.required_facility != 'AerospaceComplex' ||
				hologram_theatre.psych_multiplier != 0.5 ||
				research_hospital.research_multiplier != 0.5 ||
				research_hospital.psych_multiplier != 0.25 ||
				robotic_assembly.mineral_multiplier != 0.5 ||
				hab_complex.population_limit != 14 ||
				habitation_dome.population_limit != 1000000 ||
				habitation_dome.required_facility != 'HabComplex' ||
				paradise_garden.talent_bonus != 2 ||
				genejack_factory.mineral_multiplier != 0.5 ||
				genejack_factory.drone_modifier != 1 ||
				punishment_sphere.research_multiplier != -0.5 ||
				!punishment_sphere.suppress_psych ||
				centauri_preserve.native_lifecycle_bonus != 1 ||
				temple_of_planet.native_lifecycle_bonus != 1 ||
				tree_farm.forest_nutrient_bonus != 1 ||
				tree_farm.forest_mineral_bonus != 0 ||
				tree_farm.forest_energy_bonus != 0 ||
				hybrid_forest.forest_nutrient_bonus != 1 ||
				hybrid_forest.forest_mineral_bonus != 0 ||
				hybrid_forest.forest_energy_bonus != 1 ||
				voice.global_native_lifecycle_bonus != 1 ||
				ascent.required_project != 'TheVoiceOfPlanet'
			) {
				return 'restored facility definition is invalid';
			}
			const expected_snapshot_minerals =
				initial_mineral_stamp +
					base.get_tile().get_resources(base.get_owner()).MINERALS +
					recycling_tanks.mineral_bonus -
					processed_turn_unit_support;
			if (base.get_accumulated_minerals() != expected_snapshot_minerals) {
				return
					'accumulated minerals are ' + #to_string(base.get_accumulated_minerals()) +
					', expected ' + #to_string(expected_snapshot_minerals);
			}
			let conquered_base = null;
			for (candidate of game.get_bm().get_bases()) {
				if (candidate.name == conquered_snapshot_base_name) {
					conquered_base = candidate;
					break;
				}
			}
			if (conquered_base == null) {
				return 'conquered base is missing';
			}
			if (conquered_base.get_owner().id != game.get_player().id) {
				return 'conquered base owner was not restored';
			}
			let expansion_base = null;
			for (candidate of game.get_bm().get_bases()) {
				if (candidate.name == expansion_snapshot_base_name) {
					expansion_base = candidate;
					break;
				}
			}
			if (expansion_base == null) {
				return 'founded expansion base is missing';
			}
			const expansion_production = expansion_base.get_production();
			if (
				expansion_base.get_owner().id != game.get_player().id ||
				#sizeof(expansion_base.get_pops()) != 1 ||
				#sizeof(expansion_base.get_worked_tiles()) != 1 ||
				!#is_defined(expansion_production) ||
				expansion_production.id != 'ScoutPatrol'
			) {
				return 'founded expansion base state was not restored';
			}
			const accumulated_nutrients = base.get('accumulated_nutrients');
			if (!#is_defined(accumulated_nutrients)) {
				return 'accumulated nutrients are missing';
			}
			const expected_snapshot_nutrients =
				initial_nutrient_stamp +
					base.get_tile().get_resources(base.get_owner()).NUTRIENTS -
					game.get('map_growth_base') +
					recycling_tanks.nutrient_bonus;
			if (accumulated_nutrients != expected_snapshot_nutrients) {
				return
					'accumulated nutrients are ' + #to_string(accumulated_nutrients) +
					', expected ' + #to_string(expected_snapshot_nutrients);
			}
			const pops = base.get_pops();
			const worked_tiles = base.get_worked_tiles();
			if (#sizeof(pops) != 1) {
				return 'population count is ' + #to_string(#sizeof(pops));
			}
			if (#sizeof(worked_tiles) != 1) {
				return 'worked tile count is ' + #to_string(#sizeof(worked_tiles));
			}
			const pop = pops[0];
			if (!pop.has('worked_tile')) {
				return 'population has no worked tile';
			}
			const tile = pop.get('worked_tile');
			if (!base.is_tile_worked(tile)) {
				return 'population tile is absent from the base worked set';
			}
			if (!tile.has('working_pop')) {
				return 'worked tile has no population link';
			}
			if (tile.get('working_pop') != pop) {
				return 'worked tile links to a different population';
			}
			return #undefined;
		};

		let handle_turn = (turn_id) => {
			const turn_key = #to_string(turn_id);
			if (#is_defined(handled_turns[turn_key])) {
				return;
			}
			handled_turns[turn_key] = true;
			if (
				#sizeof(game.get_players()) != 2 ||
				#sizeof(game.get_bm().get_bases()) < 2 ||
				!game.get_um().has_unit(1)
			) {
				#print('RUNNING_RECONNECT_FAIL_CLIENT: restored state is incomplete');
				glsmac.exit();
				return;
			}

			if (turn_id == 1) {
				for (player of game.get_players()) {
					const research_error = get_research_state_error(player);
					if (#is_defined(research_error)) {
						#print('RUNNING_RECONNECT_FAIL_CLIENT: ' + research_error);
						glsmac.exit();
						return;
					}
				}
				const base_state_error = get_base_state_error();
				if (#is_defined(base_state_error)) {
					#print('RUNNING_RECONNECT_FAIL_CLIENT: ' + base_state_error);
					glsmac.exit();
					return;
				}
				#print('RUNNING_RECONNECT_BASE_STATE_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_CONQUERED_BASE_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_EXPANSION_BASE_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_UNIT_DEF_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_PRODUCTION_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_TERRAFORM_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_RESEARCH_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_ENERGY_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_LOAN_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_SANCTIONS_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_INTEGRITY_RESUMED_CLIENT');
				#print('RUNNING_RECONNECT_RESUMED_CLIENT');
				game.event('complete_turn', {});
			}
			else if (turn_id == 2 && !exit_scheduled) {
				const terraform_state_error = get_terraform_state_error(3, false);
				if (#is_defined(terraform_state_error)) {
					#print('RUNNING_RECONNECT_FAIL_CLIENT: ' + terraform_state_error);
					glsmac.exit();
					return;
				}
				exit_scheduled = true;
				#print('RUNNING_RECONNECT_TERRAFORM_ADVANCED_CLIENT');
				#print('RUNNING_RECONNECT_PASS_CLIENT');
				#async(3000, () => {
					glsmac.exit();
				});
			}
		};

		game.on('start_ui', (e) => {
			if (game.get_turn() > 0) {
				handle_turn(game.get_turn());
			}
		});

		game.on('turn', (e) => {
			handle_turn(e.year - 2100);
		});
	});

	glsmac.run();

});
