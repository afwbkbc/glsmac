return (m) => {

	let background = null;
	let music = null;

	const sliding = #include('sliding')(m);
	const popup = #include('popup')(m);

	// in some cases (i.e. with --host or --join command line arguments) game should exit on cancel instead of reopening same popup
	let mainmenu_allowed = true;

	m.glsmac.on('mainmenu_show', (e) => {

		m.root.clear();

		if (!mainmenu_allowed) {
			m.glsmac.exit();
			return;
		}

		const i = {
			glsmac: m.glsmac,
			sliding: sliding,
			popup: popup,
			settings: e.settings,
			randomize_map: () => {
				e.settings.global.map.ocean_coverage = #random_float(0.4, 0.8);
				e.settings.global.map.erosive_forces = #random_float(0.5, 1.0);
				e.settings.global.map.native_lifeforms = #random_float(0.25, 0.75);
				e.settings.global.map.cloud_cover = #random_float(0.25, 0.75);
			},
			customize_preview_defaults: {
				// copied from original SMAC
				ocean_coverage_type: 3,
				erosive_forces_type: 0,
				native_lifeforms_type: 1,
				cloud_cover_type: 2,
			},
		};

		const resize_customize_preview = (width) => {
			if (#is_defined(i.customize_preview)) {
				i.customize_preview.width = #round(#to_float(width) * 404.0 / 1024.0);
			}
		};
		const resize_customize_preview_moons = (width, height) => {
			if (#is_defined(i.customize_preview_moons)) {
				i.customize_preview_moons.width = #round(#to_float(width) * 450.0 / 1024.0);
				i.customize_preview_moons.height = #round(#to_float(height) * 450.0 / 768.0);
			}
		};

		i.unset_customize_preview = () => {
			if (#is_defined(i.customize_preview)) {
				i.customize_preview.remove();
				i.customize_preview = #undefined;
				i.customize_preview_data = #undefined;
			}
		};
		i.set_customize_preview = (data) => {
			if (!#is_defined(i.customize_preview_data)) {
				i.customize_preview_data = #clone(i.customize_preview_defaults);
			}
			let planet_changed = false;
			let moons_changed = false;
			for (k in data) {
				if (i.customize_preview_data[k] != data[k]) {
					i.customize_preview_data[k] = data[k];
					if (k == 'erosive_forces_type') {
						moons_changed = true;
					} else {
						planet_changed = true;
					}
				}
			}
			if (planet_changed) {
				const background = 'S' + #to_string(i.customize_preview_data.ocean_coverage_type) + 'L' + #to_string(i.customize_preview_data.native_lifeforms_type) + 'C' + #to_string(i.customize_preview_data.cloud_cover_type) + '.pcx:stretch()';
				if (#is_defined(i.customize_preview)) {
					i.customize_preview.background = background;
				} else {
					i.customize_preview = m.root.surface({
						id: 'mainmenu-customize-preview',
						align: 'right',
						top: 0,
						bottom: 0,
						right: 0,
						zindex: 0.3,
						background: background,
					});
					resize_customize_preview(m.ui.get_width());
				}
			}
			if (moons_changed) {
				if (i.customize_preview_data.erosive_forces_type == 0) {
					if (#is_defined(i.customize_preview_moons)) {
						i.customize_preview_moons.remove();
						i.customize_preview_moons = #undefined;
					}
				} else {
					const background = 'moon' + #to_string(i.customize_preview_data.erosive_forces_type) + '.pcx:stretch()';
					if (#is_defined(i.customize_preview_moons)) {
						i.customize_preview_moons.background = background;
					} else {
						i.customize_preview_moons = m.root.surface({
							id: 'mainmenu-customize-preview-moons',
							align: 'top left',
							top: 0,
							left: 0,
							zindex: 0.3,
							background: background,
						});
						resize_customize_preview_moons(m.ui.get_width(), m.ui.get_height());
					}
				}
			}
		};

		m.ui.on('resize', (wh) => {
			resize_customize_preview(wh.width);
			resize_customize_preview_moons(wh.width, wh.height);
		});

		background = m.root.surface({
			id: 'mainmenu-background',
			top: 0,
			left: 0,
			right: 0,
			bottom: 0,
			zindex: 0.2,
			background: 'openinga.pcx:stretch()',
		});

		music = m.root.sound({
			id: 'mainmenu-music',
			sound: 'opening menu.wav',
			autoplay: true,
			repeat: true,
			volume: 0.8,
		});

		i.steps = {};
		for (step of [
			'notimpl',
			'main',
			'start_game',
			'select_mapsize',
			'customize_ocean_coverage',
			'customize_erosive_forces',
			'customize_native_lifeforms',
			'customize_cloud_cover',
			'select_difficulty_level',
			'select_rules',
			'multiplayer_type',
			'multiplayer_role',
			'multiplayer_host',
			'multiplayer_join',
			'multiplayer_lobby',
		]) {
			i.steps[step] = #include('steps/' + step);
		}

		const c = m.glsmac.config;
		if (#is_defined(c.host)) {
			if (!#is_defined(c.gamename)) {
				throw Error('--host requires --gamename argument');
			}
			if (!#is_defined(c.playername)) {
				throw Error('--host requires --playername argument');
			}
			mainmenu_allowed = false; // do not reopen menu next time
			i.settings.local.network_type = 'simple_tcpip';
			i.settings.global.game_name = c.gamename;
			i.settings.local.player_name = c.playername;
			i.settings.local.network_role = 'server';
			m.glsmac.init();
			i.steps.multiplayer_lobby(i);
		} else if (#is_defined(c.join)) {
			if (!#is_defined(c.playername)) {
				throw Error('--join requires --playername argument');
			}
			mainmenu_allowed = false; // do not reopen menu next time
			i.settings.local.network_role = 'client';
			i.settings.local.player_name = c.playername;
			i.settings.local.remote_address = c.join;
			m.glsmac.init();
			i.steps.multiplayer_lobby(i);
		} else {
			i.steps.main(i);
		}
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
		popup.hide();
		m.root.clear();
	});

};
