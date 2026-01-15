return {

	update_units: (tile) => {
		if (#is_defined(this.list)) {
			this.list.remove();
		}
		this.list = this.frame.scrollview({
			left: 2,
			right: 2,
			top: -6,
			bottom: -4,
			padding: 10,
		});

		const width = 64;
		let left = 0;

		for (unit of tile.get_units()) {
			#print('UNIT', unit);
			this.list.widget({
				type: 'unit-preview',
				data: {
					unit: unit,
				},
				left: left,
				width: width,
				height: 48,
			});
			left += width;
		}
	},

	init: (p) => {

		this.frame = p.el.panel({
			class: 'bottombar-frame',
			align: 'bottom',
			bottom: 7,
			left: 245,
			right: 258,
			height: 58,
		});

		p.map.on('tile_preview', (e) => {
			this.update_units(e.tile);
		});

	},

};
