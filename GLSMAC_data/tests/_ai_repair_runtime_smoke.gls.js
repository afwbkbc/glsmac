#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let game = null;
	let injured_id = 0;
	let ai_id = 0 - 1;
	let home_tile = null;
	let staging_tile = null;
	let arrived_home = false;
	let setup_complete = false;
	let setup_wait_ticks = 0;
	let wait_ticks = 0;

	const fail = (message) => {
		#print('AI_REPAIR_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const complete_human_turn = () => {
		if (!game.is_game_over() && !game.is_turn_complete(game.get_player().id)) {
			game.event('complete_turn', {});
		}
	};

	const finish_setup = () => {
		setup_wait_ticks++;
		for (unit of staging_tile.get_units()) {
			if (unit.owner == ai_id && unit.health < 0.4) {
				injured_id = unit.id;
				break;
			}
		}
		if (injured_id == 0) {
			if (setup_wait_ticks >= 20) {
				fail('injured unit was not spawned');
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
		if (!game.get_um().has_unit(injured_id)) {
			fail('injured unit disappeared before it recovered');
			return false;
		}
		const injured = game.get_um().get_unit(injured_id);
		if (!arrived_home && injured.get_tile() == home_tile) {
			arrived_home = true;
			if (injured.health >= 0.5) {
				fail('injured unit did not retreat while critically damaged');
				return false;
			}
		}
		if (arrived_home) {
			if (injured.health < 0.8 && injured.get_tile() != home_tile) {
				fail('injured unit left its base before recovering');
				return false;
			}
			if (injured.health >= 0.799) {
				#print('AI_REPAIR_RUNTIME_PASS: wounded AI unit retreated, rested, and recovered in a friendly base');
				#async(500, () => { glsmac.exit(); });
				return false;
			}
		}
		if (wait_ticks >= 300) {
			fail(arrived_home ? 'injured unit did not recover within thirty seconds' : 'injured unit did not retreat within thirty seconds');
			return false;
		}
		return true;
	};

	glsmac.on('configure_game', (e) => {
		game = e.game;

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

			let home_base = null;
			for (base of game.get_bm().get_bases()) {
				if (base.get_owner().id == ai.id) {
					home_base = base;
					break;
				}
			}
			if (home_base == null) {
				fail('computer player base is missing');
				return;
			}
			home_tile = home_base.get_tile();

			for (candidate of home_tile.get_surrounding_tiles()) {
				if (candidate.is_land && !candidate.is_locked() && candidate.get_base() == null && #sizeof(candidate.get_units()) == 0) {
					staging_tile = candidate;
					break;
				}
			}
			if (staging_tile == null) {
				fail('no legal retreat staging tile is available');
				return;
			}

			game.event('spawn_unit', {
				owner: ai,
				tile: staging_tile,
				type: 'ScoutPatrol',
				morale: 1,
				health: 0.3,
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
