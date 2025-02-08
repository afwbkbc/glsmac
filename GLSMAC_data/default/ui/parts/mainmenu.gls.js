return (m) => {

	const menublock = #include('mainmenu/menublock')(m);
	let background = null;
	let music = null;

	m.glsmac.on('mainmenu_show', (e) => {

		background = m.root.surface({
			id: 'mainmenu-background',
			top: 0,
			left: 0,
			right: 0,
			bottom: 0,
			background: 'openinga.pcx:stretch()',
		});

		music = m.root.sound({
			id: 'mainmenu-music',
			sound: 'opening menu.wav',
			autoplay: true,
			repeat: true,
			volume: 0.8,
		});

		let steps = null;
		steps = {

			main: () => {
				m.glsmac.deinit();
				menublock.show([
					['Start Game', () => {
						m.glsmac.init_single_player();
						steps.start_game();
					}],
					['Quick Start', () => {
						m.glsmac.init_single_player();
						m.glsmac.randomize_settings();
						m.glsmac.start_game();
					}],
					['Scenario', () => {

					}],
					['Load Game', () => {

					}],
					['Multiplayer', () => {

					}],
					['View Credits', () => {

					}],
					['Exit Game', () => {
						m.glsmac.exit();
					}],
				], () => {
					m.glsmac.exit();
				});
			},

			start_game: () => {
				menublock.show([
					['Make Random Map', () => {

					}],
					['Customize Random Map', () => {

					}],
					['The Map of Planet', () => {

					}],
					['Huge Map of Planet', () => {

					}],
					['Load Map File', () => {

					}],
				], () => {
					return steps.main();
				});
			},

		};

		steps.main();

	});

	m.glsmac.on('mainmenu_hide', (e) => {
		if (background != null) {
			background.remove();
			background = null;
		}
		if (music != null) {
			music.remove();
			music = null;
		}
		menublock.hide();
	});

};
