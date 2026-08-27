#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let game = null;
	let ai_id = 0 - 1;
	let reinforcement_base_id = 0;
	let rover_id = 0;
	let reinforcement_tile = null;
	let staging_tile = null;
	let setup_complete = false;
	let setup_wait_ticks = 0;
	let wait_ticks = 0;

	const fail = (message) => {
		#print('AI_REINFORCEMENT_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const complete_human_turn = () => {
		if (!game.is_game_over() && !game.is_turn_complete(game.get_player().id)) {
			game.event('complete_turn', {});
		}
	};

	const finish_setup = () => {
		setup_wait_ticks++;
		const base = reinforcement_tile.get_base();
		if (base != null && base.get_owner().id == ai_id) {
			reinforcement_base_id = base.id;
		}
		for (unit of staging_tile.get_units()) {
			if (unit.owner == ai_id && unit.get_def().id == 'ReconRover') {
				rover_id = unit.id;
				break;
			}
		}
		if (reinforcement_base_id == 0 || rover_id == 0) {
			if (setup_wait_ticks >= 30) {
				fail('test base or rover was not spawned');
				return false;
			}
			return true;
		}
		setup_complete = true;
		#async(100, complete_human_turn);
		#async(100, check_result);
		return false;
	};

	const check_result = () => {
		wait_ticks++;
		if (!game.get_um().has_unit(rover_id)) {
			fail('reinforcement rover disappeared before reaching the base');
			return false;
		}
		let reinforcement_base = null;
		for (base of game.get_bm().get_bases()) {
			if (base.id == reinforcement_base_id) {
				reinforcement_base = base;
				break;
			}
		}
		if (reinforcement_base == null || reinforcement_base.get_owner().id != ai_id) {
			fail('reinforcement base disappeared or changed owners');
			return false;
		}
		const rover = game.get_um().get_unit(rover_id);
		if (rover.get_tile() == reinforcement_tile) {
			if (reinforcement_tile.is_locked()) {
				return true;
			}
			#print('AI_REINFORCEMENT_RUNTIME_PASS: field rover routed to an under-defended friendly base');
			#async(2000, () => { glsmac.exit(); });
			return false;
		}
		if (wait_ticks >= 300) {
			fail('field rover did not reinforce the empty base within thirty seconds');
			return false;
		}
		return true;
	};

	glsmac.on('configure_game', (e) => {
		game = e.game;
		game.on('configure', (e) => {
			game.on('start', (e) => {
				game.set('f_social_get_new_base_minerals', (player) => { return 0; });
			});
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
					if (!isolated) {
						continue;
					}
					for (first_step of candidate.get_surrounding_tiles()) {
						if (!first_step.is_land || first_step.is_locked()) {
							continue;
						}
						for (second_step of first_step.get_surrounding_tiles()) {
							if (
								tm.get_distance(candidate, second_step) == 2 &&
								second_step.is_land &&
								!second_step.is_locked() &&
								second_step.get_base() == null &&
								#sizeof(second_step.get_units()) == 0
							) {
								reinforcement_tile = candidate;
								staging_tile = second_step;
								break;
							}
						}
						if (staging_tile != null) {
							break;
						}
					}
					if (staging_tile != null) {
						break;
					}
				}
				if (staging_tile != null) {
					break;
				}
			}
			if (reinforcement_tile == null || staging_tile == null) {
				fail('no isolated connected land tiles are available for the test');
				return;
			}

			game.event('spawn_base', {
				owner: ai,
				tile: reinforcement_tile,
				name: 'Reinforcement Test',
				production: 'ScoutPatrol',
			});
			game.event('spawn_unit', {
				owner: ai,
				tile: staging_tile,
				type: 'ReconRover',
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
