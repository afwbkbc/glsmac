#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);
	const process_base_production = #include('../default/game/event/process_base_production');

	let starting_pop_count = 0;
	let starting_base_unit_count = 0;
	let starting_intake = #undefined;
	let queued_unit_id = #undefined;
	let lifecycle_verified = false;
	let expansion_verified = false;
	let terraforming_verified = false;
	let repair_verified = false;
	let turn_three_advance_requested = false;
	let former_id = 0;
	let fungus_former_id = 0;
	let field_repair_unit_id = 0;
	let terraform_site = null;
	let fungus_site = null;
	let terraform_site_resources = null;
	let ui_started = false;
	let exit_scheduled = false;
	const lifecycle_base_name = 'Runtime Smoke Base';
	const expansion_base_name = 'Runtime Expansion Base';

	const finish_if_ready = () => {
		if (lifecycle_verified && expansion_verified && terraforming_verified && repair_verified && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print('RUNTIME_SMOKE_PASS: reached turn 5 with production, lifecycle, expansion, and terraforming state intact');
			#async(500, () => {
				glsmac.exit();
			});
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		const find_founding_site_coords = () => {
			const tm = game.get_tm();
			let result = null;
			for (let y = 0; y < tm.get_map_height(); y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const tile = tm.get_tile(x, y);
					if (!tile.is_land) {
						continue;
					}
					if (tile.is_locked() || tile.get_base() != null) {
						continue;
					}
					if (#sizeof(tile.get_units()) != 0) {
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

		const advance_from_turn_three_if_ready = () => {
			if (lifecycle_verified && expansion_verified && !turn_three_advance_requested) {
				turn_three_advance_requested = true;
				game.event('complete_turn', {});
			}
		};

		game.register_event('runtime_smoke_replace_base', {
			validate: (e) => {
				if (e.caller != 0) {
					return 'Only the host can replace the lifecycle test base';
				}
			},
			apply: (e) => {
				e.game.bm.despawn_base(e.data.base_id);
				const replacement = e.game.bm.spawn_base(e.data.owner, e.data.tile, {
					name: e.data.name,
				});
				return {
					replacement_id: replacement.id,
				};
			},
			rollback: (e) => {
				throw Error('Accepted lifecycle test event was rolled back');
			},
		});

		game.on('start_ui', (e) => {
			if (game.get_player().difficulty_level != 'Transcend') {
				#print('RUNTIME_SMOKE_FAIL: quickstart player difficulty was not preserved');
				glsmac.exit();
				return;
			}
			const bases = game.get_bm().get_bases();
			if (#sizeof(bases) == 0) {
				#print('RUNTIME_SMOKE_FAIL: no starting base was available for the base screen');
				glsmac.exit();
				return;
			}
			#async(0, () => {
				game.select_base(bases[0]);
			});
			ui_started = true;
			finish_if_ready();
		});

		game.on('turn', (e) => {
			const turn_id = e.year - 2100;
			if (turn_id == 1) {
				const victory_state = game.get_victory_state();
				if (
					game.is_game_over() ||
					game.get_conquest_winner() != null ||
					victory_state != {type: '', winner: -1, turn: 0}
				) {
					#print('RUNTIME_SMOKE_FAIL: one-player game was treated as a conquest victory');
					glsmac.exit();
					return;
				}
				#print('RUNTIME_SMOKE_SINGLE_PLAYER_CONQUEST_GUARD_PASS');
				const um = game.get_um();
				const bases = game.get_bm().get_bases();
				if (!um.has_unit(1) || #sizeof(bases) == 0) {
					#print('RUNTIME_SMOKE_FAIL: starting unit or base is missing');
					glsmac.exit();
					return;
				}

				const smoke_unit = um.get_unit(1);
				const base = bases[0];
				const capital_energy = game.get('f_economy_get_base_energy')(base);
				if (
					!base.has_facility('Headquarters') ||
					base.get_accumulated_minerals() != 10 ||
					capital_energy.distance != 0 ||
					capital_energy.inefficiency != 0 ||
					capital_energy.net != capital_energy.gross
				) {
					#print('RUNTIME_SMOKE_FAIL: starting headquarters or capital energy loss is invalid');
					glsmac.exit();
					return;
				}
				#print('RUNTIME_SMOKE_HEADQUARTERS_PASS');
				let invalid_home_base_rejected = false;
				try {
					um.spawn_unit({
						def: 'Former',
						owner: game.get_player(),
						tile: base.get_tile(),
						morale: 1,
						health: 1.0,
						home_base_id: base.id + 1000000,
					});
				}
				catch {
					: (e) => {
						invalid_home_base_rejected = true;
					}
				}
				if (!invalid_home_base_rejected) {
					#print('RUNTIME_SMOKE_FAIL: invalid unit home base was accepted');
					glsmac.exit();
					return;
				}
				smoke_unit.health = 0.5;
				const research_state = game.get_player().get_research_state();
				const rover = um.get_unit_def('ReconRover');
				if (smoke_unit.home_base_id != base.id) {
					#print('RUNTIME_SMOKE_FAIL: starting scout has no home base');
					glsmac.exit();
					return;
				}
				if (base.get_consumption().MINERALS != 0) {
					#print('RUNTIME_SMOKE_FAIL: starting base has unexpected unit support');
					glsmac.exit();
					return;
				}
				if (
					!game.get_player().has_technology('CentauriEcology') ||
					research_state.technologies != ['CentauriEcology'] ||
					research_state.target != 'Biogenetics' ||
					research_state.progress != 0 ||
					!base.can_set_production('unit', 'Former') ||
					base.can_set_production('unit', 'ReconRover') ||
					rover.required_technology != 'DoctrineMobility' ||
					rover.movement_per_turn != 2.0
				) {
					#print('RUNTIME_SMOKE_FAIL: Gaians did not receive their Centauri Ecology starting technology');
					glsmac.exit();
					return;
				}
				starting_pop_count = #sizeof(base.get_pops());
				starting_base_unit_count = #sizeof(base.get_tile().get_units());
				const production = base.get_production();
				if (!#is_defined(production) || production.mineral_cost <= 0) {
					#print('RUNTIME_SMOKE_FAIL: starting base has no valid production');
					glsmac.exit();
					return;
				}
				const recycling_tanks = game.get_bm().get_facility_def('RecyclingTanks');
				if (
					recycling_tanks.production_kind != 'facility' ||
					recycling_tanks.mineral_cost != 40 ||
					recycling_tanks.required_technology != 'Biogenetics' ||
					base.can_set_production('facility', recycling_tanks.id) ||
					base.has_facility(recycling_tanks.id)
				) {
					#print('RUNTIME_SMOKE_FAIL: Recycling Tanks definition or starting state is invalid');
					glsmac.exit();
					return;
				}
				game.get_player().set_research_state({
					technologies: ['CentauriEcology', 'Biogenetics'],
					target: 'IndustrialBase',
					progress: 0,
				});
				if (!base.can_set_production('facility', recycling_tanks.id)) {
					#print('RUNTIME_SMOKE_FAIL: Biogenetics did not unlock Recycling Tanks');
					glsmac.exit();
					return;
				}
				queued_unit_id = production.id;
				const intake = base.get_intake();
				starting_intake = {
					nutrients: intake.NUTRIENTS,
					minerals: intake.MINERALS,
					energy: intake.ENERGY,
				};
				base.set_production_queue([
					{kind: 'facility', id: recycling_tanks.id},
					{kind: 'unit', id: queued_unit_id},
				]);
				base.set_accumulated_minerals(recycling_tanks.mineral_cost);
				const unworked_tiles = base.get_unworked_tiles();
				if (#sizeof(unworked_tiles) == 0) {
					#print('RUNTIME_SMOKE_FAIL: no free tile for base lifecycle coverage');
					glsmac.exit();
					return;
				}
				const lifecycle_tile = unworked_tiles[0];
				for (candidate of unworked_tiles) {
					if (
						candidate != lifecycle_tile &&
						candidate.is_land &&
						candidate.rockiness < 3 &&
						!candidate.features.xenofungus &&
						candidate.get_base() == null &&
						#sizeof(candidate.get_units()) == 0
					) {
						terraform_site = candidate;
						break;
					}
				}
				if (terraform_site == null) {
					#print('RUNTIME_SMOKE_FAIL: no legal Former test tile is available');
					glsmac.exit();
					return;
				}
				terraform_site_resources = terraform_site.get_resources();
				const former = um.spawn_unit({
					def: 'Former',
					owner: game.get_player(),
					tile: terraform_site,
					morale: 2,
					health: 0.5,
				});
				const former_def = former.get_def();
				if (
					former_def.is_native ||
					former_def.offense != 0 ||
					former_def.defense != 1 ||
					former_def.morale_set != 'STANDARD' ||
					former_def.required_technology != 'CentauriEcology' ||
					former_def.can_found_base ||
					!former_def.can_terraform
				) {
					#print('RUNTIME_SMOKE_FAIL: Former definition metadata is invalid');
					glsmac.exit();
					return;
				}
				former.set_terraforming_order('road', 1);
				former.set_terraforming_order('forest', 1);
				former.set_terraforming_order('none', 0);
				former_id = former.id;
				const field_repair_unit = um.spawn_unit({
					def: 'ScoutPatrol',
					owner: game.get_player(),
					tile: terraform_site,
					morale: 2,
					health: 0.75,
				});
				field_repair_unit_id = field_repair_unit.id;

				const tm = game.get_tm();
				for (let y = 0; y < tm.get_map_height(); y++) {
					for (let x = 0; x < tm.get_map_width(); x++) {
						if (x % 2 != y % 2) {
							continue;
						}
						const candidate = tm.get_tile(x, y);
						if (
							candidate.is_land &&
							candidate.features.xenofungus &&
							!candidate.is_locked() &&
							candidate.get_base() == null &&
							#sizeof(candidate.get_units()) == 0
						) {
							fungus_site = candidate;
							break;
						}
					}
					if (fungus_site != null) {
						break;
					}
				}
				if (fungus_site == null) {
					#print('RUNTIME_SMOKE_FAIL: no fungus removal site is available');
					glsmac.exit();
					return;
				}
				const fungus_former = um.spawn_unit({
					def: 'Former',
					owner: game.get_player(),
					tile: fungus_site,
					morale: 1,
					health: 1.0,
				});
				fungus_former.set_terraforming_order('remove_fungus', 1);
				fungus_former_id = fungus_former.id;

				game.event('unit_skip_turn', {
					unit: smoke_unit,
				});
				game.event('add_base_pop', {
					base: base,
					type: 'WORKER',
				});
				game.event('spawn_base', {
					owner: game.get_player(),
					tile: lifecycle_tile,
					name: lifecycle_base_name,
				});
				game.event('terraform_tile', {
					unit: former,
					type: 'farm',
				});
				game.event('complete_turn', {});
				#print('RUNTIME_SMOKE: queued facility, unit, base lifecycle, terraforming, and turn events');
			}
			else if (turn_id == 2) {
				const bases = game.get_bm().get_bases();
				if (
					#sizeof(bases) == 0 ||
					#sizeof(bases[0].get_pops()) <= starting_pop_count ||
					!game.get_um().has_unit(1)
				) {
					#print('RUNTIME_SMOKE_FAIL: state did not survive turn advancement');
					glsmac.exit();
					return;
				}
				const base = bases[0];
				const queue = base.get_production_queue();
				const intake = base.get_intake();
				if (
					!base.has_facility('RecyclingTanks') ||
					#sizeof(queue) != 1 ||
					queue[0].production_kind != 'unit' ||
					queue[0].id != queued_unit_id ||
					intake.NUTRIENTS != starting_intake.nutrients + 1 ||
					intake.MINERALS != starting_intake.minerals + 1 ||
					intake.ENERGY != starting_intake.energy + 1
				) {
					#print('RUNTIME_SMOKE_FAIL: facility completion, queue advancement, or resource bonus is invalid');
					glsmac.exit();
					return;
				}
				const former = game.get_um().get_unit(former_id);
				const fungus_former = game.get_um().get_unit(fungus_former_id);
				const field_repair_unit = game.get_um().get_unit(field_repair_unit_id);
				if (
					game.get_um().get_unit(1).health < 0.699 ||
					game.get_um().get_unit(1).health > 0.701 ||
					field_repair_unit.health < 0.799 ||
					field_repair_unit.health > 0.801 ||
					former.health != 0.5 ||
					former.terraforming != 'farm' ||
					former.terraforming_turns_remaining != 3 ||
					former.movement != 0.0 ||
					terraform_site.terraforming.farm
				) {
					#print('RUNTIME_SMOKE_FAIL: Former order did not advance into turn 2');
					glsmac.exit();
					return;
				}
				if (
					fungus_former.terraforming != 'none' ||
					fungus_former.terraforming_turns_remaining != 0
				) {
					#print('RUNTIME_SMOKE_FAIL: xenofungus removal order did not complete');
					glsmac.exit();
					return;
				}
				if (fungus_site.features.xenofungus) {
					#print('RUNTIME_SMOKE_FAIL: xenofungus removal did not update the wrapped tile feature');
					glsmac.exit();
					return;
				}
				#print('RUNTIME_SMOKE_TERRAFORM_ORDER_PASS');
				#print('RUNTIME_SMOKE_FUNGUS_REMOVAL_PASS');
				#print('RUNTIME_SMOKE_FACILITY_PRODUCTION_PASS');
				const command_center = game.get_bm().get_facility_def('CommandCenter');
				if (!command_center.full_repair_land) {
					#print('RUNTIME_SMOKE_FAIL: Command Center repair definition is invalid');
					glsmac.exit();
					return;
				}
				base.add_facility(command_center.id);
				const repair_unit = game.get_um().get_unit(1);
				repair_unit.health = 0.3;
				base.set_accumulated_minerals(queue[0].mineral_cost);
				game.event('complete_turn', {});
			}
			else if (turn_id == 3) {
				const bases = game.get_bm().get_bases();
				const former = game.get_um().get_unit(former_id);
				let produced_unit_has_home_base = false;
				if (#sizeof(bases) > 0) {
					for (unit of bases[0].get_tile().get_units()) {
						if (unit.id != 1 && unit.home_base_id == bases[0].id) {
							produced_unit_has_home_base = true;
						}
					}
				}
				if (
					#sizeof(bases) == 0 ||
					!bases[0].has_facility('RecyclingTanks') ||
					!bases[0].has_facility('CommandCenter') ||
					#sizeof(bases[0].get_tile().get_units()) <= starting_base_unit_count ||
					game.get_um().get_unit(1).health != 1.0 ||
					game.get_um().get_unit(field_repair_unit_id).health < 0.799 ||
					game.get_um().get_unit(field_repair_unit_id).health > 0.801 ||
					former.health != 0.5 ||
					!produced_unit_has_home_base
				) {
					#print('RUNTIME_SMOKE_FAIL: queued unit production or facility persistence failed');
					glsmac.exit();
					return;
				}
				repair_verified = true;
				#print('RUNTIME_SMOKE_UNIT_REPAIR_PASS');
				#print('RUNTIME_SMOKE_BASE_PRODUCTION_PASS');

				let lifecycle_base = null;
				for (base of bases) {
					if (base.name == lifecycle_base_name) {
						lifecycle_base = base;
						break;
					}
				}
				if (lifecycle_base == null) {
					#print('RUNTIME_SMOKE_FAIL: named lifecycle base is missing');
					glsmac.exit();
					return;
				}
				const remote_energy = game.get('f_economy_get_base_energy')(lifecycle_base);
				if (
					lifecycle_base.has_facility('Headquarters') ||
					remote_energy.distance <= 0 ||
					remote_energy.net != remote_energy.gross - remote_energy.inefficiency
				) {
					#print('RUNTIME_SMOKE_FAIL: remote-base efficiency calculation is invalid');
					glsmac.exit();
					return;
				}
				#print('RUNTIME_SMOKE_EFFICIENCY_PASS');
				const capital = bases[0];
				const headquarters = game.get_bm().get_facility_def('Headquarters');
				lifecycle_base.set_production('facility', headquarters.id);
				lifecycle_base.set_accumulated_minerals(headquarters.mineral_cost);
				let headquarters_event = {caller: 0, game: game, data: {base: lifecycle_base}};
				headquarters_event.applied = process_base_production.apply(headquarters_event);
				if (
					!lifecycle_base.has_facility('Headquarters') ||
					capital.has_facility('Headquarters') ||
					#sizeof(headquarters_event.applied.previous_headquarters) != 1
				) {
					#print('RUNTIME_SMOKE_FAIL: Headquarters relocation did not complete');
					glsmac.exit();
					return;
				}
				process_base_production.rollback(headquarters_event);
				if (
					lifecycle_base.has_facility('Headquarters') ||
					!capital.has_facility('Headquarters')
				) {
					#print('RUNTIME_SMOKE_FAIL: Headquarters relocation did not roll back');
					glsmac.exit();
					return;
				}
				#print('RUNTIME_SMOKE_HEADQUARTERS_RELOCATION_PASS');

				const old_base_id = lifecycle_base.id;
				const founding_site_coords = find_founding_site_coords();
				if (founding_site_coords == null) {
					#print('RUNTIME_SMOKE_FAIL: no legal colony founding site is available');
					glsmac.exit();
					return;
				}
				const founding_site = game.get_tm().get_tile(
					founding_site_coords.x,
					founding_site_coords.y
				);
				game.event('runtime_smoke_replace_base', {
					base_id: old_base_id,
					owner: lifecycle_base.get_owner(),
					tile: lifecycle_base.get_tile(),
					name: lifecycle_base.name,
				});
				game.event('spawn_unit', {
					owner: game.get_player(),
					tile: founding_site,
					type: 'ColonyPod',
					health: 1.0,
					morale: 2,
				});

				let wait_ticks = 0;
				#async(100, () => {
					wait_ticks++;
					for (base of game.get_bm().get_bases()) {
						if (base.name == lifecycle_base_name && base.id != old_base_id) {
							#print('RUNTIME_SMOKE_BASE_LIFECYCLE_PASS');
							lifecycle_verified = true;
							advance_from_turn_three_if_ready();
							finish_if_ready();
							return false;
						}
					}
					if (wait_ticks >= 100) {
						#print('RUNTIME_SMOKE_FAIL: named base was not reusable after despawn');
						glsmac.exit();
						return false;
					}
					return true;
				});

				let founding_requested = false;
				let founding_wait_ticks = 0;
				let colony_pod_id = 0;
				#async(100, () => {
					founding_wait_ticks++;
					if (!founding_requested) {
						for (unit of founding_site.get_units()) {
							if (unit.owner == game.get_player().id && unit.def == 'ColonyPod') {
								const def = unit.get_def();
								if (
									def.is_native ||
									def.offense != 0 ||
									def.defense != 1 ||
									def.morale_set != 'STANDARD' ||
									!def.can_found_base
								) {
									#print('RUNTIME_SMOKE_FAIL: Colony Pod definition metadata is invalid');
									glsmac.exit();
									return false;
								}
								colony_pod_id = unit.id;
								founding_requested = true;
								game.event('found_base', {
									unit: unit,
									name: expansion_base_name,
								});
								break;
							}
						}
					}
					else {
						for (base of game.get_bm().get_bases()) {
							if (base.name == expansion_base_name) {
								const production = base.get_production();
								if (
									game.get_um().has_unit(colony_pod_id) ||
									base.get_owner().id != game.get_player().id ||
									base.get_tile() != founding_site ||
									#sizeof(base.get_pops()) != 1 ||
									#sizeof(base.get_worked_tiles()) != 1 ||
									base.get_accumulated_minerals() != 10 ||
									!#is_defined(production) ||
									production.id != 'ScoutPatrol'
								) {
									#print('RUNTIME_SMOKE_FAIL: founded base state is invalid');
									glsmac.exit();
									return false;
								}
								#print('RUNTIME_SMOKE_BASE_FOUNDING_PASS');
								expansion_verified = true;
								advance_from_turn_three_if_ready();
								finish_if_ready();
								return false;
							}
						}
					}
					if (founding_wait_ticks >= 100) {
						#print('RUNTIME_SMOKE_FAIL: colony founding timed out');
						glsmac.exit();
						return false;
					}
					return true;
				});
			}
			else if (turn_id == 4) {
				const former = game.get_um().get_unit(former_id);
				if (
					former.terraforming != 'farm' ||
					former.terraforming_turns_remaining != 1 ||
					terraform_site.terraforming.farm
				) {
					#print('RUNTIME_SMOKE_FAIL: Former order did not advance into turn 4');
					glsmac.exit();
					return;
				}
				game.event('complete_turn', {});
			}
			else if (turn_id == 5) {
				const former = game.get_um().get_unit(former_id);
				const updated_resources = terraform_site.get_resources();
				if (
					former.terraforming != 'none' ||
					former.terraforming_turns_remaining != 0 ||
					former.movement != former.get_def().movement_per_turn ||
					!terraform_site.terraforming.farm ||
					updated_resources.NUTRIENTS != terraform_site_resources.NUTRIENTS + 1 ||
					updated_resources.MINERALS != terraform_site_resources.MINERALS ||
					updated_resources.ENERGY != terraform_site_resources.ENERGY
				) {
					#print('RUNTIME_SMOKE_FAIL: Farm completion or resource yields are invalid');
					glsmac.exit();
					return;
				}
				game.select_tile(terraform_site);
				terraforming_verified = true;
				#print('RUNTIME_SMOKE_TERRAFORMING_PASS');
				finish_if_ready();
			}
		});
	});

	glsmac.run();

});
