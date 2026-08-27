#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let runtime_started = false;
	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;

	const fail = (message) => {
		#print('PLANET_BUSTER_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print(
				'PLANET_BUSTER_RUNTIME_PASS: live sea-level rollback, base snapshot, launch, blast, sanctions, diplomacy, and support reassignment verified'
			);
			#async(2500, () => { glsmac.exit(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('start_ui', (e) => {
			ui_started = true;
			finish_if_ready();
		});

		game.on('turn', (e) => {
			if (runtime_started || e.year - 2100 != 1) {
				return;
			}
			runtime_started = true;

			const players = game.get_players();
			if (#sizeof(players) != 2) {
				fail('quickstart did not create exactly two factions');
				return;
			}
			const actor = game.get_player();
			let defender = null;
			let actor_base = null;
			let support_base = null;
			for (player of players) {
				if (player.id != actor.id) {
					defender = player;
					break;
				}
			}
			for (base of game.get_bm().get_bases()) {
				if (base.get_owner().id == actor.id && actor_base == null) {
					actor_base = base;
				} else if (
					defender != null && base.get_owner().id == defender.id &&
					support_base == null
				) {
					support_base = base;
				}
			}
			if (defender == null || actor_base == null || support_base == null) {
				fail('quickstart factions or bases are missing');
				return;
			}

			const research = actor.get_research_state();
			let technologies = [];
			let has_orbital_spaceflight = false;
			let has_self_aware_machines = false;
			for (technology_id of research.technologies) {
				technologies :+technology_id;
				if (technology_id == 'OrbitalSpaceflight') {
					has_orbital_spaceflight = true;
				} else if (technology_id == 'SelfAwareMachines') {
					has_self_aware_machines = true;
				}
			}
			if (!has_orbital_spaceflight) {
				technologies :+'OrbitalSpaceflight';
			}
			if (!has_self_aware_machines) {
				technologies :+'SelfAwareMachines';
			}
			actor.set_research_state({
				technologies: technologies,
				target: research.target,
				progress: research.progress,
			});
			if (!actor_base.has_facility('AerospaceComplex')) {
				actor_base.add_facility('AerospaceComplex');
			}

			let planet_buster_definition = null;
			for (definition of game.get_um().get_unit_defs()) {
				if (definition.weapon == 'PlanetBuster') {
					planet_buster_definition = definition;
					break;
				}
			}
			if (
				planet_buster_definition == null ||
				!planet_buster_definition.is_missile ||
				planet_buster_definition.offense != 99 ||
				planet_buster_definition.required_technology != 'OrbitalSpaceflight' ||
				!actor_base.can_set_production('unit', planet_buster_definition.id) ||
				!actor_base.can_set_production('facility', 'OrbitalDefensePod')
			) {
				fail('Planet Buster or Orbital Defense Pod is unavailable after its prerequisites');
				return;
			}

			defender.set_orbital_facility_count('OrbitalDefensePod', 2);
			defender.set_orbital_defense_deployments(1);
			if (
				defender.get_orbital_facility_count('OrbitalDefensePod') != 2 ||
				defender.get_orbital_defense_deployments() != 1
			) {
				fail('live orbital defense state wrappers did not preserve values');
				return;
			}
			defender.set_orbital_facility_count('OrbitalDefensePod', 0);
			defender.set_orbital_defense_deployments(0);

			const tm = game.get_tm();
			if (tm.get_sea_level() != 0) {
				fail('generated map did not start at the coast datum');
				return;
			}
			const initial_climate = tm.get_climate_state();
			tm.set_climate_state(11, 300, 19);
			const test_climate = tm.get_climate_state();
			if (
				test_climate.level != 11 || test_climate.future_change != 300 ||
				test_climate.progress != 19
			) {
				fail('live climate-state wrappers did not preserve values');
				return;
			}
			tm.set_climate_state(
				initial_climate.level,
				initial_climate.future_change,
				initial_climate.progress
			);
			let minimum_land_elevation = 100000;
			let maximum_water_elevation = -100000;
			let minimum_land_tile = null;
			let maximum_water_tile = null;
			for (let sea_y = 0; sea_y < tm.get_map_height(); sea_y++) {
				for (let sea_x = 0; sea_x < tm.get_map_width(); sea_x++) {
					if (sea_x % 2 == sea_y % 2) {
						const sea_tile = tm.get_tile(sea_x, sea_y);
						if (sea_tile.is_land) {
							if (sea_tile.elevation < minimum_land_elevation) {
								minimum_land_elevation = sea_tile.elevation;
								minimum_land_tile = sea_tile;
							}
						} else if (sea_tile.elevation > maximum_water_elevation) {
							maximum_water_elevation = sea_tile.elevation;
							maximum_water_tile = sea_tile;
						}
					}
				}
			}
			const raised_sea_snapshot = tm.apply_sea_level_change(3000);
			const flooded_tile = minimum_land_tile;
			if (tm.get_sea_level() != 3000) {
				fail('native sea-level value did not change');
				return;
			}
			if (flooded_tile == null) {
				fail(
					'raising sea level did not flood a live map tile; land floor=' +
					#to_string(minimum_land_elevation) + ', water ceiling=' +
					#to_string(maximum_water_elevation) + ', tile sea=' +
					#to_string(minimum_land_tile.sea_level) + ', tile water=' +
					#to_string(minimum_land_tile.is_water)
				);
				return;
			}
			if (flooded_tile.sea_level != 3000) {
				fail('flooded tile wrapper did not receive the new sea level');
				return;
			}
			tm.restore_sea_level(raised_sea_snapshot);
			if (tm.get_sea_level() != 0 || !flooded_tile.is_land || flooded_tile.sea_level != 0) {
				fail('sea-level rollback did not restore the flooded coastline');
				return;
			}
			const flooded_old_road = flooded_tile.terraforming.road;
			const flooded_old_farm = flooded_tile.terraforming.farm;
			flooded_tile.update_terraforming({road: true, farm: true});
			const cleanup_sea_snapshot = tm.apply_sea_level_change(3000);
			if (
				!flooded_tile.is_water || flooded_tile.terraforming.road ||
				flooded_tile.terraforming.farm
			) {
				fail('flooding did not clear incompatible land terraforming');
				return;
			}
			tm.restore_sea_level(cleanup_sea_snapshot);
			if (
				!flooded_tile.is_land || !flooded_tile.terraforming.road ||
				!flooded_tile.terraforming.farm
			) {
				fail('sea-level rollback did not restore terraforming');
				return;
			}
			flooded_tile.update_terraforming({
				road: flooded_old_road,
				farm: flooded_old_farm,
			});
			const lowered_sea_snapshot = tm.apply_sea_level_change(-3000);
			const exposed_tile = maximum_water_tile;
			if (
				tm.get_sea_level() != -3000 || exposed_tile == null ||
				exposed_tile.sea_level != -3000
			) {
				fail('lowering sea level did not expose a live map tile');
				return;
			}
			tm.restore_sea_level(lowered_sea_snapshot);
			if (tm.get_sea_level() != 0 || !exposed_tile.is_water || exposed_tile.sea_level != 0) {
				fail('sea-level rollback did not restore the exposed coastline');
				return;
			}

			let target_tile = null;
			let launch_tile = null;
			for (let y = 0; y < tm.get_map_height(); y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const candidate = tm.get_tile(x, y);
					if (
						!candidate.is_land || candidate.is_locked() ||
						candidate.get_base() != null || #sizeof(candidate.get_units(true)) != 0
					) {
						continue;
					}
					let candidate_launch = null;
					let blast_is_clear = true;
					for (nearby of candidate.get_surrounding_tiles()) {
						if (nearby == candidate) {
							continue;
						}
						if (nearby.get_base() != null || #sizeof(nearby.get_units(true)) != 0) {
							blast_is_clear = false;
							break;
						}
						if (!nearby.is_locked() && candidate_launch == null) {
							candidate_launch = nearby;
						}
					}
					if (blast_is_clear && candidate_launch != null) {
						target_tile = candidate;
						launch_tile = candidate_launch;
						break;
					}
				}
				if (target_tile != null) {
					break;
				}
			}
			if (target_tile == null || launch_tile == null) {
				fail('no isolated land target is available for the live blast');
				return;
			}
			target_tile.update_terraforming({road: true, farm: true});
			target_tile.update_features({xenofungus: true});
			const target_elevation_before = target_tile.elevation + 0;
			const target_was_land = target_tile.is_land == true;
			const terrain_snapshot = tm.apply_crater(target_tile, 1);
			if (
				target_tile.elevation >= target_elevation_before ||
				target_tile.terraforming.road || target_tile.terraforming.farm ||
				target_tile.features.xenofungus
			) {
				fail(
					'native crater mismatch: elevation=' + #to_string(target_tile.elevation) +
					' before=' + #to_string(target_elevation_before) +
					' road=' + #to_string(target_tile.terraforming.road) +
					' farm=' + #to_string(target_tile.terraforming.farm) +
					' fungus=' + #to_string(target_tile.features.xenofungus)
				);
				return;
			}
			tm.restore_terrain(terrain_snapshot);
			if (
				target_tile.elevation != target_elevation_before ||
				target_tile.is_land != target_was_land ||
				!target_tile.terraforming.road || !target_tile.terraforming.farm ||
				!target_tile.features.xenofungus
			) {
				fail('native terrain snapshot did not restore tile state and wrappers');
				return;
			}

			const bm = game.get_bm();
			let target_base = bm.spawn_base(defender, target_tile, {
				name: 'Planet Buster Runtime Target',
				production: 'ScoutPatrol',
			});
			target_base.add_facility('RecyclingTanks');
			target_base.set_accumulated_minerals(37);
			const target_base_id = target_base.id;
			const snapshot = bm.snapshot_base(target_base);
			bm.despawn_base(target_base_id);
			target_base = bm.restore_base(snapshot);
			const restored_queue = target_base.get_production_queue();
			const restored_tile = target_base.get_tile();
			if (
				target_base.id != target_base_id ||
				target_base.name != 'Planet Buster Runtime Target' ||
				target_base.get_owner().id != defender.id ||
				restored_tile.x != target_tile.x || restored_tile.y != target_tile.y ||
				!target_base.has_facility('RecyclingTanks') ||
				target_base.get_accumulated_minerals() != 37 ||
				#sizeof(restored_queue) != 1 || restored_queue[0].id != 'ScoutPatrol'
			) {
				fail('native base snapshot did not restore complete live state');
				return;
			}

			const survivor = game.get_um().spawn_unit({
				def: 'ScoutPatrol',
				owner: defender,
				tile: support_base.get_tile(),
				morale: 2,
				health: 1.0,
				home_base_id: target_base_id,
			});
			const blast_defender = game.get_um().spawn_unit({
				def: 'ScoutPatrol',
				owner: defender,
				tile: target_tile,
				morale: 2,
				health: 1.0,
				home_base_id: target_base_id,
			});
			const missile = game.get_um().spawn_unit({
				def: planet_buster_definition.id,
				owner: actor,
				tile: launch_tile,
				morale: 2,
				health: 1.0,
				home_base_id: actor_base.id,
			});
			const survivor_id = survivor.id;
			const blast_defender_id = blast_defender.id;
			const missile_id = missile.id;
			const atrocities_before = actor.get_major_atrocities();
			const sanctions_before = actor.get_sanction_turns();
			game.event('planet_buster', {unit: missile, tile: target_tile});

			let wait_ticks = 0;
			#async(100, () => {
				wait_ticks++;
				if (game.get_um().has_unit(missile_id)) {
					if (wait_ticks >= 100) {
						fail('Planet Buster event timed out');
						return false;
					}
					return true;
				}
				let target_survived = false;
				for (base of bm.get_bases()) {
					if (base.id == target_base_id) {
						target_survived = true;
						break;
					}
				}
				if (target_survived || game.get_um().has_unit(blast_defender_id)) {
					fail('live Planet Buster blast did not destroy its target');
					return false;
				}
				if (
					target_tile.elevation >= target_elevation_before ||
					target_tile.terraforming.road || target_tile.terraforming.farm ||
					target_tile.features.xenofungus
				) {
					fail('live Planet Buster blast did not leave a cleared crater');
					return false;
				}
				if (!game.get_um().has_unit(survivor_id)) {
					fail('live Planet Buster blast destroyed an out-of-range unit');
					return false;
				}
				const live_survivor = game.get_um().get_unit(survivor_id);
				if (live_survivor.home_base_id != support_base.id) {
					fail('surviving unit was not reassigned from the destroyed support base');
					return false;
				}
				const live_actor = game.get_player(actor.id);
				if (
					live_actor.get_major_atrocities() != atrocities_before + 1 ||
					live_actor.get_sanction_turns() != sanctions_before + 20 ||
					live_actor.get_diplomatic_relation(game.get_player(defender.id)) != 'vendetta'
				) {
					fail('live Planet Buster diplomatic consequences are invalid');
					return false;
				}
				runtime_complete = true;
				finish_if_ready();
				return false;
			});
		});
	});

	glsmac.run();

});
