return {

	init: (p) => {

		p.ui.class('bottombar-frame').set({
			background: 'interface.pcx:crop(86, 665, 109, 688)',
			border: 'rgb(49, 78, 44),2',
		});

		const el = p.root.group({
			zindex: 0.8,
			align: 'bottom',
			height: 256,
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

		const p = {
			el: el,
			sections: {},
		};
		for (s of ['object_preview', 'tile_preview', 'middle_area', 'objects_list', 'mini_map']) {
			p.sections[s] = #include(s);
		}
		for (s of p.sections) {
			s.init(p);
		}

	},

};
