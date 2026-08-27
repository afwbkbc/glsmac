#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let map_size_requested = false;
	let ready_requested = false;
	let ready_retry_ticks = 0;
	let game_configured = false;
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
	const defeated_snapshot_unit_id = 3;
	const expansion_snapshot_unit_id = 4;
	const former_snapshot_unit_id = 5;
	const air_snapshot_unit_id = 6;
	const conquered_snapshot_base_name = 'Reconnect Conquest Probe';
	const expansion_snapshot_base_name = 'Reconnect Expansion Probe';
	const terraform_order = 'forest';

	glsmac.on('configure_state', (e) => {
		#async(100, () => {
			const game = glsmac.game;
			if (#sizeof(game.get_players()) != 2) {
				return true;
			}

			const map = game.get_settings().global.map;
			if (game.is_master() && !map_size_requested) {
				map_size_requested = true;
				game.event('game_settings', {
					changes: [
						['planet_size', '20x10'],
						['native_lifeforms', 0.0],
					],
				});
			}
			if (map.size_x != 20 || map.size_y != 10 || map.native_lifeforms != 0.0) {
				return true;
			}

			const me = game.get_player();
			if (!me.is_ready()) {
				ready_retry_ticks++;
				if (!ready_requested || ready_retry_ticks >= 10) {
					ready_requested = true;
					ready_retry_ticks = 0;
					game.event('ready_or_not', {
						ready: true,
					});
				}
			}
			else {
				ready_requested = false;
				ready_retry_ticks = 0;
			}
			return !game_configured;
		});
	});

	glsmac.on('configure_game', (e) => {
		game_configured = true;
		const game = e.game;
		const role = game.is_master() ? 'HOST' : 'CLIENT';
		let handled_turns = {};
		let terraform_site_coords = null;

		game.register_event('running_reconnect_set_energy', {
			validate: (e) => {
				if (#typeof(e.data.energy_credits) != 'Int' || e.data.energy_credits < 0) {
					return 'Invalid reconnect energy stamp';
				}
			},
			apply: (e) => {
				const player = e.game.get_player(e.caller);
				const previous = player.energy_credits;
				player.set_energy_credits(e.data.energy_credits);
				return {energy_credits: previous};
			},
			rollback: (e) => {
				e.game.get_player(e.caller).set_energy_credits(e.applied.energy_credits);
			},
		});

		game.register_event('running_reconnect_set_loan', {
			validate: (e) => {
				if (e.data.lender.id == e.caller) {
					return 'Reconnect loan lender must be another player';
				}
			},
			apply: (e) => {
				const borrower = e.game.get_player(e.caller);
				const previous = {
					loan: borrower.get_diplomatic_loan(e.data.lender),
					sanction_turns: borrower.get_sanction_turns(),
					integrity_blemishes: borrower.get_integrity_blemishes(),
					prototyped_components: borrower.get_prototyped_components(),
					sky_hydroponics: borrower.get_orbital_facility_count('SkyHydroponicsLab'),
					orbital_defense_pods: borrower.get_orbital_facility_count('OrbitalDefensePod'),
					orbital_defense_deployments: borrower.get_orbital_defense_deployments(),
				};
				borrower.set_diplomatic_loan(e.data.lender, {
					balance: loan_balance_stamp,
					payment: loan_payment_stamp,
				});
				borrower.set_sanction_turns(sanction_turns_stamp);
				borrower.set_integrity_blemishes(integrity_blemishes_stamp);
				borrower.set_prototyped_components(prototyped_components_stamp);
				borrower.set_orbital_facility_count('SkyHydroponicsLab', sky_hydroponics_stamp);
				borrower.set_orbital_facility_count('OrbitalDefensePod', orbital_defense_pods_stamp);
				borrower.set_orbital_defense_deployments(orbital_defense_deployments_stamp);
				return previous;
			},
			rollback: (e) => {
				const borrower = e.game.get_player(e.caller);
				if (e.applied.loan == null) {
					borrower.clear_diplomatic_loan(e.data.lender);
				} else {
					borrower.set_diplomatic_loan(e.data.lender, e.applied.loan);
				}
				borrower.set_sanction_turns(e.applied.sanction_turns);
				borrower.set_integrity_blemishes(e.applied.integrity_blemishes);
				borrower.set_prototyped_components(e.applied.prototyped_components);
				borrower.set_orbital_facility_count(
					'SkyHydroponicsLab',
					e.applied.sky_hydroponics
				);
				borrower.set_orbital_facility_count(
					'OrbitalDefensePod',
					e.applied.orbital_defense_pods
				);
				borrower.set_orbital_defense_deployments(
					e.applied.orbital_defense_deployments
				);
			},
		});

		const find_base_for_player = (player_id) => {
			for (base of game.get_bm().get_bases()) {
				if (base.get_owner().id == player_id) {
					return base;
				}
			}
			return null;
		};

		const find_base_by_name = (name) => {
			for (base of game.get_bm().get_bases()) {
				if (base.name == name) {
					return base;
				}
			}
			return null;
		};

		const find_unit_def_by_chassis = (chassis) => {
			for (def of game.get_um().get_unit_defs()) {
				if (def.chassis == chassis && def.offense > 0) {
					return def;
				}
			}
			return null;
		};

		const get_remote_player_id = () => {
			const local_player_id = game.get_player().id;
			for (player of game.get_players()) {
				if (player.id != local_player_id) {
					return player.id;
				}
			}
			return null;
		};

		const get_research_state_error = (player, expect_progress) => {
			const starting_technologies = player.get_faction().get_starting_technologies();
			let starts_with_ecology = false;
			for (id of starting_technologies) {
				if (id == 'CentauriEcology') {
					starts_with_ecology = true;
				}
			}
			const state = player.get_research_state();
			if (state.technologies != starting_technologies) {
				return 'faction starting technologies are invalid';
			}
			if (starts_with_ecology) {
				if (
					!player.has_technology('CentauriEcology') ||
					state.target != 'Biogenetics' ||
					(expect_progress ? state.progress <= 0 : state.progress != 0)
				) {
					return 'starting Centauri Ecology progression is invalid';
				}
				return #undefined;
			}
			const base = find_base_for_player(player.id);
			if (
				player.has_technology('CentauriEcology') ||
				state.target != 'Biogenetics' ||
				(expect_progress ? state.progress <= 0 : state.progress != 0) ||
				base == null ||
				base.can_set_production('unit', 'Former')
			) {
				return 'Centauri Ecology progress or Former production gate is invalid';
			}
			return #undefined;
		};

		const find_founding_site_coords = () => {
			const tm = game.get_tm();
			let result = null;
			for (let y = 0; y < tm.get_map_height(); y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const tile = tm.get_tile(x, y);
					if (
						!tile.is_land ||
						tile.is_locked() ||
						tile.get_base() != null ||
						#sizeof(tile.get_units()) != 0
					) {
						continue;
					}
					if (
						terraform_site_coords == null &&
						!tile.features.monolith &&
						!tile.features.xenofungus
					) {
						terraform_site_coords = {x: x, y: y};
						continue;
					}
					let is_adjacent_to_base = false;
					for (nearby of tile.get_surrounding_tiles()) {
						if (nearby.get_base() != null) {
							is_adjacent_to_base = true;
							break;
						}
					}
					if (!is_adjacent_to_base) {
						result = {x: x, y: y};
						break;
					}
				}
				if (result != null) {
					break;
				}
			}
			return result;
		};

		const find_local_colony_pod = () => {
			const tm = game.get_tm();
			let result_id = 0;
			for (let y = 0; y < tm.get_map_height(); y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					for (unit of tm.get_tile(x, y).get_units()) {
						if (unit.owner == game.get_player().id && unit.def == 'ColonyPod') {
							result_id = unit.id;
							break;
						}
					}
					if (result_id != 0) {
						break;
					}
				}
				if (result_id != 0) {
					break;
				}
			}
			return result_id == 0 ? null : game.get_um().get_unit(result_id);
		};

		const get_terraform_state_error = (owner_id, turns_remaining, moved_this_turn) => {
			if (!game.get_um().has_unit(former_snapshot_unit_id)) {
				return 'Former is missing';
			}
			const former = game.get_um().get_unit(former_snapshot_unit_id);
			const def = former.get_def();
			if (
				def.id != 'Former' ||
				def.required_technology != 'CentauriEcology' ||
				def.is_native ||
				def.offense != 0 ||
				def.defense != 1 ||
				def.morale_set != 'STANDARD' ||
				def.can_found_base ||
				!def.can_terraform
			) {
				return 'Former definition metadata is invalid';
			}
			const tile = former.get_tile();
			if (
				former.owner != owner_id ||
				!tile.is_land ||
				tile.get_base() != null ||
				tile.features.monolith ||
				tile.features.xenofungus ||
				tile.terraforming[terraform_order]
			) {
				return 'Former owner or tile state is invalid';
			}
			if (
				former.terraforming != terraform_order ||
				former.terraforming_turns_remaining != turns_remaining ||
				former.movement != 0.0 ||
				former.moved_this_turn != moved_this_turn
			) {
				return 'Former order state is invalid';
			}
			return #undefined;
		};

		const run_initial_founding_probe = () => {
			let founding_requested = false;
			let terraform_requested = false;
			let energy_requested = false;
			let loan_requested = false;
			let colony_pod_id = 0;
			let wait_ticks = 0;
			#async(100, () => {
				wait_ticks++;
				if (!terraform_requested && game.get_um().has_unit(former_snapshot_unit_id)) {
					const former = game.get_um().get_unit(former_snapshot_unit_id);
					const def = former.get_def();
					if (
						former.owner != game.get_player().id ||
						def.id != 'Former' ||
						def.is_native ||
						def.offense != 0 ||
						def.defense != 1 ||
						def.morale_set != 'STANDARD' ||
						def.can_found_base ||
						!def.can_terraform
					) {
						#print('RUNNING_RECONNECT_FAIL_CLIENT: Former definition is invalid');
						glsmac.exit();
						return false;
					}
					terraform_requested = true;
					game.event('terraform_tile', {
						unit: former,
						type: terraform_order,
					});
				}
				if (!founding_requested) {
					const colony_pod = find_local_colony_pod();
					if (colony_pod != null) {
						if (colony_pod.id != expansion_snapshot_unit_id) {
							#print('RUNNING_RECONNECT_FAIL_CLIENT: unexpected Colony Pod id');
							glsmac.exit();
							return false;
						}
						const def = colony_pod.get_def();
						if (
							def.is_native ||
							def.offense != 0 ||
							def.defense != 1 ||
							def.morale_set != 'STANDARD' ||
							!def.can_found_base
						) {
							#print('RUNNING_RECONNECT_FAIL_CLIENT: Colony Pod definition is invalid');
							glsmac.exit();
							return false;
						}
						colony_pod_id = colony_pod.id;
						founding_requested = true;
						game.event('found_base', {
							unit: colony_pod,
							name: expansion_snapshot_base_name,
						});
					}
				}
				if (founding_requested && terraform_requested) {
					const expansion_base = find_base_by_name(expansion_snapshot_base_name);
					const former = game.get_um().has_unit(former_snapshot_unit_id)
						? game.get_um().get_unit(former_snapshot_unit_id)
						: null;
					if (expansion_base != null && former != null && former.terraforming == terraform_order) {
						const production = expansion_base.get_production();
						if (
							game.get_um().has_unit(colony_pod_id) ||
							expansion_base.get_owner().id != game.get_player().id ||
							#sizeof(expansion_base.get_pops()) != 1 ||
							#sizeof(expansion_base.get_worked_tiles()) != 1 ||
							!#is_defined(production) ||
							production.id != 'ScoutPatrol'
						) {
							#print('RUNNING_RECONNECT_FAIL_CLIENT: founded base state is invalid');
							glsmac.exit();
							return false;
						}
						const terraform_state_error = get_terraform_state_error(
							game.get_player().id,
							4,
							true
						);
						if (#is_defined(terraform_state_error)) {
							#print('RUNNING_RECONNECT_FAIL_CLIENT: ' + terraform_state_error);
							glsmac.exit();
							return false;
						}
						if (game.get_player().energy_credits != initial_energy_stamp) {
							if (!energy_requested) {
								energy_requested = true;
								game.event('running_reconnect_set_energy', {
									energy_credits: initial_energy_stamp,
								});
							}
							return true;
						}
						const lender = game.get_player(get_remote_player_id());
						const loan = game.get_player().get_diplomatic_loan(lender);
						if (loan == null) {
							if (!loan_requested) {
								loan_requested = true;
								game.event('running_reconnect_set_loan', {lender: lender});
							}
							return true;
						}
						if (
							loan.balance != loan_balance_stamp || loan.payment != loan_payment_stamp ||
							game.get_player().get_sanction_turns() != sanction_turns_stamp ||
							game.get_player().get_integrity_blemishes() != integrity_blemishes_stamp ||
							game.get_player().get_prototyped_components() != prototyped_components_stamp ||
							game.get_player().get_orbital_facility_count('SkyHydroponicsLab') !=
								sky_hydroponics_stamp ||
							game.get_player().get_orbital_facility_count('OrbitalDefensePod') !=
								orbital_defense_pods_stamp ||
							game.get_player().get_orbital_defense_deployments() !=
								orbital_defense_deployments_stamp
						) {
							#print('RUNNING_RECONNECT_FAIL_CLIENT: initial diplomatic stamp is invalid');
							glsmac.exit();
							return false;
						}
						#print('RUNNING_RECONNECT_BASE_FOUNDING_INITIAL_CLIENT');
						#print('RUNNING_RECONNECT_LOAN_INITIAL_CLIENT');
						#print('RUNNING_RECONNECT_SANCTIONS_INITIAL_CLIENT');
						#print('RUNNING_RECONNECT_INTEGRITY_INITIAL_CLIENT');
						#print('RUNNING_RECONNECT_TERRAFORM_INITIAL_CLIENT');
						#print('RUNNING_RECONNECT_DROP_READY');
						return false;
					}
				}
				if (wait_ticks >= 200) {
					#print('RUNNING_RECONNECT_FAIL_CLIENT: colony founding timed out');
					glsmac.exit();
					return false;
				}
				return true;
			});
		};

		const get_snapshot_production_ids = (base) => {
			return base.get_tile().is_water
				? ['SeaLurk', 'SeaLurk']
				: ['SporeLauncher', 'ScoutPatrol'];
		};

		const get_production_state_error = (base) => {
			const expected_ids = get_snapshot_production_ids(base);
			const production = base.get_production();
			const queue = base.get_production_queue();
			if (!#is_defined(production)) {
				return 'production target is missing';
			}
			if (
				production.production_kind != 'unit' ||
				production.id != expected_ids[0] ||
				production.mineral_cost <= 0 ||
				#sizeof(queue) != 2
			) {
				return 'production target or queue size is invalid';
			}
			for (let i = 0; i < #sizeof(queue); i++) {
				if (queue[i].production_kind != 'unit' || queue[i].id != expected_ids[i]) {
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
				headquarters.energy_bonus != 1 ||
				headquarters.defender_morale_bonus != 0 ||
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
				childrens_creche.growth_rating_bonus != 2 ||
				childrens_creche.efficiency_rating_bonus != 2 ||
				childrens_creche.defender_morale_minimum != 1 ||
				naval_yard.unit_morale_water_bonus != 2 ||
				naval_yard.water_defense_multiplier != 2.0 ||
				aerospace_complex.unit_morale_air_bonus != 2 ||
				aerospace_complex.air_defense_multiplier != 2.0 ||
				biology_lab.required_technology != 'CentauriEmpathy' ||
				biology_lab.research_bonus != 2 ||
				biology_lab.native_lifecycle_bonus != 1 ||
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
				temple_of_planet.native_lifecycle_bonus != 1
			) {
				return 'built Recycling Tanks definition is invalid';
			}
			return #undefined;
		};

		const get_base_state_error = (player_id) => {
			if (game.get_um().has_unit(defeated_snapshot_unit_id)) {
				return 'defeated unit was restored from the snapshot';
			}
			const base = find_base_for_player(player_id);
			if (base == null) {
				return 'base is missing';
			}
			if (base.get('network_node_artifact_linked') != true) {
				return 'Network Node artifact state is missing';
			}
			if (!game.get_um().has_unit(air_snapshot_unit_id)) {
				return 'partially fueled Needlejet is missing';
			}
			const air_unit = game.get_um().get_unit(air_snapshot_unit_id);
			const air_def = air_unit.get_def();
			if (
				air_unit.owner != player_id || air_unit.fuel != 1 ||
				air_def.chassis != 'Needlejet' || air_def.operational_range != 2 ||
				air_def.is_missile || !air_def.is_air
			) {
				return 'Needlejet fuel or definition metadata is invalid';
			}
			const production_state_error = get_production_state_error(base);
			if (#is_defined(production_state_error)) {
				return production_state_error;
			}
			const accumulated_nutrients = base.get('accumulated_nutrients');
			if (!#is_defined(accumulated_nutrients)) {
				return 'accumulated nutrients are missing';
			}
			const expected_snapshot_nutrients =
				initial_nutrient_stamp +
					base.get_tile().get_resources(base.get_owner()).NUTRIENTS -
					game.get('map_growth_base') +
					game.get_bm().get_facility_def('RecyclingTanks').nutrient_bonus;
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
				#print('RUNNING_RECONNECT_FAIL_' + role + ': synchronized state is incomplete');
				glsmac.exit();
				return;
			}

			if (turn_id == 1) {
				for (player of game.get_players()) {
					const research_error = get_research_state_error(player, !game.is_master());
					if (#is_defined(research_error)) {
						#print('RUNNING_RECONNECT_FAIL_' + role + ': ' + research_error);
						glsmac.exit();
						return;
					}
				}
				#print('RUNNING_RECONNECT_RESEARCH_INITIAL_' + role);
				if (game.is_master()) {
					const client_base = find_base_for_player(get_remote_player_id());
					if (client_base == null) {
						#print('RUNNING_RECONNECT_FAIL_HOST: client base is missing');
						glsmac.exit();
						return;
					}
					let conquest_tile = null;
					for (tile of client_base.get_unworked_tiles()) {
						if (tile.get_base() == null) {
							conquest_tile = tile;
							break;
						}
					}
					if (conquest_tile == null) {
						#print('RUNNING_RECONNECT_FAIL_HOST: conquest probe tile is missing');
						glsmac.exit();
						return;
					}
					const conquered_base = game.get_bm().spawn_base(game.get_player(), conquest_tile, {
						name: conquered_snapshot_base_name,
					});
					conquered_base.set_owner(client_base.get_owner());
					// Initial growth adds the base-tile yield, then spends the map growth threshold.
					client_base.set('accumulated_nutrients', initial_nutrient_stamp);
					client_base.set('network_node_artifact_linked', true);
					const production_ids = get_snapshot_production_ids(client_base);
					client_base.add_facility('RecyclingTanks');
					client_base.set_production_queue([
						{kind: 'unit', id: production_ids[0]},
						{kind: 'unit', id: production_ids[1]},
					]);
					client_base.set_accumulated_minerals(initial_mineral_stamp);
					const defeated_unit = game.get_um().spawn_unit({
						def: 'MindWorms',
						owner: client_base.get_owner(),
						tile: client_base.get_tile(),
						morale: 1,
						health: 1.0,
						home_base_id: client_base.id,
					});
					if (defeated_unit.id != defeated_snapshot_unit_id) {
						#print('RUNNING_RECONNECT_FAIL_HOST: unexpected defeated unit id');
						glsmac.exit();
						return;
					}
					defeated_unit.health = 0.0;
					const founding_site_coords = find_founding_site_coords();
					if (founding_site_coords == null) {
						#print('RUNNING_RECONNECT_FAIL_HOST: colony founding site is missing');
						glsmac.exit();
						return;
					}
					game.event('spawn_unit', {
						owner: client_base.get_owner(),
						tile: game.get_tm().get_tile(
							founding_site_coords.x,
							founding_site_coords.y
						),
						type: 'ColonyPod',
						health: 1.0,
						morale: 2,
						home_base_id: client_base.id,
					});
					if (terraform_site_coords == null) {
						#print('RUNNING_RECONNECT_FAIL_HOST: terraforming site is missing');
						glsmac.exit();
						return;
					}
					game.event('spawn_unit', {
						owner: client_base.get_owner(),
						tile: game.get_tm().get_tile(
							terraform_site_coords.x,
							terraform_site_coords.y
						),
						type: 'Former',
						health: 1.0,
						morale: 2,
						home_base_id: client_base.id,
					});
					const air_def = find_unit_def_by_chassis('Needlejet');
					if (air_def == null || air_def.operational_range != 2 || air_def.is_missile) {
						#print('RUNNING_RECONNECT_FAIL_HOST: Needlejet definition is missing');
						glsmac.exit();
						return;
					}
					game.event('spawn_unit', {
						owner: client_base.get_owner(),
						tile: client_base.get_tile(),
						type: air_def.id,
						health: 1.0,
						morale: 2,
						fuel: 1,
					});
					#print('RUNNING_RECONNECT_HOST_WAITING');
					game.event('complete_turn', {});
				}
				else {
					const base_state_error = get_base_state_error(game.get_player().id);
					if (#is_defined(base_state_error)) {
						#print('RUNNING_RECONNECT_FAIL_CLIENT: ' + base_state_error);
						glsmac.exit();
						return;
					}
					#print('RUNNING_RECONNECT_BASE_STATE_INITIAL_CLIENT');
					run_initial_founding_probe();
				}
			}
			else if (turn_id == 2 && game.is_master() && !exit_scheduled) {
				for (player of game.get_players()) {
					const research_error = get_research_state_error(player, true);
					if (#is_defined(research_error)) {
						#print('RUNNING_RECONNECT_FAIL_HOST: ' + research_error);
						glsmac.exit();
						return;
					}
				}
				const terraform_state_error = get_terraform_state_error(
					get_remote_player_id(),
					3,
					false
				);
				if (#is_defined(terraform_state_error)) {
					#print('RUNNING_RECONNECT_FAIL_HOST: ' + terraform_state_error);
					glsmac.exit();
					return;
				}
				const former_tile = game.get_um().get_unit(former_snapshot_unit_id).get_tile();
				if (
					terraform_site_coords == null ||
					former_tile.x != terraform_site_coords.x ||
					former_tile.y != terraform_site_coords.y
				) {
					#print('RUNNING_RECONNECT_FAIL_HOST: Former tile changed after reconnect');
					glsmac.exit();
					return;
				}
				exit_scheduled = true;
				#print('RUNNING_RECONNECT_RESEARCH_RESUMED_HOST');
				#print('RUNNING_RECONNECT_TERRAFORM_ADVANCED_HOST');
				#print('RUNNING_RECONNECT_PASS_HOST');
				#async(5000, () => {
					glsmac.exit();
				});
			}
		};

		game.on('start_ui', (e) => {
			if (!game.is_master() && game.get_turn() > 0) {
				handle_turn(game.get_turn());
			}
		});

		game.on('start', (e) => {
			game.on('turn', (e) => {
				handle_turn(e.year - 2100);
			});
		});
	});

	glsmac.run();

});
