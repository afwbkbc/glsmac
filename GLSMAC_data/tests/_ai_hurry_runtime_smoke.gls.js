#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let game = null;
	let ai_id = 0 - 1;
	let base_id = 0;
	let base_tile = null;
	let initial_credits = 200;
	let hurried = false;
	let setup_complete = false;
	let setup_wait_ticks = 0;
	let wait_ticks = 0;

	const fail = (message) => {
		#print('AI_HURRY_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const complete_human_turn = () => {
		if (!game.is_game_over() && !game.is_turn_complete(game.get_player().id)) {
			game.event('complete_turn', {});
		}
	};

	const finish_setup = () => {
		setup_wait_ticks++;
		const base = base_tile.get_base();
		if (base == null) {
			if (setup_wait_ticks >= 30) {
				fail('test base was not spawned');
				return false;
			}
			return true;
		}
		base_id = base.id;
		game.event('process_player_economy', {
			player: game.get_player(ai_id),
			energy_credits: initial_credits,
		});
		setup_complete = true;
		#async(500, complete_human_turn);
		#async(100, check_result);
		return false;
	};

	const check_result = () => {
		wait_ticks++;
		let base = null;
		for (candidate of game.get_bm().get_bases()) {
			if (candidate.id == base_id) {
				base = candidate;
				break;
			}
		}
		if (base == null || base.get_owner().id != ai_id) {
			fail('test base disappeared or changed owners');
			return false;
		}
		let garrisoned = false;
		for (unit of base.get_tile().get_units()) {
			if (unit.owner == ai_id && unit.get_def().offense > 0) {
				garrisoned = true;
				break;
			}
		}
		if (hurried && garrisoned && !base.get_tile().is_locked()) {
			#print('AI_HURRY_RUNTIME_PASS: AI spent energy to rush an emergency base defender');
			setup_complete = false;
			#async(2000, () => { glsmac.exit(); });
			return false;
		}
		if (wait_ticks >= 250) {
			fail(hurried ? 'hurried defender was not completed' : 'AI did not spend energy on the emergency defender');
			return false;
		}
		return true;
	};

	glsmac.on('configure_game', (e) => {
		game = e.game;

		game.on('economy_updated', (e) => {
			if (
				setup_complete &&
				game.get_player(ai_id).energy_credits < initial_credits
			) {
				hurried = true;
			}
		});

		game.on('start_ui', (e) => {
			let ai = null;
			for (player of game.get_players()) {
				if (player.type == 'ai') {
					ai = player;
					break;
				}
			}
			if (ai == null) {
				fail('computer player is missing');
				return;
			}
			ai_id = ai.id;
			const tm = game.get_tm();
			const bases = game.get_bm().get_bases();
			const units = game.get_um().get_units();
			for (let y = 0; y < tm.get_map_height(); y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const candidate = tm.get_tile(x, y);
					if (
						!candidate.is_land ||
						candidate.is_locked() ||
						candidate.get_base() != null ||
						#sizeof(candidate.get_units()) != 0
					) {
						continue;
					}
					let isolated = true;
					for (base of bases) {
						if (tm.get_distance(candidate, base.get_tile()) < 5) {
							isolated = false;
							break;
						}
					}
					if (isolated) {
						for (unit of units) {
							if (tm.get_distance(candidate, unit.get_tile()) < 5) {
								isolated = false;
								break;
							}
						}
					}
					if (isolated) {
						base_tile = candidate;
						break;
					}
				}
				if (base_tile != null) {
					break;
				}
			}
			if (base_tile == null) {
				fail('no isolated land tile is available for the test base');
				return;
			}
			let former_tile = null;
			for (candidate of base_tile.get_surrounding_tiles()) {
				if (
					candidate.is_land &&
					!candidate.is_locked() &&
					candidate.get_base() == null &&
					#sizeof(candidate.get_units()) == 0
				) {
					former_tile = candidate;
					break;
				}
			}
			if (former_tile == null) {
				fail('no legal tile is available for the test former');
				return;
			}
			game.event('spawn_base', {
				owner: ai,
				tile: base_tile,
				name: 'Hurry Test',
				production: 'ScoutPatrol',
			});
			game.event('spawn_unit', {
				owner: ai,
				tile: former_tile,
				type: 'Former',
				morale: 1,
				health: 1.0,
			});
			#async(100, finish_setup);
		});

		game.on('turn', (e) => {
			if (setup_complete) {
				#async(100, complete_human_turn);
			}
		});
	});

	glsmac.run();

});
