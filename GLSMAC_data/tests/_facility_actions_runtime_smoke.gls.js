#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	const psi_gate_rules = #include('../default/game/psi_gate_rules');
	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;
	let start_runtime = null;
	let runtime_started = false;

	const fail = (message) => {
		#print('FACILITY_ACTIONS_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print(
				'FACILITY_ACTIONS_RUNTIME_PASS: installed assets, serialized Psi Gate capability, ' +
				'transport/cargo teleport, endpoint limits, and Alien Artifact contribution verified'
			);
			#async(500, () => { glsmac.exit(); });
		}
	};

	const start_if_ready = () => {
		if (ui_started && start_runtime != null && !runtime_started) {
			runtime_started = true;
			#async(500, () => { start_runtime(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('start_ui', (e) => {
			const psi_gate = game.get_bm().get_facility_def('PsiGate');
			const artifact = game.get_um().get_unit_def('AlienArtifact');
			if (
				psi_gate == null || !psi_gate.psi_gate || artifact == null ||
				artifact.buildable || artifact.weapon != 'AlienArtifact'
			) {
				fail('facility or special-unit capability did not survive native serialization');
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

			const bm = game.get_bm();
			const tm = game.get_tm();
			const um = game.get_um();
			const player = game.get_player();
			let source = null;
			for (base of bm.get_bases()) {
				if (base.get_owner().id == player.id) {
					source = base;
					break;
				}
			}
			if (source == null) {
				fail('quickstart player has no source base');
				return;
			}

			let destination_tile = null;
			for (let y = 0; y < tm.get_map_height() && destination_tile == null; y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const tile = tm.get_tile(x, y);
					if (
						tile.is_land && tile != source.get_tile() && tile.get_base() == null &&
						#sizeof(tile.get_units(true)) == 0 && !tile.is_locked()
					) {
						destination_tile = tile;
						break;
					}
				}
			}
			if (destination_tile == null) {
				fail('no empty land tile is available for the destination base');
				return;
			}

			const destination = bm.spawn_base(player, destination_tile, {
				name: 'Psi Gate Destination',
				production: 'ScoutPatrol',
			});
			source.add_facility('PsiGate');
			destination.add_facility('PsiGate');
			if (source.can_set_production('unit', 'AlienArtifact')) {
				fail('Alien Artifact was exposed as ordinary base production');
				return;
			}
			source.set_production('project', 'TheWeatherParadigm');
			source.set_accumulated_minerals(17);
			const artifact = um.spawn_unit({
				def: 'AlienArtifact',
				owner: player,
				tile: source.get_tile(),
				morale: 2,
				health: 1.0,
			});
			const artifact_id = artifact.id;

			let transport_def = null;
			for (def of um.get_unit_defs()) {
				if (def.chassis == 'Foil' && def.weapon == 'TroopTransport') {
					transport_def = def;
					break;
				}
			}
			if (transport_def == null || transport_def.cargo_capacity < 1) {
				fail('generated Foil transport definition is missing');
				return;
			}
			const carrier = um.spawn_unit({
				def: transport_def.id,
				owner: player,
				tile: source.get_tile(),
				morale: 2,
				health: 1.0,
			});
			const cargo = um.spawn_unit({
				def: 'ScoutPatrol',
				owner: player,
				tile: source.get_tile(),
				morale: 2,
				health: 1.0,
				transport_id: carrier.id,
			});
			const old_movement = carrier.movement;
			const old_moved = carrier.moved_this_turn;

			start_runtime = () => {
				const artifact_start_minerals = source.get_accumulated_minerals();
				game.event('contribute_alien_artifact', {unit: artifact});
				let artifact_wait_ticks = 0;
				#async(50, () => {
					artifact_wait_ticks++;
					if (
						source.get_accumulated_minerals() != artifact_start_minerals + 50 ||
						um.has_unit(artifact_id)
					) {
						if (artifact_wait_ticks >= 100) {
							fail('Alien Artifact contribution did not synchronize');
							return false;
						}
						return true;
					}

					game.event('teleport_unit', {unit: carrier, destination: destination});
					let teleport_wait_ticks = 0;
					#async(50, () => {
						teleport_wait_ticks++;
						if (
							carrier.get_tile() != destination_tile ||
							cargo.get_tile() != destination_tile
						) {
							if (teleport_wait_ticks >= 100) {
								fail('Psi Gate teleport did not synchronize');
								return false;
							}
							return true;
						}
						if (
							cargo.transport_id != carrier.id || carrier.movement != old_movement ||
							carrier.moved_this_turn != old_moved
						) {
							fail('Psi Gate spent movement or detached transported cargo');
							return false;
						}
						if (
							psi_gate_rules.get_base_used_turn(source) != game.get_turn() ||
							psi_gate_rules.get_base_used_turn(destination) != game.get_turn() ||
							!#is_defined(psi_gate_rules.get_teleport_error(
								game,
								carrier,
								player.id,
								source
							))
						) {
							fail('Psi Gate endpoint usage limit was not enforced');
							return false;
						}

						runtime_complete = true;
						finish_if_ready();
						return false;
					});
					return false;
				});
			};
			start_if_ready();
		});
	});

	glsmac.run();

});
