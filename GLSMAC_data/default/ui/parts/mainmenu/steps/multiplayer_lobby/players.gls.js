return (i) => {

	return i.make_section('Players', {
		left: 301,
		width: 490,
		height: 202,
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
		const buttons = {
			ready: body.button({
				class: 'popup-button',
				align: 'bottom left',
				bottom: 4,
				left: 8,
				width: 234,
				height: 22,
				text: 'Ready',
			}),
			cancel: body.button({
				class: 'popup-button',
				align: 'bottom right',
				bottom: 4,
				right: 8,
				width: 234,
				height: 22,
				text: 'Cancel',
			}),
		};

		const add_row = (player) => {
			const id = #to_string(player.id);
			if (!#is_defined(rows[id])) {
				const row_el = body.panel({
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
				const faction_select = row_el.select({
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

				let row = {
					row: row_el,
					ready: row_el.panel(),
					name: row_el.button({
						class: 'lobby-player-name',
						text: player.name,
						color: faction_color,
					}),
					faction: faction_select,
					difficulty: row_el.select({
						class: 'lobby-player-difficulty',
						items: [
							['transcend', 'Transcend'],
						],
						value: 'transcend', // TODO
						readonly: !is_me,
						color: faction_color,
					}),
				};

				rows[id] = row;

				update_row(player);
			}
		};

		const update_row = (player) => {

			const id = #to_string(player.id);
			const row = rows[id];

			if (#is_defined(row)) {

				const is_me = player.id == me.id;
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
				if (player.is_ready()) {
					row.ready.class = 'lobby-player-ready';
					if (is_me) {
						buttons.ready.text = 'Not ready';
					}
				} else {
					row.ready.class = 'lobby-player-notready';
					if (is_me) {
						buttons.ready.text = 'Ready';
					}
				}
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

		buttons.cancel.on('click', (e) => {
			i.popup.back();
			return true;
		});
		buttons.ready.on('click', (e) => {
			if (buttons.ready.text == 'Ready') {
				game.event('ready_or_not', {
					ready: true,
				});
			} else {
				game.event('ready_or_not', {
					ready: false,
				});
			}
			return true;
		});

	}, () => {
		const game = i.glsmac.game;
		game.off('player_update');
	});

};
