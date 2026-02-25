return (m) => {

	m.glsmac.on('configure_game', (e) => {
		const game = e.game;

		let ui = null;
		let messages_buffer = [];

		game.on('message', (e) => {
			if (ui != null) {
				ui.process_message(e.text);
			} else {
				messages_buffer :+e.text;
			}
		});

		game.on('start_ui', (e) => {

			m.root.clear();

			ui = {
				game: game,
				map: game.get_map(),
				ui: m.ui,
				root: m.root,
				modules: {
					bottom_bar: #include('bottom_bar/bottom_bar'),
				},
				process_message: (text) => {
					for (module of this.modules) {
						if (#is_defined(module.process_message)) {
							module.process_message(text);
						}
					}
				},
			};
			for (module of ui.modules) {
				module.init(ui);
			}

			for (m of messages_buffer) {
				ui.process_message(m);
			}
			messages_buffer = [];
			// TODO

		});

	});

};
