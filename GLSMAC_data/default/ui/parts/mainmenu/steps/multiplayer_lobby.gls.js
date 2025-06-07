return (i) => {

	let parts = {};

	i.popup.show({
		title: 'Multiplayer Setup',
		width: 800,
		height: 600,
		generator: (body) => {

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
				#print('CONNECTED');
			});
			i.connection.on('disconnect', (e) => {
				#print('DISCONNECTED');
				if (#is_defined(i.connection)) {
					i.connection.close();
					i.connection = #undefined;
					i.popup.back();
					i.popup.error('Connection lost');
				}
			});
			i.connection.on('player_join', (e) => {
				#print('PLAYER JOINED', e);
			});
			i.connection.on('player_leave', (e) => {
				#print('PLAYER LEFT', e);
			});

			i.connection.open();

			const ii = i + {
				make_section: (title, properties, generator) => {
					let section = body.panel({
						class: 'popup-panel',
					} + properties);
					section.panel({
						class: 'popup-panel-header',
					}).text({
						class: 'popup-panel-header-text',
						text: title,
					});
					let inner = section.panel({
						class: 'popup-panel-body',
					});
					generator(inner);
					return section;
				},
			};

			for (section of [
				'game_settings',
				'players',
				'custom_game_options',
			]) {
				parts[section] = #include('multiplayer_lobby/' + section)(ii);
			}

		},
		destructor: () => {
			if (#is_defined(i.connection)) {
				i.connection.close();
				i.connection = #undefined;
				i.popup.back();
			}
		},
	});

};
