return {

	init: (p) => {

		this.frame = p.el.panel({
			class: 'bottombar-frame',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 137,
			width: 106,
		});

		const f_update = (tile) => {
			if (!#is_defined(this.preview)) {
				this.preview = this.frame.widget({
					type: 'tile-preview',
					data: {
						tile: tile,
					},
					align: 'top center',
					top: 2,
					width: 84,
					height: 52,
				});
			} else {
				this.preview.data = {
					tile: tile
				};
			}
		};

		p.map.on('tile_select', (e) => {
			f_update(e.tile);
		});
		f_update(p.map.get_selected_tile());

	},

};
