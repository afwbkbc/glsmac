return {

	height: 256,

	process_message: (text) => {

		if (this.message_fade_timer != null) {
			this.message_fade_timer.stop();
		}
		this.message.text = text;
		this.message_fade_timer = this.el.timer(5000, () => {
			this.message_fade_timer = null;
			this.message.text = '';
		});

		this.pp.sections.middle_area.process_message(text);
	},

	init: (p) => {

		this.pp = {};
		this.message_fade_timer = null;

		p.ui.class('bottombar-panel').extend('default-panel');
		p.ui.class('bottombar-panel-inner').extend('default-panel-inner');

		p.ui.class('bottombar-panel-button').set({
			background: 'interface.pcx:crop(47, 587, 70, 610)',
			border: 'black,2',
			_hover: {
				border: 'rgb(120,164,212),2',
			},
			_active: {
				border: 'rgb(120,164,212),2',
				background: 'interface.pcx:crop(86, 587, 109, 610)',
			},
		});

		p.ui.class('bottombar-menu-button').set({
			width: 106,
			height: 14,
			font: 'arialnb.ttf:16',
			color: 'rgb(228, 104, 24)',
			background: 'console_x2_a.pcx:crop(1, 217, 106, 230)',
			_hover: {
				color: 'black',
				background: 'console_x2_a.pcx:crop(1, 232, 106, 245)',
			},
			_active: {
				color: 'black',
				background: 'console_x2_a.pcx:crop(1, 247, 106, 260)',
			},
		});

		const height = this.height; // TODO: fix this within objects
		this.el = p.root.area({
			zindex: 0.8,
			align: 'bottom',
			height: height,
			left: 0,
			right: 0,
		});

		const background = {
			under: parent.el.surface({
				zindex: 0.2,
				left: 0,
				right: 0,
				top: 50,
				bottom: 0,
				background: 'black',
			}),
			right: parent.el.surface({
				zindex: 0.3,
				align: 'right',
				width: 520,
				background: 'console2_a.pcx:crop(504, 0, 1023, 256)',
			}),
			left: parent.el.surface({
				zindex: 0.4,
				align: 'left',
				width: 356,
				background: 'console2_a.pcx:crop(0, 0, 356, 256)',
			}),
			middle: parent.el.surface({
				zindex: 0.5,
				left: 356,
				right: 520,
				background: 'console2_a.pcx:crop(357, 0, 503, 256)',
			}),
		};

		this.message = this.el.text({
			left: 228,
			top: 10,
			font: 'arialnb.ttf:18',
			color: 'rgb(232,212,140)',
		});

		const btn_left_menu = this.el.button({
			class: 'bottombar-menu-button',
			align: 'top left',
			left: 11,
			top: 20,
			sound: 'amenu2.wav',
			volume: 0.4,
			text: 'MENU',
		});

		btn_left_menu.on('click', (e) => {
			if (!btn_left_menu.active) {
				btn_left_menu.active = true;
				p.modules.menu.open('left_menu', {
					align: 'bottom left',
					left: 0,
					bottom: parent.height,
					height: 166, // tmp
				});
			} else {
				p.modules.menu.close('left_menu');
			}
			return true;
		});

		const btn_right_menu = this.el.button({
			class: 'bottombar-menu-button',
			align: 'top right',
			right: 11,
			top: 22,
			sound: 'mmenu.wav',
			volume: 0.2,
			text: 'COMMLINK',
		});
		btn_right_menu.on('click', (e) => {
			if (!btn_right_menu.active) {
				btn_right_menu.active = true;
				p.modules.menu.open('right_menu', {
					align: 'bottom right',
					right: 0,
					bottom: parent.height,
					height: 166, // tmp
				});
			} else {
				p.modules.menu.close('right_menu');
			}
			return true;
		});

		p.modules.menu.on_close((menu_id) => {
			switch (menu_id) {
				case 'left_menu': {
					btn_left_menu.active = false;
					break;
				}
				case 'right_menu': {
					btn_right_menu.active = false;
					break;
				}
			}
		});

		this.pp = {
			game: p.game,
			map: p.map,
			ui: p.ui,
			el: parent.el,
			sections: {},
			modules: p.modules,
			get_stats_str: (object) => {
				switch (#classof(object)) {
					case 'Unit': {
						let stats_str = '';
						// tmp
						const def = object.get_def();
						if (def.id == 'SporeLauncher') {
							stats_str += '(?)';
						} else {
							stats_str += '?';
						}
						stats_str += ' - ? - ' + #to_string(#round(def.movement_per_turn));
						return stats_str;
					}
				}
				return '';
			},
		};
		for (s of ['object_preview', 'tile_preview', 'middle_area', 'objects_list', 'mini_map']) {
			this.pp.sections[s] = #include(s);
		}
		for (s of this.pp.sections) {
			s.init(this.pp);
		}

	},

};
