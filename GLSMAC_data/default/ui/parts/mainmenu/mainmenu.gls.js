return (m) => {

	const sliding = #include('sliding')(m);
	const popup = #include('popup')(m);

	let background = null;
	let music = null;

	m.glsmac.on('mainmenu_show', (e) => {

		const randomize_map = () => {
			e.settings.global.map.ocean_coverage = #random_float(0.4, 0.8);
			e.settings.global.map.erosive_forces = #random_float(0.5, 1.0);
			e.settings.global.map.native_lifeforms = #random_float(0.25, 0.75);
			e.settings.global.map.cloud_cover = #random_float(0.25, 0.75);
		};

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

			notimpl: () => {
				popup.error('This feature is not implemented yet.');
			},

			main: () => {
				m.glsmac.deinit();
				sliding.show({
					entries: [
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
							steps.notimpl();
						}],
						['Load Game', () => {
							steps.notimpl();
						}],
						['Multiplayer', () => {
							steps.notimpl();
						}],
						['View Credits', () => {
							steps.notimpl();
						}],
						['Exit Game', () => {
							m.glsmac.exit();
						}],
					]
				});
			},

			start_game: () => {
				sliding.show({
					entries: [
						['Make Random Map', () => {
							e.settings.global.map.type = 'random';
							randomize_map();
							steps.select_mapsize();
						}],
						['Customize Random Map', () => {
							e.settings.global.map.type = 'custom';
							steps.select_mapsize();
						}],
						['The Map of Planet', () => {
							steps.notimpl();
						}],
						['Huge Map of Planet', () => {
							steps.notimpl();
						}],
						['Load Map File', () => {
							steps.notimpl();
						}],
					]
				});
			},

			select_mapsize: () => {
				const next = () => {
					if (e.settings.global.map.type == 'random') {
						steps.select_difficulty_level();
					} else {
						steps.customize_ocean_coverage();
					}
				};
				sliding.show({
					title: 'Select size of planet',
					entries: [
						['Tiny Planet', () => {
							e.settings.global.map.size_x = 68;
							e.settings.global.map.size_y = 34;
							next();
						}],
						['Small Planet', () => {
							e.settings.global.map.size_x = 88;
							e.settings.global.map.size_y = 44;
							next();
						}],
						['Standard Planet', () => {
							e.settings.global.map.size_x = 112;
							e.settings.global.map.size_y = 56;
							next();
						}],
						['Large Planet', () => {
							e.settings.global.map.size_x = 140;
							e.settings.global.map.size_y = 70;
							next();
						}],
						['Huge Planet', () => {
							e.settings.global.map.size_x = 180;
							e.settings.global.map.size_y = 90;
							next();
						}],
					]
				});
			},

			customize_ocean_coverage: () => {
				sliding.show({
					title: 'Select ocean coverage',
					entries: [
						['30-50% of surface', () => {
							e.settings.global.map.ocean_coverage = 0.4;
							steps.customize_erosive_forces();
						}],
						['50-70% of surface', () => {
							e.settings.global.map.ocean_coverage = 0.6;
							steps.customize_erosive_forces();
						}],
						['70-90% of surface', () => {
							e.settings.global.map.ocean_coverage = 0.8;
							steps.customize_erosive_forces();
						}],
					]
				});
			},
			customize_erosive_forces: () => {
				sliding.show({
					title: 'Adjust erosive forces',
					entries: [
						['Strong', () => {
							e.settings.global.map.erosive_forces = 0.5;
							steps.customize_native_lifeforms();
						}],
						['Average', () => {
							e.settings.global.map.erosive_forces = 0.75;
							steps.customize_native_lifeforms();
						}],
						['Weak', () => {
							e.settings.global.map.erosive_forces = 1.0;
							steps.customize_native_lifeforms();
						}],
					]
				});
			},
			customize_native_lifeforms: () => {
				sliding.show({
					title: 'Native life forms',
					entries: [
						['Rare', () => {
							e.settings.global.map.native_lifeforms = 0.25;
							steps.customize_cloud_cover();
						}],
						['Average', () => {
							e.settings.global.map.native_lifeforms = 0.5;
							steps.customize_cloud_cover();
						}],
						['Abundant', () => {
							e.settings.global.map.native_lifeforms = 0.75;
							steps.customize_cloud_cover();
						}],
					]
				});
			},
			customize_cloud_cover: () => {
				sliding.show({
					title: 'Select cloud cover',
					entries: [
						['Sparse', () => {
							e.settings.global.map.cloud_cover = 0.25;
							steps.select_difficulty_level();
						}],
						['Average', () => {
							e.settings.global.map.cloud_cover = 0.5;
							steps.select_difficulty_level();
						}],
						['Dense', () => {
							e.settings.global.map.cloud_cover = 0.75;
							steps.select_difficulty_level();
						}],
					]
				});
			},
			select_difficulty_level: () => {
				let entries = [];
				for (difficulty_level of e.settings.global.rules.difficulty_levels) {
					const d = difficulty_level; // TODO: gse fix: keep difficulty_level available here
					entries :+[difficulty_level, () => {
						e.settings.global.difficulty_level = d;
						steps.select_rules();
					}];
				}
				sliding.show({
					title: 'Pick a difficulty level',
					entries: entries
				});
			},

			select_rules: () => {
				sliding.show({
					title: 'Game rules',
					entries: [
						['Play with Standard Rules', () => {
							// TODO: rules
							// TODO: faction selection
							m.glsmac.start_game();
						}],
						['Play with Current Rules', () => {
							// TODO: rules
							// TODO: faction selection
							m.glsmac.start_game();
						}],
						['Customize Rules', () => {
							// TODO: custom rules
							steps.notimpl();
						}],
					]
				});
				#print('SETTINGS', e.settings.global);
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
		sliding.hide();
	});

};
