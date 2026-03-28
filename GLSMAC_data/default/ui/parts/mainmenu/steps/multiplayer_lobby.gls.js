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

			body.listen(i.glsmac.game, 'chat_message', (e) => {
				message(e.player.name + ': ' + e.text);
			});

			i.connection = i.glsmac.connect();

			const f_error = (e) => {
				if (#is_defined(i.connection)) {
					i.connection.close();
					i.connection = #undefined;
					i.popup.back();
					i.popup.error(e.message);
				}
			};

			const f_start_game = () => {
				game_starting = true; // keep connection open
				i.popup.hide();
				i.glsmac.start_game();
			};

			i.connection.on('error', f_error);

			i.connection.on('connect', (e) => {
				//#print('CONNECTED');
				//#print(e);
			});
			i.connection.on('disconnect', (e) => {
				if (#is_defined(i.connection)) {
					i.connection = #undefined;
					i.popup.back();
					if (e.reason == '') {
						e.reason = 'Connection lost';
					}
					i.popup.error(e.reason);
				}
			});
			let state_listen_id = 0;
			state_listen_id = body.listen(i.connection, 'game_state', (e) => {
				body.unlisten(state_listen_id); // only initial state was needed
				switch (e.state) {
					case 'lobby': {
						// nothing
						break;
					}
					case 'initializing': {
						f_error({
							message: 'Server is initializing, try again later',
						});
						break;
					}
					case 'running': {
						f_start_game();
						break;
					}
					default: {
						f_error({
							message: 'Unexpected game state: ' + e.state,
						});
					}
				}
			});
			body.listen(i.connection, 'player_join', (e) => {
				message(e.player.name + ' joined the game.');
			});
			body.listen(i.connection, 'player_leave', (e) => {
				message(e.player.name + ' left the game.');
			});

			let countdown_value = 0;

			i.connection.open(() => {

				const ii = i + {
					start_countdown: () => {
						if (countdown_timer == null) {
							countdown_value = 3;
							message('All players are ready. Starting game in ' + #to_string(countdown_value) + ' seconds...');
							countdown_timer = body.timer(3 * 1000, () => {
								countdown_timer = null;

								f_start_game();

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
			for (cleanup of cleanups) {
				cleanup();
			}
			if (!game_starting) {
				if (#is_defined(i.connection)) {
					i.connection.close();
					i.connection = #undefined;
				}
			}
		},
	});

};
