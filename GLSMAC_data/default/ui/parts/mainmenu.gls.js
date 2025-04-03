return (m) => {

	const menublock = #include('mainmenu/menublock')(m);
	let background = null;
	let music = null;

	m.glsmac.on('mainmenu_show', (e) => {

		#print('SETTINGS', e);
		const randomize_map = () => {
			e.settings.global.map.ocean_coverage = #random_float(0.4, 0.8);
			e.settings.global.map.erosive_forces = #random_float(0.5, 1.0);
			e.settings.global.map.native_lifeforms = #random_float(0.25, 0.75);
			e.settings.global.map.cloud_cover = #random_float(0.25, 0.75);
		};

		randomize_map();

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
						m.glsmac.init_single_player(() => {
							e.settings.local.game_mode = 'single';
							steps.start_game();
						});
					}],
					['Quick Start', () => {
						m.glsmac.init_single_player(() => {
							e.settings.local.game_mode = 'single';
							randomize_map();
							m.glsmac.start_game();
						});
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
						randomize_map();
						steps.select_mapsize();
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

			select_mapsize: () => {
				menublock.show([], () => {
					
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
