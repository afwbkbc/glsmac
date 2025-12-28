return (m) => {

	let background = null;
	let music = null;

	const sliding = #include('sliding')(m);
	const popup = #include('popup')(m);

	// in some cases (i.e. with --host or --join command line arguments) game should exit on cancel instead of reopening same popup
	let mainmenu_allowed = true;

	m.glsmac.on('mainmenu_show', (e) => {

		m.root.clear();

		if ( !mainmenu_allowed ) {
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
				throw Error( '--host requires --gamename argument' );
			}
			if (!#is_defined(c.playername)) {
				throw Error( '--host requires --playername argument' );
			}
			mainmenu_allowed = false; // do not reopen menu next time
			i.settings.local.network_type = 'simple_tcpip';
			i.settings.global.game_name = c.gamename;
			i.settings.local.player_name = c.playername;
			i.settings.local.network_role = 'server';
			m.glsmac.init();
			i.steps.multiplayer_lobby(i);
		} elseif (#is_defined(c.join)) {
			if (!#is_defined(c.playername)) {
				throw Error( '--join requires --playername argument' );
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
