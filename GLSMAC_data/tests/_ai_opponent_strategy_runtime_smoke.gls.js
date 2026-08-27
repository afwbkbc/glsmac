#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let game = null;
	let ai_id = 0 - 1;
	let ai_base = null;
	let wait_ticks = 0;
	let military_selected = false;

	const fail = (message) => {
		#print('AI_OPPONENT_STRATEGY_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const find_ai = () => {
		for (player of game.get_players()) {
			if (player.type == 'ai') {
				return player;
			}
		}
		return null;
	};

	const find_owned_base = (player_id) => {
		for (base of game.get_bm().get_bases()) {
			if (base.get_owner().id == player_id) {
				return base;
			}
		}
		return null;
	};

	const find_rival_tile = () => {
		const base = find_owned_base(game.get_player().id);
		return base == null ? null : base.get_tile();
	};

	const map_is_locked = () => {
		const tm = game.get_tm();
		for (let y = 0; y < tm.get_map_height(); y++) {
			for (let x = 0; x < tm.get_map_width(); x++) {
				if (x % 2 == y % 2 && tm.get_tile(x, y).is_locked()) {
					return true;
				}
			}
		}
		return false;
	};

	const check_result = () => {
		wait_ticks++;
		const queue = ai_base.get_production_queue();
		if (#sizeof(queue) > 0 && queue[0].production_kind == 'unit') {
			const def = game.get_um().get_unit_def(queue[0].id);
			if (def.offense > 0) {
				military_selected = true;
			}
		}
		if (
			military_selected &&
			game.is_turn_complete(ai_id) &&
			!map_is_locked()
		) {
			#print('AI_OPPONENT_STRATEGY_RUNTIME_PASS: outmatched AI selected military production against a distant stronger rival');
			#async(2000, () => { glsmac.exit(); });
			return false;
		}
		if (wait_ticks >= 200) {
			if (military_selected) {
				fail('AI turn or map did not settle after selecting military production');
			} else {
				fail('outmatched AI did not select military production');
			}
			return false;
		}
		return true;
	};

	const setup_scenario = () => {
		const ai = find_ai();
		if (ai == null) {
			fail('computer player is missing');
			return;
		}
		ai_id = ai.id;
		ai_base = find_owned_base(ai_id);
		if (ai_base == null) {
			fail('computer base is missing');
			return;
		}
		const rival_tile = find_rival_tile();
		if (rival_tile == null) {
			fail('human base is missing');
			return;
		}
		if (game.get_tm().get_distance(ai_base.get_tile(), rival_tile) <= 2) {
			fail('starting bases are too close to isolate global rival pressure');
			return;
		}
		game.event('spawn_unit', {
			owner: ai,
			tile: ai_base.get_tile(),
			type: 'ScoutPatrol',
			morale: 1,
			health: 1.0,
		});
		for (let i = 0; i < 4; i++) {
			game.event('spawn_unit', {
				owner: game.get_player(),
				tile: rival_tile,
				type: 'LaserInfantry',
				morale: 2,
				health: 1.0,
			});
		}
		#async(100, check_result);
	};

	glsmac.on('configure_game', (e) => {
		game = e.game;
		game.on('start_ui', (e) => {
			setup_scenario();
		});
	});

	glsmac.run();

});
