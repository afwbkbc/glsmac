return (i) => {

	return i.make_section('Players', {
		left: 301,
		width: 490,
		height: 358,
	}, (body) => {

		let rows = {};

		const add_row = (player) => {
			const id = #to_string(player.id);
			if (!#is_defined(rows[id])) {
				const row = body.panel({
					class: 'lobby-player-row',
					top: player.id * 24 + 2,
				});
				rows[id] = {
					row: row,
					ready: row.panel({
						class: 'lobby-player-ready',
					}),
					name: row.button({
						class: 'lobby-player-name',
						text: player.name,
					}),
					faction: row.select({
						class: 'lobby-player-faction',
						items: [
							['random', 'Random'],
							['fac1', 'Fac1'],
							['fac2', 'Fac2'],
						],
						value: 'random', // TODO
					}),
					difficulty: row.select({
						class: 'lobby-player-difficulty',
						items: [
							['transcend', 'Transcend'],
						],
						value: 'transcend', // TODO
					}),
				};
			}
		};

		const remove_row = (player) => {
			const id = #to_string(player.id);
			if (#is_defined(rows[id])) {
				(rows[id].row).remove(); // TODO: remove ( )
				rows[id] = #undefined;
			}
		};

		const players = i.glsmac.game.get_players();
		for (player of players) {
			add_row(player);
		}

		i.connection.on('player_join', (e) => {
			add_row(e.player);
		});
		i.connection.on('player_leave', (e) => {
			remove_row(e.player);
		});


	});

};
