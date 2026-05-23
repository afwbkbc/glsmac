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

	set_image: () => {
		const tile = this.tile;
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
				tile: tile,
			};
		}
	},

	line: (text) => {
		this.lines.text({
			color: 'rgb(116,156,56)',
			font: 'arialnb.ttf:14',
			text: text,
			left: 3,
		});
	},

	set_lines: () => {
		const tile = this.tile;
		if (#is_defined(this.lines)) {
			this.lines.remove(); // TODO: fix .clear()
		}
		this.lines = this.frame.listview({
			left: 3,
			right: 3,
			top: 63,
			bottom: 3,
			itemsize: 16,
		});

		if (this.show_resources) {

			const resources = tile.get_resources();
			this.line('Nutrients: ' + #to_string(resources.NUTRIENTS));
			this.line(''); // TODO: hints
			this.line('Minerals: ' + #to_string(resources.MINERALS));
			this.line(''); // TODO: hints
			this.line('Energy: ' + #to_string(resources.ENERGY));
			this.line(''); // TODO: hints

		} else {

			if (tile.is_water) {
				if (tile.elevation < this.TRENCH_LEVEL) {
					this.line('Ocean Trench');
				} else if (tile.elevation < this.OCEAN_LEVEL) {
					this.line('Ocean');
				} else {
					this.line('Ocean Shelf');
				}
				this.line('Depth: ' + #to_string(this.SEA_LEVEL - tile.elevation));
			} else {
				this.line('Elev:' + #to_string(tile.elevation - this.SEA_LEVEL));
				let tilestr = '';
				if (tile.rockiness < #sizeof(this.ROCKINESS_LEVELS)) {
					tilestr += this.ROCKINESS_LEVELS[tile.rockiness];
				}
				tilestr += ' & ';
				if (tile.moisture < #sizeof(this.MOISTURE_LEVELS)) {
					tilestr += this.MOISTURE_LEVELS[tile.moisture];
				}
				this.line(tilestr);
			}

			for (f in tile.features) {
				if (tile.features[f]) {
					this.line(this.get_feature_name(tile, f));
				}
			}

			for (f in tile.bonuses) {
				if (tile.bonuses[f]) {
					this.line(this.get_resource_name(f));
				}
			}

			// TODO: terraforming

		}

		this.line(''); // tmp workaround for 'cut-off' bottom in listview

	},

	init: (p) => {

		this.show_resources = false;

		const frame_outer = p.el.panel({
			class: 'bottombar-panel',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 137,
			width: 106,
		});
		this.frame = frame_outer.panel({
			class: 'bottombar-panel-inner',
		});

		p.map.on('tile_preview', (e) => {
			this.tile = e.tile;
			this.set_image();
			this.set_lines();
		});

		frame_outer.on('mousedown', (e) => {
			this.show_resources = !this.show_resources;
			this.set_lines();
			return true;
		});

	},

};
