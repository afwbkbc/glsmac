#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);
	const process_base_production = #include('../default/game/event/process_base_production');

	const technologies = #include('../default/technologies');
	const facility_catalog = #include('../default/facilities');
	let production_gates = [
		['unit', 'Former', 'CentauriEcology'],
		['unit', 'ReconRover', 'DoctrineMobility'],
		['unit', 'LaserInfantry', 'AppliedPhysics'],
		['unit', 'SynthmetalSentinels', 'IndustrialBase'],
	];
	let facility_ids = [];
	let project_ids = [];
	for (entry of facility_catalog.definitions) {
		const kind = entry.data.is_project ? 'project' : 'facility';
		if (entry.data.is_project) {
			project_ids :+entry.id;
		} else {
			facility_ids :+entry.id;
		}
		production_gates :+[kind, entry.id, entry.data.required_technology];
	}

	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;

	const fail = (message) => {
		#print('RESEARCH_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print(
				'RESEARCH_RUNTIME_PASS: validated 77 technologies, 37 facilities, 33 projects, and batch production gates'
			);
			#async(500, () => { glsmac.exit(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;
		let last_unit_upgrade = null;
		let victory_projects_unlocked = null;
		game.register_event('research_runtime_seed_energy', {
			validate: (e) => {
				if (e.caller != 0 && e.caller != e.data.player.id) {
					return 'Only the runtime test player may seed its energy balance';
				}
			},
			apply: (e) => {
				const previous = e.data.player.energy_credits;
				e.data.player.set_energy_credits(e.data.energy_credits);
				e.game.trigger('economy_updated', {player: e.data.player});
				return {energy_credits: previous};
			},
			rollback: (e) => {
				e.data.player.set_energy_credits(e.applied.energy_credits);
				e.game.trigger('economy_updated', {player: e.data.player});
			},
		});
		game.register_event('research_runtime_complete_projects', {
			validate: (e) => {
				if (e.caller != 0 && e.caller != e.data.base.get_owner().id) {
					return 'Only the runtime test base owner may complete its projects';
				}
			},
			apply: (e) => {
				e.data.base.add_facility('TheVoiceOfPlanet');
				const ascent = e.game.get_bm().get_facility_def(
					'TheAscentToTranscendence'
				);
				const unlocked = e.data.base.can_set_production('project', ascent.id);
				e.data.base.add_facility(ascent.id);
				e.game.trigger('research_runtime_projects_completed', {
					unlocked: unlocked,
				});
				return {};
			},
			rollback: (e) => {
				e.data.base.remove_facility('TheAscentToTranscendence');
				e.data.base.remove_facility('TheVoiceOfPlanet');
			},
		});
		game.on('unit_upgraded', (upgrade) => {
			last_unit_upgrade = {
				unit_id: upgrade.unit.id,
				target_def_id: upgrade.unit.def,
				previous_def_id: upgrade.previous_def_id,
				cost: upgrade.cost,
				energy_credits: upgrade.player.get_energy_credits(),
			};
		});
		game.on('research_runtime_projects_completed', (result) => {
			victory_projects_unlocked = result.unlocked;
		});

		game.on('start_ui', (e) => {
			ui_started = true;
			finish_if_ready();
		});

		game.on('turn', (e) => {
			const turn_id = e.year - 2100;
			const player = game.get_player();
			let base = null;
			for (candidate of game.get_bm().get_bases()) {
				if (candidate.get_owner().id == player.id) {
					base = candidate;
					break;
				}
			}
			if (base == null) {
				fail('player base is missing');
				return;
			}

			if (turn_id == 1) {
				if (!base.has_facility('Headquarters')) {
					fail('starting base has no headquarters');
					return;
				}
				let catalog_count = 0;
				for (technology_id of technologies.order) {
					const definition = game.get('f_technology_get_definition')(technology_id);
					if (
						definition == null ||
						definition.id != technology_id ||
						definition.name == '' ||
						definition.cost <= 0
					) {
						fail('technology definition is invalid: ' + technology_id);
						return;
					}
					catalog_count++;
				}
				if (catalog_count != 77) {
					fail('technology catalog count is ' + #to_string(catalog_count));
					return;
				}

				const state = player.get_research_state();
				if (
					player.get_faction().id != 'GAIANS' ||
					state.technologies != ['CentauriEcology'] ||
					state.target != 'Biogenetics' ||
					state.progress != 0
				) {
					fail('initial Gaian research state is invalid');
					return;
				}
				for (gate of production_gates) {
					const should_be_available =
						(gate[2] == '' || gate[2] == 'CentauriEcology') &&
						!base.has_facility(gate[1]);
					if (base.can_set_production(gate[0], gate[1]) != should_be_available) {
						fail('initial production gate is invalid: ' + gate[1]);
						return;
					}
				}

				const biogenetics = game.get('f_technology_get_definition')('Biogenetics');
				player.set_research_state({
					technologies: ['CentauriEcology'],
					target: 'Biogenetics',
					progress: biogenetics.cost - 1,
				});
				game.event('complete_turn', {});
				return;
			}

			if (turn_id == 2) {
				const state = player.get_research_state();
				if (
					state.technologies != ['Biogenetics', 'CentauriEcology'] ||
					state.target != 'IndustrialBase' ||
					state.progress < 0 ||
					state.progress >= game.get('f_technology_get_definition')('IndustrialBase').cost
				) {
					fail('live research completion did not advance to Industrial Base');
					return;
				}

				let all_technologies = [];
				for (technology_id of technologies.order) {
					all_technologies :+technology_id;
				}
				player.set_research_state({
					technologies: all_technologies,
					target: '',
					progress: 0,
				});
				let funding_requested = false;
				let funding_wait_ticks = 0;
				#async(100, () => {
					if (!ui_started) {
						return true;
					}
					funding_wait_ticks++;
					const funded_player = game.get_player();
					if (!funding_requested) {
						funding_requested = true;
						game.event('research_runtime_seed_energy', {
							player: funded_player,
							energy_credits: 1000,
						});
						return true;
					}
					if (funded_player.get_energy_credits() != 1000) {
						if (funding_wait_ticks >= 100) {
							fail('social engineering test funding timed out');
							return false;
						}
						return true;
					}
					game.event('set_social_engineering', {
						player: funded_player,
						choices: {
							politics: 'Democratic',
							economics: 'Planned',
							values: 'Wealth',
							future_society: 'None',
						},
					});
					let social_wait_ticks = 0;
					#async(100, () => {
						social_wait_ticks++;
						const social_player = game.get_player();
						if (social_player.get_social_engineering().politics != 'Democratic') {
							if (social_wait_ticks >= 100) {
								fail('social engineering event application timed out');
								return false;
							}
							return true;
						}
						if (social_player.get_energy_credits() != 680) {
							fail('three-model Transcend upheaval cost was not 320 energy credits');
							return false;
						}
						game.event('complete_turn', {});
						return false;
					});
					return false;
				});
				return;
			}

			if (turn_id == 3) {
				const social_choices = player.get_social_engineering();
				const social_ratings = game.get('f_social_get_ratings')(player);
				const social_energy = base.get_intake().ENERGY;
				const social_growth_cost = game.get(
					'f_base_get_nutrients_for_growth'
				)(game, base);
				player.set_social_engineering({
					politics: 'Frontier',
					economics: 'Simple',
					values: 'Survival',
					future_society: 'None',
				});
				const baseline_energy = base.get_intake().ENERGY;
				const baseline_growth_cost = game.get(
					'f_base_get_nutrients_for_growth'
				)(game, base);
				const ecological_damage = game.get('f_ecology_get_base_damage')(base);
				player.set_social_engineering(social_choices);
				if (
					social_choices.politics != 'Democratic' ||
					social_choices.economics != 'Planned' ||
					social_choices.values != 'Wealth' ||
					social_choices.future_society != 'None'
				) {
					fail('live social engineering event did not store its choices');
					return;
				}
				if (
					social_ratings.economy != 1 || social_ratings.effic != 2 ||
					social_ratings.support != 0 - 2 || social_ratings.morale != 0 - 3 ||
					social_ratings.police != 0 - 1 || social_ratings.growth != 4 ||
					social_ratings.planet != 1 || social_ratings.industry != 2
				) {
					fail('live social engineering ratings are invalid');
					return;
				}
				if (social_energy != baseline_energy + 1) {
					fail('ECONOMY rating did not increase live base energy');
					return;
				}
				if (social_growth_cost >= baseline_growth_cost) {
					fail('GROWTH rating did not reduce live base growth cost');
					return;
				}
				if (game.get('f_social_get_free_support')(player, base.get_size()) != 1) {
					fail('SUPPORT rating returned an invalid live free-unit allowance');
					return;
				}
				if (game.get('f_social_get_new_base_minerals')(player) != 0) {
					fail('SUPPORT penalty did not remove free minerals from new bases');
					return;
				}
				if (game.get('f_social_get_unit_training_morale_bonus')(player, 4) != 2) {
					fail('MORALE penalty did not halve positive unit training modifiers');
					return;
				}
				if (game.get('f_social_get_mineral_cost')(player, 40) != 32) {
					fail('INDUSTRY rating returned an invalid live mineral cost');
					return;
				}
				if (
					ecological_damage.percent < 0 ||
					ecological_damage.clean_allowance != 16 ||
					ecological_damage.facility_divisor != 1 ||
					player.get_ecological_damage_events() != 0
				) {
					fail('live ecological damage state is invalid');
					return;
				}
				player.set_ecological_damage_events(3);
				if (
					game.get('f_ecology_get_base_damage')(base).clean_allowance != 19
				) {
					fail('fungal bloom clean-mineral allowance did not update');
					return;
				}
				player.set_ecological_damage_events(0);
				const unit_defs = game.get_um().get_unit_defs();
				let found_late_land_unit = false;
				let found_sea_unit = false;
				let found_air_unit = false;
				let found_clean_unit = false;
				let found_trained_unit = false;
				let found_improved_former = false;
				for (unit_def of unit_defs) {
					if (unit_def.offense >= 20 && unit_def.is_land) {
						found_late_land_unit = base.can_set_production('unit', unit_def.id);
					}
					if (unit_def.offense > 1 && unit_def.is_water) {
						found_sea_unit = true;
					}
					if (unit_def.offense > 1 && unit_def.is_air) {
						found_air_unit = base.can_set_production('unit', unit_def.id);
					}
					for (ability of unit_def.abilities) {
						if (ability == 'CleanReactor') {
							found_clean_unit = base.can_set_production('unit', unit_def.id);
						} else if (ability == 'HighMorale') {
							found_trained_unit = base.can_set_production('unit', unit_def.id);
						} else if (
							unit_def.can_terraform &&
							(ability == 'SuperFormer' || ability == 'FungicideTanks')
						) {
							found_improved_former = base.can_set_production('unit', unit_def.id);
						}
					}
				}
				if (
					#sizeof(unit_defs) < 102 || !found_late_land_unit ||
					!found_sea_unit || !found_air_unit || !found_clean_unit ||
					!found_trained_unit || !found_improved_former
				) {
					fail('generated unit catalog is unavailable at runtime');
					return;
				}
				base.add_facility('AerospaceComplex');
				for (gate of production_gates) {
					const expected_available =
						gate[1] != 'HabitationDome' && gate[1] != 'TheAscentToTranscendence' &&
						!base.has_facility(gate[1]);
					if (base.can_set_production(gate[0], gate[1]) != expected_available) {
						fail('full-catalog production gate stayed locked: ' + gate[1]);
						return;
					}
				}
				base.remove_facility('AerospaceComplex');
				base.add_facility('HabComplex');
				if (!base.can_set_production('facility', 'HabitationDome')) {
					fail('Habitation Dome stayed locked after Hab Complex');
					return;
				}
				base.remove_facility('HabComplex');

				if (#sizeof(project_ids) != 33) {
					fail('secret project runtime catalog count is invalid');
					return;
				}
				const supercollider = game.get_bm().get_facility_def('TheSupercollider');
				const project_labs_before = game.get('f_technology_get_base_labs')(base).total;
				base.add_facility(supercollider.id);
				const project_labs_after = game.get('f_technology_get_base_labs')(base).total;
				if (
					supercollider.production_kind != 'project' || !supercollider.is_project ||
					game.get_bm().get_project_base(supercollider.id) != base ||
					base.can_set_production('project', supercollider.id) ||
					project_labs_after <= project_labs_before
				) {
					fail('secret project ownership or local effect is invalid');
					return;
				}
				base.remove_facility(supercollider.id);

				const effect_project_ids = [
					'TheHumanGenomeProject',
					'TheCommandNexus',
					'TheWeatherParadigm',
					'TheMerchantExchange',
					'TheCitizensDefenseForce',
					'TheVirtualWorld',
					'ThePlanetaryTransitSystem',
					'TheXenoempathyDome',
					'TheNeuralAmplifier',
					'TheMaritimeControlCenter',
					'TheSupercollider',
					'TheAsceticVirtues',
					'TheLongevityVaccine',
					'ThePholusMutagen',
					'TheCyborgFactory',
					'TheTheoryOfEverything',
					'TheDreamTwister',
					'TheNetworkBackbone',
					'TheNanoFactory',
					'TheLivingRefinery',
					'TheCloningVats',
					'TheSelfAwareColony',
					'ClinicalImmortality',
					'TheSpaceElevator',
					'TheSingularityInductor',
					'TheBulkMatterTransmitter',
					'TheTelepathicMatrix',
				];
				for (project_id of effect_project_ids) {
					base.add_facility(project_id);
				}
				const project_effects = game.get('f_project_get_effects')(base);
				let effective_ids = {};
				for (definition of game.get('f_base_get_effective_facilities')(base)) {
					effective_ids[definition.id] = true;
				}
				if (
					#sizeof(game.get('f_project_get_owned')(base)) != 27 ||
					project_effects.talent_bonus != 2 ||
					project_effects.growth_rating_bonus != 10 ||
					project_effects.population_limit_bonus != 2 ||
					project_effects.mineral_bonus != 2 ||
					project_effects.support_bonus != 2 ||
					project_effects.maintenance_multiplier != 0.5 ||
					project_effects.native_lifecycle_bonus != 2 ||
					project_effects.network_node_drone_modifier != -2 ||
					project_effects.network_node_research_bonus != 0 ||
					project_effects.terraforming_rate_multiplier != 1.5 ||
					project_effects.new_base_population != 3 ||
					project_effects.small_base_drone_modifier != -1 ||
					project_effects.psi_attack_multiplier != 1.5 ||
					project_effects.psi_defense_multiplier != 1.5 ||
					project_effects.naval_movement_bonus != 2.0 ||
					project_effects.police_rating_bonus != 1 ||
					project_effects.extra_police_units != 1 ||
					project_effects.unit_upgrade_cost_multiplier != 0.5 ||
					project_effects.drone_modifier != -2 ||
					project_effects.economy_multiplier != 0.0 ||
					!project_effects.ignore_power_penalties ||
					!project_effects.ignore_thought_control_penalties ||
					!project_effects.ignore_cybernetic_penalties ||
					!project_effects.full_repair ||
					!project_effects.prevent_riots ||
					!#is_defined(effective_ids.CommandCenter) ||
					!#is_defined(effective_ids.PerimeterDefense) ||
					!#is_defined(effective_ids.NavalYard) ||
					!#is_defined(effective_ids.BioenhancementCenter) ||
					!#is_defined(effective_ids.QuantumConverter)
				) {
					fail('batch secret project effects are invalid');
					return;
				}
				for (project_id of effect_project_ids) {
					base.remove_facility(project_id);
				}

				const intake_before = base.get_intake();
				const consumption_before = base.get_consumption().ENERGY;
				const labs_before = game.get('f_technology_get_base_labs')(base).total;
				let existing_facility_energy_bonus = 0;
				for (existing_facility of base.get_facilities()) {
					existing_facility_energy_bonus += existing_facility.energy_bonus;
				}
				let nutrient_bonus = 0;
				let mineral_bonus = 0;
				let energy_bonus = 0;
				let maintenance = 0;
				let mineral_multiplier = 0.0;
				let psych_bonus = 0;
				let psych_multiplier = 0.0;
				let research_multiplier = 0.0;
				let research_bonus = 0;
				let defense_multiplier = 1.0;
				let morale_bonus = 0;
				let land_morale_bonus = 0;
				let water_morale_bonus = 0;
				let air_morale_bonus = 0;
				let water_defense_multiplier = 1.0;
				let air_defense_multiplier = 1.0;
				let growth_rating_bonus = 0;
				let native_lifecycle_bonus = 0;
				let drone_modifier = 0;
				let talent_bonus = 0;
				let suppress_psych = 0;
				for (facility_id of facility_ids) {
					const definition = game.get_bm().get_facility_def(facility_id);
					nutrient_bonus += definition.nutrient_bonus;
					mineral_bonus += definition.mineral_bonus;
					energy_bonus += definition.energy_bonus;
					maintenance += definition.energy_maintenance;
					mineral_multiplier += definition.mineral_multiplier;
					psych_bonus += definition.psych_bonus;
					psych_multiplier += definition.psych_multiplier;
					research_multiplier += definition.research_multiplier;
					research_bonus += definition.research_bonus;
					defense_multiplier += #max(definition.defense_multiplier - 1.0, 0.0);
					morale_bonus += definition.unit_morale_bonus;
					land_morale_bonus += definition.unit_morale_land_bonus;
					water_morale_bonus += definition.unit_morale_water_bonus;
					air_morale_bonus += definition.unit_morale_air_bonus;
					water_defense_multiplier += #max(
						definition.water_defense_multiplier - 1.0,
						0.0
					);
					air_defense_multiplier += #max(
						definition.air_defense_multiplier - 1.0,
						0.0
					);
					growth_rating_bonus += definition.growth_rating_bonus;
					native_lifecycle_bonus += definition.native_lifecycle_bonus;
					drone_modifier += definition.drone_modifier;
					talent_bonus += definition.talent_bonus;
					suppress_psych += definition.suppress_psych ? 1 : 0;
					const is_orbital = definition.orbital_resource != '' ||
						definition.orbital_defense;
					if (
						definition.mineral_to_energy_divisor == 0 &&
						!is_orbital &&
						!base.has_facility(facility_id)
					) {
						base.add_facility(facility_id);
					}
				}
				const intake_after = base.get_intake();
				const psych_after = game.get('f_economy_get_base_allocation')(game, base).psych;
				const labs_after = game.get('f_technology_get_base_labs')(base);
				if (
					#sizeof(facility_ids) != 38 ||
					nutrient_bonus != 2 || mineral_bonus != 2 || energy_bonus != 3 ||
					maintenance != 75 || mineral_multiplier != 2.0 ||
					psych_bonus != 0 || psych_multiplier != 2.0 ||
					research_multiplier != 2.0 || research_bonus != 2 ||
					defense_multiplier != 3.0 || morale_bonus != 2 ||
					land_morale_bonus != 2 || water_morale_bonus != 2 ||
					air_morale_bonus != 2 || water_defense_multiplier != 2.0 ||
					air_defense_multiplier != 2.0 || growth_rating_bonus != 2 ||
					native_lifecycle_bonus != 4 ||
					drone_modifier != -5 || talent_bonus != 2 || suppress_psych != 1 ||
					intake_after.NUTRIENTS != intake_before.NUTRIENTS + nutrient_bonus ||
					intake_after.MINERALS != #ceil(
						#to_float(intake_before.MINERALS + mineral_bonus) * (1.0 + mineral_multiplier)
					) ||
					intake_after.ENERGY != intake_before.ENERGY + energy_bonus -
						existing_facility_energy_bonus ||
					base.get_consumption().ENERGY != consumption_before + maintenance ||
					psych_after.bonus != psych_bonus + #ceil(
						#to_float(psych_after.value) * psych_multiplier
					) ||
					labs_after.bonus != 2 + research_bonus + #ceil(
						#to_float(labs_after.value + 2 + research_bonus) * research_multiplier
					) ||
					labs_after.total <= labs_before
				) {
					fail('batch facility effects are invalid');
					return;
				}
				base.set_production_queue([]);
				for (facility_id of facility_ids) {
					const definition = game.get_bm().get_facility_def(facility_id);
					const is_orbital = definition.orbital_resource != '' ||
						definition.orbital_defense;
					const has_facility = base.has_facility(facility_id);
					const can_set = base.can_set_production('facility', facility_id);
					const can_queue = base.can_queue_production('facility', facility_id);
					let construction_is_valid = has_facility;
					if (definition.mineral_to_energy_divisor > 0) {
						construction_is_valid = !has_facility && can_set && !can_queue;
					} else if (is_orbital) {
						construction_is_valid = !has_facility && can_set && can_queue;
					}
					if (!construction_is_valid) {
						fail(
							'batch facility construction missed ' + facility_id +
							' (installed=' + #to_string(has_facility) +
							', set=' + #to_string(can_set) +
							', queue=' + #to_string(can_queue) + ')'
						);
						return;
					}
				}

				let previous_queue = [];
				for (queued of base.get_production_queue()) {
					previous_queue :+{
						kind: queued.production_kind,
						id: queued.id,
					};
				}
				const previous_minerals = base.get_accumulated_minerals();
				base.set_accumulated_minerals(13);
				base.set_production('facility', 'StockpileEnergy');
				const stockpile_credits = game.get(
					'f_economy_get_base_stockpile_energy'
				)(game, base);
				let stockpile_event = {caller: 0, game: game, data: {base: base}};
				stockpile_event.applied = process_base_production.apply(stockpile_event);
				if (
					stockpile_credits <= 0 ||
					base.get_accumulated_minerals() != 13 ||
					base.get_production().id != 'StockpileEnergy' ||
					base.has_facility('StockpileEnergy')
				) {
					fail('Stockpile Energy runtime conversion state is invalid');
					return;
				}
				process_base_production.rollback(stockpile_event);
				base.set_production_queue(previous_queue);
				base.set_accumulated_minerals(previous_minerals);

				const sky = game.get_bm().get_facility_def('SkyHydroponicsLab');
				const orbital_count = player.get_orbital_facility_count(sky.id);
				const orbital_intake_before = base.get_intake().NUTRIENTS;
				const orbital_cost = game.get('f_base_get_production_cost')(base, sky);
				base.set_accumulated_minerals(orbital_cost);
				base.set_production('facility', sky.id);
				let orbital_event = {caller: 0, game: game, data: {base: base}};
				orbital_event.applied = process_base_production.apply(orbital_event);
				if (
					player.get_orbital_facility_count(sky.id) != orbital_count + 1 ||
					base.has_facility(sky.id) ||
					base.get_intake().NUTRIENTS != orbital_intake_before + 1
				) {
					fail('orbital launch or resource contribution is invalid');
					return;
				}
				process_base_production.rollback(orbital_event);
				if (
					player.get_orbital_facility_count(sky.id) != orbital_count ||
					base.get_intake().NUTRIENTS != orbital_intake_before
				) {
					fail('orbital launch rollback is invalid');
					return;
				}
				base.set_production_queue(previous_queue);
				base.set_accumulated_minerals(previous_minerals);

				base.add_facility('TheSpaceElevator');
				base.remove_facility('AerospaceComplex');
				const elevator_cost = game.get('f_base_get_production_cost')(base, sky);
				const industry_cost = game.get('f_social_get_mineral_cost')(
					player,
					sky.mineral_cost
				);
				const elevator_access = base.can_set_production('facility', sky.id);
				const expected_elevator_cost = #ceil(#to_float(industry_cost) / 2.0);
				if (
					!elevator_access || elevator_cost != expected_elevator_cost
				) {
					fail(
						'Space Elevator orbital access or production multiplier is invalid' +
						' (access=' + #to_string(elevator_access) +
						', cost=' + #to_string(elevator_cost) +
						', expected=' + #to_string(expected_elevator_cost) + ')'
					);
					return;
				}
				base.remove_facility('TheSpaceElevator');
				base.add_facility('AerospaceComplex');

				const begin_victory_test = () => {
					const voice = game.get_bm().get_facility_def('TheVoiceOfPlanet');
					const ascent = game.get_bm().get_facility_def('TheAscentToTranscendence');
					if (
						voice.global_native_lifecycle_bonus != 1 ||
						ascent.required_project != 'TheVoiceOfPlanet' ||
						base.can_set_production('project', ascent.id)
					) {
						fail('Ascent was available before Voice of Planet');
						return;
					}
					victory_projects_unlocked = null;
					game.event('research_runtime_complete_projects', {base: base});
					let victory_wait_ticks = 0;
					#async(100, () => {
						victory_wait_ticks++;
						if (victory_projects_unlocked == null || !game.is_game_over()) {
							if (victory_wait_ticks >= 100) {
								fail('transcendence victory timed out');
								return false;
							}
							return true;
						}
						if (!victory_projects_unlocked) {
							fail('Ascent stayed locked after Voice of Planet');
							return false;
						}
						const victory = game.get_victory_state();
						if (victory != {type: 'transcendence', winner: player.id, turn: 3}) {
							fail('transcendence victory state is invalid');
							return false;
						}
						runtime_complete = true;
						finish_if_ready();
						return false;
					});
				};

				base.add_facility('TheNanoFactory');
				player.set_prototyped_components([
					'ColonyModule', 'HandWeapons', 'Infantry', 'Laser', 'NoArmor',
				]);
				const upgrade_source = game.get_um().spawn_unit({
					def: 'ScoutPatrol',
					owner: player,
					tile: base.get_tile(),
					morale: 4,
					health: 0.7,
					home_base_id: base.id,
				});
				const upgrade_id = upgrade_source.id;
				const upgrade_energy = player.get_energy_credits();
				last_unit_upgrade = null;
				game.event('upgrade_unit', {
					unit: upgrade_source,
					target_def_id: 'LaserInfantry',
				});
				let upgrade_wait_ticks = 0;
				#async(100, () => {
					upgrade_wait_ticks++;
					if (
						!game.get_um().has_unit(upgrade_id) ||
						game.get_um().get_unit(upgrade_id).def == 'ScoutPatrol' ||
						last_unit_upgrade == null
					) {
						if (upgrade_wait_ticks >= 100) {
							fail('unit upgrade timed out');
							return false;
						}
						return true;
					}
					const upgraded = game.get_um().get_unit(upgrade_id);
					if (
						upgraded.def != 'LaserInfantry' || upgraded.morale != 4 ||
						upgraded.health < 0.699 || upgraded.health > 0.701 ||
						upgraded.home_base_id != base.id || upgraded.movement != 0.0 ||
						!upgraded.moved_this_turn ||
						last_unit_upgrade.unit_id != upgrade_id ||
						last_unit_upgrade.target_def_id != 'LaserInfantry' ||
						last_unit_upgrade.previous_def_id != 'ScoutPatrol' ||
						last_unit_upgrade.cost != 10 ||
						last_unit_upgrade.energy_credits != upgrade_energy - 10
					) {
						fail('unit upgrade runtime state is invalid');
						return false;
					}
					begin_victory_test();
					return false;
				});
				return;
			}

			fail('runtime test exceeded three turns');
		});
	});

	glsmac.run();

});
