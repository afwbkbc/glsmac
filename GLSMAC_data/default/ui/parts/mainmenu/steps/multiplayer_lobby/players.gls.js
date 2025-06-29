return (i) => {

	return i.make_section('Players', {
		left: 301,
		width: 490,
		height: 358,
	}, (body) => {

		const game = i.glsmac.game;
		const me = game.get_player();

		const factions = game.get_fm().list();
		let faction_choices = [
			['RANDOM', 'Random', {
				color: 'white',
			}],
		];
		for (faction of factions) {
			faction_choices :+[faction.id, faction.name, {
				color: faction.text_color,
			}];
		}

		let rows = {};

		const add_row = (player) => {
			const id = #to_string(player.id);
			if (!#is_defined(rows[id])) {
				const row = body.panel({
					class: 'lobby-player-row',
					top: player.id * 24 + 2,
				});

				const is_me = player.id == me.id;

				const faction = player.get_faction();

				let faction_id = 'RANDOM';
				let faction_color = 'white';
				if (#is_defined(faction)) {
					faction_id = faction.id;
					faction_color = faction.text_color;
				}
				const faction_select = row.select({
					class: 'lobby-player-faction',
					items: faction_choices,
					value: faction_id,
					color: faction_color,
					readonly: !is_me,
				});
				faction_select.on('select', (e) => {
					game.event('select_faction', {
						faction: e.value,
					});
					return true;
				});

				rows[id] = {
					row: row,
					ready: row.panel({
						class: 'lobby-player-ready',
					}),
					name: row.button({
						class: 'lobby-player-name',
						text: player.name,
						color: faction_color,
					}),
					faction: faction_select,
					difficulty: row.select({
						class: 'lobby-player-difficulty',
						items: [
							['transcend', 'Transcend'],
						],
						value: 'transcend', // TODO
						readonly: !is_me,
						color: faction_color,
					}),
				};

			}
		};

		const update_row = (player) => {
			const id = #to_string(player.id);
			const row = rows[id];

			if (#is_defined(row)) {
				const faction = player.get_faction();
				let color = 'white';
				if (#is_defined(faction)) {
					row.faction.value = faction.id;
					color = faction.text_color;
				} else {
					row.faction.value = 'RANDOM';
				}
				row.name.color = color;
				row.faction.color = color;
				row.difficulty.color = color;
			}
		};

		const remove_row = (player) => {
			const id = #to_string(player.id);
			if (#is_defined(rows[id])) {
				(rows[id].row).remove(); // TODO: remove ( )
				rows[id] = #undefined;
			}
		};

		const players = game.get_players();
		for (player of players) {
			add_row(player);
		}

		i.connection.on('player_join', (e) => {
			add_row(e.player);
		});
		i.connection.on('player_leave', (e) => {
			remove_row(e.player);
		});

		game.on('player_update', (e) => {
			update_row(e.player);
		});

	});

};
