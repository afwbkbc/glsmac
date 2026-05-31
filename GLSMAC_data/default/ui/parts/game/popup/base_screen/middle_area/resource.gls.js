return {

	tile_width: 100,
	tile_height: 50,

	init: (p) => {

		this.p = p;

		this.el = p.area.panel({
			class: 'base-screen-middle-area',
		});

		p.ui.class('base-screen-middle-area-tile').set({
			width: parent.tile_width,
			height: parent.tile_height,
		});

	},

	set: (data) => {

		this.el.clear();

		// tile previews

		const c_left = #round(#to_float(this.tile_width) * 1.5);
		const c_top = #round(#to_float(this.tile_height) * 1.5);
		let existing_tiles = {};

		const tile_aspect_ratio = this.tile_width / this.tile_height;

		const f_tile_key = (tile) => {
			return #to_string(tile.x) + '_' + #to_string(tile.y);
		};

		const f_tile = (tile, cx, cy) => {
			const key = f_tile_key(tile);
			if (#is_defined(existing_tiles[key])) {
				// don't draw duplicate tiles
				return;
			}
			const t = {
				tile: tile,
				cx: #round(#to_float(parent.tile_width) * cx),
				cy: #round(#to_float(parent.tile_height) * cy),
			};

			this.el.widget({
				class: 'base-screen-middle-area-tile',
				data: {
					tile: tile,
				},
				left: c_left + t.cx,
				top: c_top + t.cy,
				type: 'tile-preview',
			});

			existing_tiles[key] = t;
			return t;
		};

		const f_base = () => {
			this.el.widget({
				type: 'base-preview',
				data: {
					base: data.base,
					no_badge: true,
				},
				width: parent.tile_width,
				height: parent.tile_height + 20,
				left: c_left,
				top: c_top - 20,
			});
		};

		const f_resources = (t) => {
			t.el_resources = this.el.widget({
				class: 'base-screen-middle-area-tile',
				data: {
					tile: t.tile,
				},
				left: c_left + t.cx,
				top: c_top + t.cy,
				type: 'tile-resources',
			});
		};

		// terrain
		const t_center = f_tile(data.base.get_tile(), 0.0, 0.0);
		const t_n = f_tile(t_center.tile.get_N(), 0.0, 0.0 - 1.0); // TODO: remove the need for 0.0 - ...
		const t_ne = f_tile(t_center.tile.get_NE(), 0.5, 0.0 - 0.5);
		const t_e = f_tile(t_center.tile.get_E(), 1.0, 0.0);
		const t_se = f_tile(t_center.tile.get_SE(), 0.5, 0.5);
		const t_s = f_tile(t_center.tile.get_S(), 0.0, 1.0);
		const t_sw = f_tile(t_center.tile.get_SW(), 0.0 - 0.5, 0.5);
		const t_w = f_tile(t_center.tile.get_W(), 0.0 - 1.0, 0.0);
		const t_nw = f_tile(t_center.tile.get_NW(), 0.0 - 0.5, 0.0 - 0.5);
		if (#is_defined(t_n)) {
			const t_n_nw = f_tile(t_n.tile.get_NW(), 0.0 - 0.5, 0.0 - 1.5);
			const t_n_ne = f_tile(t_n.tile.get_NE(), 0.5, 0.0 - 1.5);
		}
		if (#is_defined(t_ne)) {
			const t_ne_ne = f_tile(t_ne.tile.get_NE(), 1.0, 0.0 - 1.0);
		}
		const t_e_ne = f_tile(t_e.tile.get_NE(), 1.5, 0.0 - 0.5);
		const t_e_se = f_tile(t_e.tile.get_SE(), 1.5, 0.5);
		if (#is_defined(t_se)) {
			const t_se_se = f_tile(t_se.tile.get_SE(), 1.0, 1.0);
		}
		if (#is_defined(t_s)) {
			const t_s_se = f_tile(t_s.tile.get_SE(), 0.5, 1.5);
			const t_s_sw = f_tile(t_s.tile.get_SW(), 0.0 - 0.5, 1.5);
		}
		if (#is_defined(t_sw)) {
			const t_sw_sw = f_tile(t_sw.tile.get_SW(), 0.0 - 1.0, 1.0);
		}
		const t_w_sw = f_tile(t_w.tile.get_SW(), 0.0 - 1.5, 0.5);
		const t_w_nw = f_tile(t_w.tile.get_NW(), 0.0 - 1.5, 0.0 - 0.5);
		if (#is_defined(t_nw)) {
			const t_nw_nw = f_tile(t_nw.tile.get_NW(), 0.0 - 1.0, 0.0 - 1.0);
		}

		// base
		f_base();

		// resources
		f_resources(t_center);
		for (tile of data.base.get_worked_tiles()) {
			const key = f_tile_key(tile);
			if (#is_defined(existing_tiles[key])) {
				f_resources(existing_tiles[key]);
			}
		}

		this.el.on('mousedown', (e) => {
			if (e.button == 'left') {
				// check if any tile was clicked
				const twh = this.tile_width / 2;
				const mx = e.x - c_left - twh;
				const my = e.y - c_top - this.tile_height / 2;
				const f_base_find_best_or_worst_tiles = this.p.game.get('f_base_find_best_or_worst_tiles');
				for (t of existing_tiles) {
					const distance = #abs(t.cx - mx) + #abs(t.cy - my) * tile_aspect_ratio;
					if (distance <= twh) {
						if (t == t_center) {
							let pops = data.base.get_pops();
							const tiles = f_base_find_best_or_worst_tiles(data.base, data.base.get_workable_tiles(), #sizeof(pops), 1);
							let i = 0;
							// TODO: optimize excessive unwork/work events
							// unwork all tiles
							for (pop of pops) {
								const worked_tile = pop.get('worked_tile');
								if (#is_defined(worked_tile)) {
									this.p.game.event('unwork_base_tile', {
										base: data.base,
										tile: worked_tile,
									});
								}
							}
							// work best tiles
							for (pop of pops) {
								const tile = tiles[i++];
								if (!#is_defined(tile)) {
									break;
								}
								this.p.game.event('work_base_tile', {
									base: data.base,
									tile: tile,
									pop: pop,
								});
							}
						} else if (#is_defined(t.el_resources)) {
							this.p.game.event('unwork_base_tile', {
								base: data.base,
								tile: t.tile,
							});
						} else {
							let target_pop = null;
							for (pop of data.base.get_pops()) {
								if (pop.get_type() != 'WORKER') {
									target_pop = pop;
									break;
								}
							}
							if (target_pop == null) {
								// find worst worked tile
								const tile = (f_base_find_best_or_worst_tiles(data.base, data.base.get_worked_tiles(), 1, 0 - 1))[0]; // TODO: 0 - 1
								if (#is_defined(tile)) {
									target_pop = tile.get('working_pop');
								}
							}
							if (target_pop != null) {
								this.p.game.event('work_base_tile', {
									base: data.base,
									tile: t.tile,
									pop: target_pop,
								});
							}
						}
						break;
					}
				}
			}
			return true;
		});
	},

};
