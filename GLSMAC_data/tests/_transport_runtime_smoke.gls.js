#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	const despawn_event = #include('../default/game/event/despawn_unit');
	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;
	let start_runtime = null;
	let runtime_started = false;

	const fail = (message) => {
		#print('TRANSPORT_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print('TRANSPORT_RUNTIME_PASS: validated capacity, embark, carrier movement, disembark, destruction, and rollback');
			#async(500, () => { glsmac.exit(); });
		}
	};

	const wait_for_arrival = (unit, tile, on_ready, attempts) => {
		if (unit.get_tile() == tile && !tile.is_locked()) {
			on_ready();
			return;
		}
		if (attempts >= 100) {
			fail('timed out waiting for unit movement and tile unlock');
			return;
		}
		#async(50, () => { wait_for_arrival(unit, tile, on_ready, attempts + 1); });
	};

	const start_if_ready = () => {
		if (ui_started && start_runtime != null && !runtime_started) {
			runtime_started = true;
			#async(500, () => { start_runtime(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		const find_transport_def = () => {
			for (def of game.get_um().get_unit_defs()) {
				if (def.chassis == 'Foil' && def.weapon == 'TroopTransport') {
					return def;
				}
			}
			return null;
		};

		const find_test_tiles = () => {
			const tm = game.get_tm();
			let result = null;
			for (let y = 0; y < tm.get_map_height() && result == null; y++) {
				for (let x = 0; x < tm.get_map_width() && result == null; x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const source_water = tm.get_tile(x, y);
					if (
						!source_water.is_water ||
						source_water.get_base() != null || #sizeof(source_water.get_units()) > 0
					) {
						continue;
					}
					for (destination_water of source_water.get_surrounding_tiles()) {
						if (
							(
								destination_water.x == source_water.x &&
								destination_water.y == source_water.y
							) ||
							!destination_water.is_water ||
							destination_water.get_base() != null ||
							#sizeof(destination_water.get_units()) > 0
						) {
							continue;
						}
						let unloading_land = null;
						for (candidate of destination_water.get_surrounding_tiles()) {
							if (
								candidate.is_land &&
								candidate.get_base() == null && #sizeof(candidate.get_units()) == 0
							) {
								unloading_land = candidate;
								break;
							}
						}
						if (unloading_land != null) {
							result = {
								source_water: source_water,
								destination_water: destination_water,
								unloading_land: unloading_land,
							};
							break;
						}
					}
				}
			}
			return result;
		};

		const validate_transport_destruction = (um, transport_def, tile) => {
			const doomed_carrier = um.spawn_unit({
				def: transport_def.id,
				owner: game.get_player(),
				tile: tile,
				morale: 2,
				health: 1.0,
			});
			const doomed_cargo = um.spawn_unit({
				def: 'ScoutPatrol',
				owner: game.get_player(),
				tile: tile,
				morale: 2,
				health: 1.0,
				transport_id: doomed_carrier.id,
			});
			const doomed_carrier_id = doomed_carrier.id;
			const doomed_cargo_id = doomed_cargo.id;
			let event = {
				game: {
					um: um,
					tm: game.get_tm(),
					get_player: (index) => { return game.get_player(index); },
				},
				caller: 0,
				data: {unit: doomed_carrier},
			};
			event.applied = despawn_event.apply(event);
			if (um.has_unit(doomed_carrier_id) || um.has_unit(doomed_cargo_id)) {
				fail('destroying a transport did not destroy its cargo');
				return false;
			}
			despawn_event.rollback(event);
			if (
				!um.has_unit(doomed_carrier_id) || !um.has_unit(doomed_cargo_id) ||
				um.get_unit(doomed_cargo_id).transport_id != doomed_carrier_id ||
				um.get_unit(doomed_cargo_id).get_tile() != tile
			) {
				fail('transport destruction rollback did not restore its cargo tree');
				return false;
			}
			const cleanup_event = {
				game: event.game,
				caller: 0,
				data: {unit: um.get_unit(doomed_carrier_id)},
			};
			cleanup_event.applied = despawn_event.apply(cleanup_event);
			return true;
		};

		game.on('start_ui', (e) => {
			ui_started = true;
			start_if_ready();
			finish_if_ready();
		});

		game.on('turn', (e) => {
			if (e.year - 2100 != 1) {
				fail('runtime test exceeded one turn');
				return;
			}
			const um = game.get_um();
			const player = game.get_player();
			const transport_def = find_transport_def();
			const tiles = find_test_tiles();
			if (transport_def == null || transport_def.cargo_capacity != 2) {
				fail('generated Fission Foil transport metadata is invalid');
				return;
			}
			if (tiles == null) {
				fail('could not find a coastal transport test route');
				return;
			}

			const carrier = um.spawn_unit({
				def: transport_def.id,
				owner: player,
				tile: tiles.source_water,
				morale: 2,
				health: 1.0,
			});
			const cargo_one = um.spawn_unit({
				def: 'ScoutPatrol',
				owner: player,
				tile: tiles.source_water,
				morale: 2,
				health: 1.0,
			});

			cargo_one.embark(carrier);
			{
				const fetched_transport = cargo_one.get_transport();
				if (
					cargo_one.transport_id != carrier.id || !cargo_one.is_embarked ||
					fetched_transport == null || fetched_transport.id != carrier.id ||
					#sizeof(carrier.get_cargo()) != 1 ||
					#sizeof(tiles.source_water.get_units()) != 1 ||
					#sizeof(tiles.source_water.get_units(true)) != 2
				) {
					#print(
						'TRANSPORT_BOARDING_DIAGNOSTIC: transport_id=' + #to_string(cargo_one.transport_id) +
						' carrier_id=' + #to_string(carrier.id) +
						' embarked=' + #to_string(cargo_one.is_embarked) +
						' fetched=' + (fetched_transport == null ? 'null' : #to_string(fetched_transport.id)) +
						' cargo=' + #to_string(#sizeof(carrier.get_cargo())) +
						' visible=' + #to_string(#sizeof(tiles.source_water.get_units())) +
						' all=' + #to_string(#sizeof(tiles.source_water.get_units(true)))
					);
					fail('boarding or embarked-unit map filtering is invalid');
					return;
				}

				const cargo_two = um.spawn_unit({
					def: 'ScoutPatrol',
					owner: player,
					tile: tiles.source_water,
					morale: 2,
					health: 1.0,
					transport_id: carrier.id,
				});
				let capacity_rejected = false;
				try {
					um.spawn_unit({
						def: 'ScoutPatrol',
						owner: player,
						tile: tiles.source_water,
						morale: 2,
						health: 1.0,
						transport_id: carrier.id,
					});
				} catch {
				:
					(error) => {
						capacity_rejected = true;
					}
				}
				if (
					!capacity_rejected || #sizeof(carrier.get_cargo()) != 2 ||
					#sizeof(tiles.source_water.get_units(true)) != 3
				) {
					fail('transport capacity was not enforced');
					return;
				}

				if (!validate_transport_destruction(um, transport_def, tiles.source_water)) {
					return;
				}

				start_runtime = () => {
					game.event('move_unit', {unit: carrier, tile: tiles.destination_water});
					wait_for_arrival(carrier, tiles.destination_water, () => {
						if (
							carrier.get_tile() != tiles.destination_water ||
							cargo_one.get_tile() != tiles.destination_water ||
							cargo_two.get_tile() != tiles.destination_water ||
							cargo_one.transport_id != carrier.id || cargo_two.transport_id != carrier.id
						) {
							fail('carrier movement did not move all cargo atomically');
							return;
						}

						game.event('move_unit', {unit: cargo_one, tile: tiles.unloading_land});
						wait_for_arrival(cargo_one, tiles.unloading_land, () => {
							if (
								cargo_one.transport_id != 0 || cargo_one.is_embarked ||
								cargo_one.get_tile() != tiles.unloading_land ||
								#sizeof(carrier.get_cargo()) != 1
							) {
								fail('cargo did not disembark onto adjacent land');
								return;
							}

							runtime_complete = true;
							finish_if_ready();
						}, 0);
					}, 0);
				};
				start_if_ready();
			}
		});
	});

	glsmac.run();

});
