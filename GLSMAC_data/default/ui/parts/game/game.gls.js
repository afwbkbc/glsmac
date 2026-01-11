return (m) => {

	m.glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('start_ui', (e) => {

			m.root.clear();

			const p = {
				game: game,
				map: game.get_map(),
				ui: m.ui,
				root: m.root,
				modules: {
					bottom_bar: #include('bottom_bar/bottom_bar'),
				},
			};
			for (module of p.modules) {
				module.init(p);
			}

			// TODO

		});

	});

};
