return {

	TRENCH_LEVEL: -2000,
	OCEAN_LEVEL: -1000,
	SEA_LEVEL: 0,

	ROCKINESS_LEVELS: ['?', 'Flat', 'Rolling', 'Rocky'],
	MOISTURE_LEVELS: ['?', 'Arid', 'Moist', 'Rainy'],

	get_feature_name: (tile, feature) => {

		if (tile.is_water) {
			// water-only features
			switch (feature) {
				case 'xenofungus': {
					return 'Sea Fungus';
				}
				case 'geothermal': {
					return 'Geothermal';
				}
			}
		} else {
			// land-only features
			switch (feature) {
				case 'xenofungus': {
					return 'Xenofungus';
				}
				case 'river': {
					return 'River';
				}
				case 'jungle': {
					return 'Jungle';
				}
				case 'dunes': {
					return 'Dunes';
				}
				case 'uranium': {
					return 'Uranium';
				}
			}
		}

		// water-or-land features
		switch (feature) {
			case 'monolith': {
				return 'Monolith';
			}
		}

	},

	get_resource_name: (resource) => {
		switch (resource) {
			case 'nutrient': {
				return 'Nutrient bonus';
			}
			case 'energy': {
				return 'Energy bonus';
			}
			case 'minerals': {
				return 'Minerals bonus';
			}
		}
	},

	set_image: (tile) => {
		if (!#is_defined(this.preview)) {
			this.preview = this.frame.widget({
				type: 'tile-preview',
				data: {
					tile: tile,
				},
				align: 'top center',
				top: 5,
				width: 84,
				height: 52,
			});
		} else {
			this.preview.data = {
				tile: tile
			};
		}
	},

	set_lines: (tile) => {

		if (#is_defined(this.lines)) {
			this.lines.remove(); // TODO: fix this.lines.clear();
		}
		this.lines = this.frame.listview({
			left: 3,
			right: 3,
			top: 63,
			bottom: 3,
			itemsize: 16,
		});

		const f_line = (text) => {
			this.lines.text({
				color: 'rgb(116,156,56)',
				font: 'arialnb.ttf:14',
				text: text,
				left: 3,
			});
		};

		if (tile.is_water) {
			if (tile.elevation < this.TRENCH_LEVEL) {
				f_line('Ocean Trench');
			} else {
				if (tile.elevation < this.OCEAN_LEVEL) {
					f_line('Ocean');
				} else {
					f_line('Ocean Shelf');
				}
			}
			f_line('Depth: ' + #to_string(this.SEA_LEVEL - tile.elevation));
		} else {
			f_line('Elev:' + #to_string(tile.elevation - this.SEA_LEVEL));
			let tilestr = '';
			if (tile.rockiness < #sizeof(this.ROCKINESS_LEVELS)) {
				tilestr += this.ROCKINESS_LEVELS[tile.rockiness];
			}
			tilestr += ' & ';
			if (tile.moisture < #sizeof(this.MOISTURE_LEVELS)) {
				tilestr += this.MOISTURE_LEVELS[tile.moisture];
			}
			f_line(tilestr);
		}

		for (f in tile.features) {
			if (tile.features[f]) {
				f_line(this.get_feature_name(tile, f));
			}
		}

		for (f in tile.resources) {
			if (tile.resources[f]) {
				f_line(this.get_resource_name(f));
			}
		}

		// TODO: terraforming

		f_line(''); // tmp workaround for 'cut-off' bottom in listview

	},

	init: (p) => {

		this.frame = p.el.panel({
			class: 'bottombar-frame',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 137,
			width: 106,
		});

		p.map.on('tile_select', (e) => {
			this.set_image(e.tile);
			this.set_lines(e.tile);
		});

	},

};
