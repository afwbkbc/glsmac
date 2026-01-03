return (m) => {

	m.glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('start', (e) => {

			m.root.clear();

			const parts = {
				ui: m.ui,
				root: m.root,
				modules: {
					bottom_bar: #include('bottom_bar/bottom_bar'),
				},
			};
			for (module of parts.modules) {
				module.init(parts);
			}

			// TODO

		});

	});

};
