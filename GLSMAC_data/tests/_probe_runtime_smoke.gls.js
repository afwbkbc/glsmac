#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let finished = false;
	let ui_started = false;
	let start_runtime = null;
	let runtime_started = false;
	let operation_notified = false;
	let runtime_probe_id = 0;
	let probe_morale_before_operation = 0;
	let probe_morale_at_notification = 0;

	const fail = (message) => {
		if (!finished) {
			finished = true;
			#print('PROBE_RUNTIME_FAIL: ' + message);
			glsmac.exit();
		}
	};

	const start_if_ready = () => {
		if (ui_started && start_runtime != null && !runtime_started) {
			runtime_started = true;
			#async(250, () => { start_runtime(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('probe_operation', (e) => {
			operation_notified = e.operation == 'subvert_unit' && e.success && e.detected;
			if (runtime_probe_id != 0 && game.get_um().has_unit(runtime_probe_id)) {
				probe_morale_at_notification = game.get_um().get_unit(runtime_probe_id).morale;
			}
		});

		game.on('start_ui', (e) => {
			ui_started = true;
			start_if_ready();
		});

		game.on('turn', (e) => {
			if (e.year - 2100 != 1) {
				fail('runtime test exceeded one turn');
				return;
			}
			const actor = game.get_player();
			let target_player = null;
			for (candidate of game.get_players()) {
				if (candidate.id != actor.id) {
					target_player = candidate;
					break;
				}
			}
			if (target_player == null) {
				fail('quickstart did not create an opponent');
				return;
			}
			const operations = game.get('f_probe_get_operations')();
			if (
				!#is_defined(operations.incite_drone_riots) ||
				!#is_defined(operations.assassinate_researchers) ||
				!#is_defined(operations.genetic_plague)
			) {
				fail('advanced base-game probe operations are unavailable');
				return;
			}
			const atrocities_before = actor.get_major_atrocities();
			actor.set_major_atrocities(atrocities_before + 1);
			if (actor.get_major_atrocities() != atrocities_before + 1) {
				fail('major atrocity state did not update through the live player wrapper');
				return;
			}
			actor.set_major_atrocities(atrocities_before);

			let actor_base = null;
			let target_base = null;
			for (base of game.get_bm().get_bases()) {
				if (base.get_owner().id == actor.id && actor_base == null) {
					actor_base = base;
				} else if (base.get_owner().id == target_player.id && target_base == null) {
					target_base = base;
				}
			}
			if (actor_base == null || target_base == null) {
				fail('quickstart bases are missing');
				return;
			}

			let probe_tile = null;
			for (tile of target_base.get_tile().get_surrounding_tiles()) {
				if (
					tile != target_base.get_tile() && tile.is_land &&
					tile.get_base() == null && #sizeof(tile.get_units()) == 0 && !tile.is_locked()
				) {
					probe_tile = tile;
					break;
				}
			}
			if (probe_tile == null) {
				fail('opponent base has no adjacent land probe tile');
				return;
			}

			let probe_definition = null;
			for (definition of game.get_um().get_unit_defs()) {
				if (definition.id == 'ProbeTeam') {
					probe_definition = definition;
					break;
				}
			}
			const research = actor.get_research_state();
			let technologies = [];
			for (technology_id of research.technologies) {
				technologies :+technology_id;
			}
			technologies :+'PlanetaryNetworks';
			actor.set_research_state({
				technologies: technologies, target: research.target, progress: research.progress,
			});
			if (
				probe_definition == null || probe_definition.weapon != 'ProbeTeam' ||
				!actor_base.can_set_production('unit', 'ProbeTeam')
			) {
				fail('buildable Probe Team definition is unavailable');
				return;
			}

			const probe = game.get_um().spawn_unit({
				def: 'ProbeTeam', owner: actor, tile: probe_tile, morale: 2, health: 1.0,
			});
			const target = game.get_um().spawn_unit({
				def: 'ScoutPatrol', owner: target_player, tile: target_base.get_tile(),
				morale: 2, health: 1.0, home_base_id: target_base.id,
			});
			runtime_probe_id = probe.id;
			const target_id = target.id;
			let probe_morale_expected = 0;
			actor.set_energy_credits(10000);
			target_player.set_energy_credits(200);
			const energy_before = game.get_player(actor.id).energy_credits;
			const expected_cost = game.get('f_probe_get_subversion_cost')(actor, target);
			if (expected_cost == null || expected_cost <= 0 || expected_cost > energy_before) {
				fail(
					'live subversion cost is invalid: cost=' +
						(expected_cost == null ? 'null' : #to_string(expected_cost)) +
						' target_energy=' + #to_string(
							game.get_player(target_player.id).energy_credits
						) +
						' target_probe=' + #to_string(
							game.get('f_probe_get_effective_rating')(target_player)
						)
				);
				return;
			}

			start_runtime = () => {
				probe_morale_before_operation = probe.morale;
				probe_morale_expected = #min(
					probe_morale_before_operation + 1,
					#sizeof(game.get_um().get_moraleset(probe.get_def().morale_set)) - 1
				);
				game.event('probe_operation', {
					unit: probe,
					operation: 'subvert_unit',
					target: target,
				});
				let ticks = 0;
				#async(50, () => {
					ticks++;
					const transferred = game.get_um().has_unit(target_id)
						? game.get_um().get_unit(target_id)
						: null;
					if (transferred == null || transferred.owner != actor.id) {
						if (ticks >= 100) {
							fail('unit subversion did not complete');
							return false;
						}
						return true;
					}
					if (
						game.get_player(actor.id).energy_credits != energy_before - expected_cost ||
						probe.movement != 0.0 ||
						probe_morale_at_notification != probe_morale_expected ||
						actor.get_diplomatic_relation(target_player) != 'vendetta' ||
						target_player.get_diplomatic_relation(actor) != 'vendetta' ||
						!operation_notified
					) {
						fail(
							'live subversion side effects are invalid: energy=' +
								#to_string(game.get_player(actor.id).energy_credits) +
							' expected=' + #to_string(energy_before - expected_cost) +
							' movement=' + #to_string(probe.movement) +
							' morale=' + #to_string(probe.morale) +
							' expected_morale=' + #to_string(probe_morale_expected) +
							' morale_before=' + #to_string(probe_morale_before_operation) +
							' morale_at_notification=' + #to_string(probe_morale_at_notification) +
							' actor_relation=' + actor.get_diplomatic_relation(target_player) +
							' target_relation=' + target_player.get_diplomatic_relation(actor) +
							' notified=' + #to_string(operation_notified)
						);
						return false;
					}
					finished = true;
					#print(
						'PROBE_RUNTIME_PASS: validated probe catalog, atrocity state, build gate, cost, subversion, promotion, movement, diplomacy, and notification'
					);
					#async(2500, () => { glsmac.exit(); });
					return false;
				});
			};
			start_if_ready();
		});
	});

	glsmac.run();

});
