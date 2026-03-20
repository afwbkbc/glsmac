return {

	height: 256,

	pp: {},

	message_fade_timer: null,

	process_message: (text) => {

		if (this.message_fade_timer != null) {
			this.message_fade_timer.stop();
		}
		this.message.text = text;
		this.message_fade_timer = #async(5000, () => {
			this.message_fade_timer = null;
			this.message.text = '';
		});

		this.pp.sections.middle_area.process_message(text);
	},

	init: (p) => {

		p.ui.class('bottombar-frame').set({
			background: 'interface.pcx:crop(86, 665, 109, 688)',
			border: 'rgb(49, 78, 44),2',
		});

		p.ui.class('bottombar-frame-inner').set({
			background: 'interface.pcx:crop(86, 548, 109, 571)',
			border: 'rgb(35, 59, 34),2',
			left: 3,
			top: 3,
			right: 3,
			bottom: 3,
		});

		p.ui.class('bottombar-frame-button').set({
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

		const height = this.height; // TODO: fix this within objects
		const el = p.root.area({
			zindex: 0.8,
			align: 'bottom',
			height: height,
			left: 0,
			right: 0,
		});

		const background = {
			under: el.surface({
				zindex: 0.2,
				left: 0,
				right: 0,
				top: 50,
				bottom: 0,
				background: 'black',
			}),
			right: el.surface({
				zindex: 0.3,
				align: 'right',
				width: 520,
				background: 'console2_a.pcx:crop(504, 0, 1023, 256)',
			}),
			left: el.surface({
				zindex: 0.4,
				align: 'left',
				width: 356,
				background: 'console2_a.pcx:crop(0, 0, 356, 256)',
			}),
			middle: el.surface({
				zindex: 0.5,
				left: 356,
				right: 520,
				background: 'console2_a.pcx:crop(357, 0, 503, 256)',
			}),
		};

		this.message = el.text({
			left: 228,
			top: 10,
			font: 'arialnb.ttf:18',
			color: 'rgb(232,212,140)',
		});

		this.pp = {
			game: p.game,
			map: p.map,
			ui: p.ui,
			el: el,
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
