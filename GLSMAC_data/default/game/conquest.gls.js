return (game) => {

	game.on('start', (e) => {
		let victory_request_pending = false;
		const check_victory = () => {
			if (
				!game.is_master() ||
				game.is_game_over() ||
				victory_request_pending ||
				game.get_turn() == 0
			) {
				return;
			}
			const winner = game.get_conquest_winner();
			if (winner != null) {
				victory_request_pending = true;
				game.event('declare_victory', {
					type: 'conquest',
					winner_id: winner.id,
				});
			}
		};

		game.set('f_check_conquest_victory', check_victory);
		game.get_um().on('unit_despawn', check_victory);
		game.get_bm().on('base_despawn', check_victory);
		game.on('update_base', (event) => { check_victory(); });
		game.on('turn', (event) => { check_victory(); });
	});

};
