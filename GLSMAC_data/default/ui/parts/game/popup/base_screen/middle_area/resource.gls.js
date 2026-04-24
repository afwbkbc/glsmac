return {

	tile_width: 100,
	tile_height: 50,

	init: (p) => {

		this.el = p.area.panel({
			class: 'base-screen-middle-area',
		});

		const w = this.tile_width; // TODO: fix 'this'
		const h = this.tile_height;

		p.ui.class('base-screen-middle-area-tile').set({
			type: 'tile-preview',
			width: w,
			height: h,
		});

	},

	set: (data) => {

		this.el.clear();

		// tile previews

		const c_left = #round(#to_float(this.tile_width) * 1.5);
		const c_top = #round(#to_float(this.tile_height) * 1.5);
		let existing_tiles = {};
		const f_tile = (tile, cx, cy) => {
			const key = #to_string(tile.x) + '_' + #to_string(tile.y);
			if (#is_defined(existing_tiles[key])) {
				// don't draw duplicate tiles
				return tile;
			}
			existing_tiles[key] = true;
			this.el.widget({
				class: 'base-screen-middle-area-tile',
				data: {
					tile: tile,
				},
				left: c_left + #round(#to_float(parent.tile_width) * cx),
				top: c_top + #round(#to_float(parent.tile_height) * cy),
			});
			return tile;
		};

		const t_center = f_tile(data.base.get_tile(), 0.0, 0.0);
		const t_n = f_tile(t_center.get_N(), 0.0, 0.0 - 1.0); // TODO: fix -1
		const t_ne = f_tile(t_center.get_NE(), 0.5, 0.0 - 0.5);
		const t_e = f_tile(t_center.get_E(), 1.0, 0.0);
		const t_se = f_tile(t_center.get_SE(), 0.5, 0.5);
		const t_s = f_tile(t_center.get_S(), 0.0, 1.0);
		const t_sw = f_tile(t_center.get_SW(), 0.0 - 0.5, 0.5);
		const t_w = f_tile(t_center.get_W(), 0.0 - 1.0, 0.0);
		const t_nw = f_tile(t_center.get_NW(), 0.0 - 0.5, 0.0 - 0.5);
		const t_n_nw = f_tile(t_n.get_NW(), 0.0 - 0.5, 0.0 - 1.5);
		const t_n_ne = f_tile(t_n.get_NE(), 0.5, 0.0 - 1.5);
		const t_ne_ne = f_tile(t_ne.get_NE(), 1.0, 0.0 - 1.0);
		const t_e_ne = f_tile(t_e.get_NE(), 1.5, 0.0 - 0.5);
		const t_e_se = f_tile(t_e.get_SE(), 1.5, 0.5);
		const t_se_se = f_tile(t_se.get_SE(), 1.0, 1.0);
		const t_s_se = f_tile(t_s.get_SE(), 0.5, 1.5);
		const t_s_sw = f_tile(t_s.get_SW(), 0.0 - 0.5, 1.5);
		const t_sw_sw = f_tile(t_sw.get_SW(), 0.0 - 1.0, 1.0);
		const t_w_sw = f_tile(t_w.get_SW(), 0.0 - 1.5, 0.5);
		const t_w_nw = f_tile(t_w.get_NW(), 0.0 - 1.5, 0.0 - 0.5);
		const t_nw_nw = f_tile(t_nw.get_NW(), 0.0 - 1.0, 0.0 - 1.0);

		// base preview

		this.el.widget({
			type: 'base-preview',
			data: {
				base: data.base,
			},
			width: parent.tile_width,
			height: parent.tile_height + 20,
			left: c_left,
			top: c_top - 20,
		});

	},

};
