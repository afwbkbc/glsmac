const action_state = #include('./ai/action_state');
const strategy = #include('./native_strategy');

const ACTION_DELAY = 200;

const get_native_units = (game, player) => {
	let result = [];
	for (unit of game.get_um().get_units()) {
		if (unit.owner == player.id) {
			result :+unit;
		}
	}
	return result;
};

const play_turn = (game, player, done) => {
	strategy.queue_ambient_spawn(game);
	let steps = 0;
	let action_attempts = {};
	const play_next_action = () => {
		if (!game.is_master() || game.is_game_over() || game.is_turn_complete(player.id)) {
			done();
			return;
		}
		const units = get_native_units(game, player);
		const waiting_for_action = action_state.refresh_pending_actions(units, action_attempts);
		let waiting_for_animation = false;
		for (unit of units) {
			if (action_state.has_nearby_animation(unit)) {
				waiting_for_animation = true;
				break;
			}
		}
		let action_started = false;
		if (!waiting_for_action && !waiting_for_animation) {
			for (unit of units) {
				if (
					!action_state.can_attempt_action(unit, action_attempts) ||
					(#is_defined(unit.transport_id) && unit.transport_id > 0)
				) {
					continue;
				}
				const action = strategy.choose_action(game, unit);
				if (action == null) {
					continue;
				}
				if (action.kind == 'attack') {
					if (!game.get_um().has_unit(action.defender_id)) {
						continue;
					}
					game.event_as(player.id, 'attack_unit', {
						attacker: unit,
						defender: game.get_um().get_unit(action.defender_id),
					});
				} else {
					game.event_as(player.id, 'move_unit', {
						unit: unit,
						tile: game.get_tm().get_tile(action.tile_x, action.tile_y),
					});
				}
				action_state.record_action_attempt(unit, action_attempts);
				action_started = true;
				break;
			}
		}
		steps++;
		if (
			(action_started || waiting_for_action || waiting_for_animation) &&
			steps < 1000
		) {
			#async(ACTION_DELAY, play_next_action);
			return;
		}
		game.event_as(player.id, 'complete_turn', {});
		#async(100, done);
	};
	#async(100, play_next_action);
};

return (game) => {
	game.on('start', (e) => {
		let ui_started = false;
		let native_running = false;
		const play_native = () => {
			if (native_running || !game.is_master() || game.is_game_over()) {
				return;
			}
			const player = game.get_native_player();
			if (game.is_turn_complete(player.id)) {
				return;
			}
			native_running = true;
			play_turn(game, player, () => { native_running = false; });
		};
		game.on('start_ui', (e) => {
			ui_started = true;
			#async(100, play_native);
		});
		game.on('turn', (e) => {
			if (ui_started) {
				#async(100, play_native);
			}
		});
	});
};
