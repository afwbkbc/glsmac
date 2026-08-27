const victory_rules = #include('victory_rules');

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
			const winner = victory_rules.get_transcendence_winner(game);
			if (winner != null) {
				victory_request_pending = true;
				game.event('declare_victory', {
					type: 'transcendence',
					winner_id: winner.id,
				});
			}
		};

		game.set('f_check_transcendence_victory', check_victory);
		game.on('update_base', (event) => { check_victory(); });
		game.on('turn', (event) => { check_victory(); });
	});
};
