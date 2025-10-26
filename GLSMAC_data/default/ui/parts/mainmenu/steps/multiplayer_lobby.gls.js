return (i) => {

	let parts = {};

	let game_starting = false;

	const sections = [
		'game_settings',
		'players',
		'chat',
		'custom_game_options',
	];
	let cleanups = [];

	let countdown_timer = null;

	i.popup.show({
		title: 'Multiplayer Setup',
		width: 800,
		height: 600,
		generator: (body, lobby) => {

			let pending_messages = [];
			const message = (text) => {
				if (#is_defined(parts.chat)) {
					parts.chat.add_message(text);
				} else {
					pending_messages :+text;
				}
			};

			i.glsmac.game.on('chat_message', (e) => {
				message(e.player.name + ': ' + e.text);
			});

			i.connection = i.glsmac.connect();

			i.connection.on('error', (e) => {
				if (#is_defined(i.connection)) {
					i.connection.close();
					i.connection = #undefined;
					i.popup.back();
					i.popup.error(e.message);
				}
			});

			i.connection.on('connect', (e) => {
				//#print('CONNECTED');
			});
			i.connection.on('disconnect', (e) => {
				if (#is_defined(i.connection)) {
					i.connection = #undefined;
					i.popup.back();
					i.popup.error('Connection lost');
				}
			});
			i.connection.on('player_join', (e) => {
				message(e.player.name + ' joined the game.');
			});
			i.connection.on('player_leave', (e) => {
				message(e.player.name + ' left the game.');
			});

			let countdown_value = 0;

			i.connection.open(() => {

				const ii = i + {
					start_countdown: () => {
						if (countdown_timer == null) {
							countdown_value = 3;
							message('All players are ready. Starting game in ' + #to_string(countdown_value) + ' seconds...');
							countdown_timer = #async(3 * 1000, () => {
								countdown_timer = null;

								game_starting = true; // keep connection open
								i.popup.hide();
								i.glsmac.start_game();

								return false;
							});
						}
					},
					stop_countdown: () => {
						if (countdown_timer != null) {
							countdown_timer.stop();
							countdown_timer = null;
						}
					},
					chat_message: message,
					make_section: (title, properties, generator, cleanup) => {
						let section = body.panel({
							class: 'popup-panel',
						} + properties);
						if (#is_defined(title)) {
							section.panel({
								class: 'popup-panel-header',
							}).text({
								class: 'popup-panel-header-text',
								text: title,
							});
						}
						let cls = 'popup-panel-body';
						if (!#is_defined(title)) {
							cls += '-noheader';
						}
						let inner = section.panel({
							class: cls,
						});
						generator(inner, this);
						if (#is_defined(cleanup)) {
							cleanups :+cleanup;
						}
						return section;
					},
				};

				for (section of sections) {
					parts[section] = #include('multiplayer_lobby/' + section)(ii);
				}
				for (text of pending_messages) {
					parts.chat.add_message(text);
				}
				pending_messages = [];

			});
		},
		destructor: () => {
			if (countdown_timer != null) {
				countdown_timer.stop();
			}
			i.glsmac.game.off('chat_message');
			for (cleanup of cleanups) {
				cleanup();
			}
			if (!game_starting) {
				if (#is_defined(i.connection)) {
					i.connection.close();
					i.connection = #undefined;
				}
				i.popup.back();
			} else {
				// there will be different handlers
				i.connection.off('player_join');
				i.connection.off('player_leave');
			}
		},
	});

};
