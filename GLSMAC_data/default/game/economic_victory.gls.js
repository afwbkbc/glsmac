const rules = #include('economic_victory_rules');

return (game) => {
	game.on('start', (e) => {
		let victory_request_pending = false;
		const check_victory = () => {
			if (
				!game.is_master() || game.is_game_over() || victory_request_pending ||
				game.get_turn() == 0
			) {
				return;
			}
			const winner = rules.get_winner(game);
			if (winner != null) {
				victory_request_pending = true;
				game.event('declare_victory', {
					type: 'economic',
					winner_id: winner.id,
				});
			}
		};

		game.set('f_economic_victory_get_headquarters', (player) => {
			return rules.get_headquarters(game, player);
		});
		game.set('f_economic_victory_get_state', (player) => {
			return rules.get_state(game, player);
		});
		game.set('f_economic_victory_get_cost', (player) => {
			return rules.get_cost(game, player);
		});
		game.set('f_economic_victory_get_winner', () => {
			return rules.get_winner(game);
		});
		game.set('f_check_economic_victory', check_victory);
		game.get_bm().on('base_despawn', (event) => {
			if (
				event.base.has_facility('Headquarters') &&
				rules.get_base_state(event.base) != null
			) {
				game.message(
					event.base.get_owner().get_faction().name +
					' has lost its Headquarters; its Global Energy Market bid is foiled.'
				);
			}
		});
		game.on('turn', check_victory);
	});
};
