#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;
	let start_runtime = null;
	let runtime_started = false;
	let expected_explorer_id = 0;
	let pod_events = 0;
	let pod_outcome = '';

	const fail = (message) => {
		#print('UNITY_POD_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print(
				'UNITY_POD_RUNTIME_PASS: installed assets, Unity reward definitions, ' +
				'live sea refresh, bonus mutation, earthquake rollback, and movement resolution verified'
			);
			#async(500, () => { glsmac.exit(); });
		}
	};

	const wait_for_pod_ready = (tile, on_ready, attempts) => {
		if (tile.features.unity_pod && !tile.is_locked()) {
			on_ready();
			return;
		}
		if (attempts >= 120) {
			fail('timed out waiting for synchronized Unity Pod preparation');
			return;
		}
		#async(50, () => { wait_for_pod_ready(tile, on_ready, attempts + 1); });
	};

	const wait_for_arrival = (unit, tile, on_ready, attempts) => {
		if (unit.get_tile() == tile && !tile.is_locked()) {
			on_ready();
			return;
		}
		if (attempts >= 120) {
			fail('timed out waiting for Unity Pod movement');
			return;
		}
		#async(50, () => { wait_for_arrival(unit, tile, on_ready, attempts + 1); });
	};

	const get_bonus_name = (tile) => {
		if (tile.bonuses.nutrient) { return 'nutrient'; }
		if (tile.bonuses.energy) { return 'energy'; }
		if (tile.bonuses.minerals) { return 'minerals'; }
		return 'none';
	};

	const is_supported_outcome = (outcome) => {
		for (candidate of [
			'energy', 'river', 'earthquake', 'production', 'artifact', 'fungus',
			'monolith', 'vehicle', 'technology', 'terraforming', 'clone', 'native',
			'resource'
		]) {
			if (candidate == outcome) {
				return true;
			}
		}
		return false;
	};

	const start_if_ready = () => {
		if (ui_started && start_runtime != null && !runtime_started) {
			runtime_started = true;
			#async(500, () => { start_runtime(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.register_event('unity_pod_runtime_prepare', {
			validate: (e) => {},
			apply: (e) => {
				const tm = e.game.get_tm();
				const source = e.data.source;
				const destination = e.data.destination;
				const sea_tile = e.data.sea_tile;
				const earthquake_tile = e.data.earthquake_tile;

				const old_bonus = get_bonus_name(source);
				source.set_bonus('minerals');
				if (!source.bonuses.minerals) {
					throw Error('UNITY_POD_RUNTIME_FAIL: resource bonus mutation was not reflected');
				}
				source.set_bonus(old_bonus);

				sea_tile.update_features({unity_pod: true});
				if (!sea_tile.features.unity_pod) {
					throw Error('UNITY_POD_RUNTIME_FAIL: live sea tile did not accept a Unity Pod');
				}
				sea_tile.update_features({unity_pod: false});
				if (sea_tile.features.unity_pod) {
					throw Error('UNITY_POD_RUNTIME_FAIL: live sea tile did not remove a Unity Pod');
				}

				const live_earthquake_tile = tm.get_tile(earthquake_tile.x, earthquake_tile.y);
				const old_elevation = live_earthquake_tile.elevation + 0;
				const snapshot = tm.apply_earthquake(live_earthquake_tile, 1);
				const raised_earthquake_tile = tm.get_tile(earthquake_tile.x, earthquake_tile.y);
				const raised_elevation = raised_earthquake_tile.elevation + 0;
				if (raised_elevation <= old_elevation) {
					fail(
						'native earthquake did not raise its center at ' +
						#to_string(earthquake_tile.x) + 'x' + #to_string(earthquake_tile.y) +
						': before=' + #to_string(old_elevation) +
						' after=' + #to_string(raised_elevation)
					);
					return {};
				}
				tm.restore_terrain(snapshot);
				const restored_earthquake_tile = tm.get_tile(earthquake_tile.x, earthquake_tile.y);
				if (restored_earthquake_tile.elevation + 0 != old_elevation) {
					throw Error('UNITY_POD_RUNTIME_FAIL: earthquake snapshot did not restore terrain');
				}

				destination.update_features({unity_pod: true});
				return {};
			},
			rollback: (e) => {
				throw Error('UNITY_POD_RUNTIME_FAIL: preparation event was rolled back');
			},
		});

		game.on('unity_pod_opened', (opened) => {
			pod_events++;
			pod_outcome = opened.outcome;
			if (
			expected_explorer_id == 0 || opened.unit.id != expected_explorer_id ||
			opened.player.id != game.get_player().id
			) {
				fail('Unity Pod notification identified the wrong explorer or player');
			}
		});

		game.on('start_ui', (e) => {
			const um = game.get_um();
			const rover = um.get_unit_def('UnityRover');
			const chopper = um.get_unit_def('UnityScoutChopper');
			const foil = um.get_unit_def('UnityFoil');
			const isle = um.get_unit_def('IsleOfTheDeep');
			if (
				rover == null || rover.buildable || rover.movement_per_turn != 2.0 ||
				chopper == null || chopper.buildable || chopper.movement_per_turn != 8.0 ||
				chopper.operational_range != 1 ||
				foil == null || foil.buildable || foil.cargo_capacity != 2 ||
				isle == null || !isle.is_native || isle.cargo_capacity != 4
			) {
				fail('Unity reward unit metadata did not survive native serialization');
				return;
			}
			ui_started = true;
			start_if_ready();
			finish_if_ready();
		});

		game.on('turn', (e) => {
			if (e.year - 2100 != 1) {
				fail('runtime test exceeded one turn');
				return;
			}
			if (start_runtime != null) {
				return;
			}

			const tm = game.get_tm();
			const um = game.get_um();
			let route = null;
			let sea_tile = null;
			let earthquake_tile = null;
			for (let y = 0; y < tm.get_map_height(); y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const tile = tm.get_tile(x, y);
					if (
						sea_tile == null && tile.is_water && tile.get_base() == null &&
						#sizeof(tile.get_units(true)) == 0
					) {
						sea_tile = tile;
					}
					if (tile.is_land && tile.get_base() == null && #sizeof(tile.get_units(true)) == 0) {
						if (route == null) {
							for (destination of tile.get_surrounding_tiles()) {
								if (
									destination.is_land && destination.get_base() == null &&
									#sizeof(destination.get_units(true)) == 0
								) {
									route = {source: tile, destination: destination};
									break;
								}
							}
						}
						if (earthquake_tile == null && tile.elevation <= 1000) {
							let inland = true;
							let neighbours = 0;
							for (nearby of tile.get_surrounding_tiles()) {
								neighbours++;
								if (nearby.is_water) {
									inland = false;
								}
							}
							if (inland && neighbours == 8) {
								earthquake_tile = tile;
							}
						}
					}
				}
			}
			if (route == null || sea_tile == null || earthquake_tile == null) {
				fail('quickstart map lacks the required land route, sea tile, or inland earthquake tile');
				return;
			}
			if (
				#typeof(route.source.set_bonus) != 'Callable' ||
				#typeof(tm.apply_earthquake) != 'Callable' ||
				#typeof(tm.restore_terrain) != 'Callable'
			) {
				fail('Unity Pod native map bindings are unavailable');
				return;
			}

			const explorer = um.spawn_unit({
				def: 'ScoutPatrol',
				owner: game.get_player(),
				tile: route.source,
				morale: 2,
				health: 1.0,
			});
			explorer.movement = 10.0;
			explorer.moved_this_turn = false;
			expected_explorer_id = explorer.id;

			start_runtime = () => {
				game.event('unity_pod_runtime_prepare', {
					source: route.source,
					destination: route.destination,
					sea_tile: sea_tile,
					earthquake_tile: earthquake_tile,
				});
				wait_for_pod_ready(route.destination, () => {
					game.event('move_unit', {unit: explorer, tile: route.destination});
					wait_for_arrival(explorer, route.destination, () => {
						if (
							route.destination.features.unity_pod || pod_events != 1 ||
							!is_supported_outcome(pod_outcome)
						) {
							fail('real movement did not resolve exactly one supported Unity Pod outcome');
							return;
						}
						runtime_complete = true;
						finish_if_ready();
					}, 0);
				}, 0);
			};
			start_if_ready();
		});
	});

	glsmac.run();

});
