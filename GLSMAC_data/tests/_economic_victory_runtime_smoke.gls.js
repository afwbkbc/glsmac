#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let runtime_started = false;
	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;
	let observed_bid = null;

	const fail = (message) => {
		#print('ECONOMIC_VICTORY_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print(
				'ECONOMIC_VICTORY_RUNTIME_PASS: installed assets, market event, base persistence, countdown, and terminal victory verified'
			);
			#async(500, () => { glsmac.exit(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;
		game.on('economic_victory_updated', (event) => {
			if (event.player.id != game.get_player().id) {
				return;
			}
			const state = game.get('f_economic_victory_get_state')(event.player);
			if (state != null) {
				observed_bid = {
					base_id: state.base.id,
					cost: state.cost,
					turn: state.turn,
					energy_credits: event.player.get_energy_credits(),
				};
			}
		});

		game.on('start_ui', (e) => {
			ui_started = true;
			finish_if_ready();
		});

		game.on('turn', (e) => {
			if (runtime_started || e.year - 2100 != 1) {
				return;
			}
			runtime_started = true;

			const player = game.get_player();
			let headquarters = null;
			for (base of game.get_bm().get_bases()) {
				if (base.get_owner().id == player.id && base.has_facility('Headquarters')) {
					headquarters = base;
					break;
				}
			}
			if (headquarters == null) {
				fail('quickstart player has no Headquarters');
				return;
			}

			const research = player.get_research_state();
			let technologies = [];
			let has_planetary_economics = false;
			for (id of research.technologies) {
				technologies :+id;
				if (id == 'PlanetaryEconomics') {
					has_planetary_economics = true;
				}
			}
			if (!has_planetary_economics) {
				technologies :+'PlanetaryEconomics';
			}
			player.set_research_state({
				technologies: technologies,
				target: research.target,
				progress: research.progress,
			});
			player.set_energy_credits(1000000000);

			const bm = game.get_bm();
			const tm = game.get_tm();
			let persistence_tile = null;
			for (let y = 0; y < tm.get_map_height(); y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const tile = tm.get_tile(x, y);
					if (tile.is_land && tile.get_base() == null && #sizeof(tile.get_units(true)) == 0) {
						persistence_tile = tile;
						break;
					}
				}
				if (persistence_tile != null) {
					break;
				}
			}
			if (persistence_tile == null) {
				fail('no empty land tile is available for base persistence check');
				return;
			}
			let persistence_base = bm.spawn_base(player, persistence_tile, {
				name: 'Economic Victory Persistence Probe',
				production: 'ScoutPatrol',
			});
			persistence_base.set('economic_victory_turn', 77);
			persistence_base.set('economic_victory_cost', 1234);
			const persistence_id = persistence_base.id;
			const snapshot = bm.snapshot_base(persistence_base);
			bm.despawn_base(persistence_id);
			persistence_base = bm.restore_base(snapshot);
			if (
				persistence_base.id != persistence_id ||
				!persistence_base.has('economic_victory_turn') ||
				!persistence_base.has('economic_victory_cost') ||
				persistence_base.get('economic_victory_turn') != 77 ||
				persistence_base.get('economic_victory_cost') != 1234
			) {
				fail('economic victory state did not survive native base serialization');
				return;
			}
			persistence_base.unset('economic_victory_turn');
			persistence_base.unset('economic_victory_cost');

			const expected_cost = game.get('f_economic_victory_get_cost')(player);
			game.event('corner_global_energy_market', {player: player});
			let wait_ticks = 0;
			let victory_requested = false;
			#async(100, () => {
				wait_ticks++;
				if (!victory_requested) {
					const state = game.get('f_economic_victory_get_state')(player);
					if (state == null || observed_bid == null) {
						if (wait_ticks >= 100) {
							fail('market event did not create an active bid');
							return false;
						}
						return true;
					}
					if (
						state.base != headquarters ||
						observed_bid.base_id != headquarters.id ||
						state.cost != expected_cost || observed_bid.cost != expected_cost ||
						state.turn != game.get_turn() + 20 ||
						observed_bid.turn != game.get_turn() + 20 ||
						observed_bid.energy_credits != 1000000000 - expected_cost
					) {
						fail('market event produced the wrong cost, target, deadline, or reserve balance');
						return false;
					}
					headquarters.set('economic_victory_turn', game.get_turn());
					game.get('f_check_economic_victory')();
					victory_requested = true;
					return true;
				}

				if (!game.is_game_over()) {
					if (wait_ticks >= 100) {
						fail('economic victory declaration timed out');
						return false;
					}
					return true;
				}
				const victory = game.get_victory_state();
				if (
					victory.type != 'economic' || victory.winner != player.id ||
					victory.turn != game.get_turn()
				) {
					fail('native terminal victory state does not identify the economic winner');
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
