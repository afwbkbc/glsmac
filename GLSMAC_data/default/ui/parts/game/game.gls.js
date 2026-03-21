return (m) => {

	m.glsmac.on('configure_game', (e) => {
		const game = e.game;

		let p = null;
		let messages_buffer = [];

		game.on('message', (e) => {
			if (p != null) {
				p.process_message(e.text);
			} else {
				messages_buffer :+e.text;
			}
		});

		game.on('start_ui', (e) => {

			m.root.clear();

			p = {
				game: game,
				map: game.get_map(),
				ui: m.ui,
				root: m.root,
				modules: {
					bottom_bar: #include('bottom_bar/bottom_bar'),
					popup: #include('popup/popup'),
				},
				process_message: (text) => {
					for (module of this.modules) {
						if (#is_defined(module.process_message)) {
							module.process_message(text);
						}
					}
				},
			};
			for (module of p.modules) {
				module.init(p);
			}

			m.root.on('keydown', (e) => {
				if (e.code == 'ESCAPE') {
					p.modules.popup.show('please_dont_go', (result) => {
						if (result) {
							m.glsmac.exit();
						}
					});
					return true;
				}
				return false;
			});

			for (m of messages_buffer) {
				p.process_message(m);
			}
			messages_buffer = [];
			// TODO

		});

	});

};
