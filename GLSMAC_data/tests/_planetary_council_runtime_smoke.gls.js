#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let started = false;
	let finished = false;
	let ui_started = false;
	let vote_requested = false;
	let observed_session = null;

	const fail = (message) => {
		if (!finished) {
			finished = true;
			#print('PLANETARY_COUNCIL_RUNTIME_FAIL: ' + message);
			glsmac.exit();
		}
	};

	const finish_if_ready = () => {
		if (finished && ui_started) {
			#print(
				'PLANETARY_COUNCIL_RUNTIME_PASS: installed assets, native Council state, AI ballot, and diplomatic victory verified'
			);
			#async(250, () => { glsmac.exit(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('council_updated', (event) => {
			if (finished || vote_requested) { return; }
			const get_session = game.get('f_council_get_session');
			if (!#is_defined(get_session)) { return; }
			const session = get_session();
			if (session == null) { return; }
			const player = game.get_player();
			if (
				session.candidate_a_id != player.id &&
				session.candidate_b_id != player.id
			) {
				fail('quickstart player was not an election candidate');
				return;
			}
			const state = player.get_council_state();
			if (
				state.proposal != 'supreme' || state.caller_id != player.id ||
				state.last_session_turn != game.get_turn() || state.vote_id != -2 ||
				state.global_trade_pact || state.unity_core_salvaged ||
				state.un_charter_repealed
			) {
				fail('native player wrapper exposed invalid active Council state');
				return;
			}
			observed_session = #clone(state);
			vote_requested = true;
			game.event('cast_council_vote', {player: player, vote_id: player.id});
		});

		game.on('start_ui', (e) => {
			ui_started = true;
			finish_if_ready();
		});

		game.on('turn', (e) => {
			if (started || e.year - 2100 != 1) { return; }
			started = true;
			const player = game.get_player();
			if (player.get_faction().id != 'PEACEKEEPERS') {
				fail('runtime did not start as the Peacekeepers');
				return;
			}

			const research = player.get_research_state();
			let technologies = [];
			for (id of research.technologies) { technologies :+id; }
			if (!player.has_technology('MindMachineInterface')) {
				technologies :+'MindMachineInterface';
			}
			player.set_research_state({
				technologies: technologies,
				target: research.target == 'MindMachineInterface' ? '' : research.target,
				progress: research.target == 'MindMachineInterface' ? 0 : research.progress,
			});

			const bm = game.get_bm();
			const tm = game.get_tm();
			let added_bases = 0;
			for (let y = 0; y < tm.get_map_height() && added_bases < 4; y++) {
				for (let x = 0; x < tm.get_map_width() && added_bases < 4; x++) {
					if (x % 2 != y % 2) { continue; }
					const tile = tm.get_tile(x, y);
					if (
						tile.is_land && tile.get_base() == null &&
						#sizeof(tile.get_units(true)) == 0
					) {
						bm.spawn_base(player, tile, {
							name: 'Council Runtime Base ' + #to_string(added_bases + 1),
							production: 'ScoutPatrol',
						});
						added_bases++;
					}
				}
			}
			if (added_bases != 4) {
				fail('could not create deterministic Council voting population');
				return;
			}

			let population_ticks = 0;
			#async(50, () => {
				population_ticks++;
				const get_voters = game.get('f_council_get_voters');
				if (
					game.get('f_council_get_votes')(player) <= 0 ||
					#sizeof(get_voters()) < 2
				) {
					if (population_ticks >= 400) {
						fail('initial Council population events timed out');
						return false;
					}
					return true;
				}
				const error = game.get('f_council_validate_call')(player, 'supreme');
				if (#is_defined(error)) {
					fail('Supreme Leader call was not eligible: ' + error);
					return false;
				}
				game.event('call_planetary_council', {
					player: player,
					proposal: 'supreme',
				});

				let victory_ticks = 0;
				#async(50, () => {
					victory_ticks++;
					if (!game.is_game_over()) {
						if (victory_ticks >= 200) {
							fail('diplomatic victory resolution timed out');
							return false;
						}
						return true;
					}
					const victory = game.get_victory_state();
					const state = player.get_council_state();
					if (
						observed_session == null || !vote_requested ||
						victory.type != 'diplomatic' || victory.winner != player.id ||
						victory.turn != game.get_turn() || state.proposal != '' ||
						state.last_session_turn != game.get_turn() || state.vote_id != -2
					) {
						fail('terminal diplomatic victory or cleared Council state is invalid');
						return false;
					}
					finished = true;
					finish_if_ready();
					return false;
				});
				return false;
			});
		});
	});

	glsmac.run();

});
