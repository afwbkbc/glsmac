return {

	init: (p) => {

		const el = p.root.group({
			zindex: 0.8,
			align: 'bottom',
			height: 256,
			left: 0,
			right: 0,
		});

		const background = {
			left: el.surface({
				zindex: 0.4,
				align: 'left',
				width: 356,
				background: 'console2_a.pcx:crop(0, 0, 356, 256)',
			}),
			right: el.surface({
				zindex: 0.3,
				align: 'right',
				width: 520,
				background: 'console2_a.pcx:crop(504, 0, 1023, 256)',
			}),
			middle: el.surface({
				zindex: 0.5,
				left: 356,
				right: 520,
				background: 'console2_a.pcx:crop(357, 0, 503, 256)',
			}),
		};

		const subparts = {
			sections: {},
		};
		for (s of ['unit_preview'/*, 'tile_preview', 'middle_area', 'objects_list', 'mini_map'*/]) {
			subparts.sections[s] = #include(s);
		}
		for (s of subparts.sections) {
			s.init(subparts);
		}

		/*
				ObjectPreview* unit_preview = nullptr;
		TilePreview* tile_preview = nullptr;
		MiddleArea* middle_area = nullptr;
		ObjectsList* objects_list = nullptr;
		StatusButton* status_button = nullptr;
		MiniMap* mini_map = nullptr;

		 */

		/*p.root.surface({
			align: 'center',
			width: 800,
			height: 600,
			background: 'logo.pcx',
		});
		#print('BOTTOM BAR', p);*/

	},

};
