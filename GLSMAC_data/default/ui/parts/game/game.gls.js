return (m) => {

	const modules = [
		'menu',
		'popup',
		'bottom_bar',
	];

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
				glsmac: m.glsmac,
				map: game.get_map(),
				ui: m.ui,
				root: m.root,
				modules: {},
				process_message: (text) => {
					for (m of modules) {
						const module = p.modules[m];
						if (#is_defined(module.process_message)) {
							module.process_message(text);
						}
					}
				},
				maybe_quit: (full_quit) => {
					p.modules.popup.show('please_dont_go', (result) => {
						if (result) {
							if (full_quit) {
								m.glsmac.exit();
							} else {
								m.glsmac.reset();
							}
						}
					});
				},
			};
			for (m of modules) {
				p.modules[m] = #include(m + '/' + m);
			}
			for (m of modules) {
				p.modules[m].init(p);
			}

			m.root.on('keydown', (e) => {
				if (e.modifiers == {} && e.code == 'ESCAPE') {
					p.maybe_quit(true);
					return true;
				}
				if (e.modifiers == {ctrl: true, shift: true} && e.code == 'Q') {
					p.maybe_quit(false);
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
