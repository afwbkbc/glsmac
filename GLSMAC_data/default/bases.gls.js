const pops = #include('bases/pops');

const result = {

	init: () => {
		//pops.init();
	},

	configure: (game) => {

		const bm = game.get_bm();

		bm.on('get_base_intake', (e) => {
			let result = {
				NUTRIENTS: 0,
				MINERALS: 0,
				ENERGY: 0,
			};

			const f_add_tile = (tile) => {
				const r = tile.get_resources();
				result.NUTRIENTS = result.NUTRIENTS + r.NUTRIENTS;
				result.MINERALS = result.MINERALS + r.MINERALS;
				result.ENERGY = result.ENERGY + r.ENERGY;
			};

			f_add_tile(e.base.get_tile());
			for (tile of e.base.get_worked_tiles()) {
				f_add_tile(tile);
			}

			return result;
		});

		bm.on('get_base_consumption', (e) => {
			let result = {
				NUTRIENTS: e.base.get_size() * 2, // 2 nutrients per pop
				MINERALS: 0,
				ENERGY: 0,
			};

			// TODO: supported units

			return result;
		});

		bm.on('get_base_workable_tiles', (e) => {
			let result = [];
			const t_center = e.base.get_tile();
			let added_tiles = {};
			const f_add = (tile) => {
				const key = #to_string(tile.x) + '_' + #to_string(tile.y);
				if (tile != t_center && !#is_defined(added_tiles[key])) {
					added_tiles[key] = true;
					result :+tile;
				}
				return tile;
			};
			const t_n = f_add(t_center.get_N());
			const t_ne = f_add(t_center.get_NE());
			const t_e = f_add(t_center.get_E());
			const t_se = f_add(t_center.get_SE());
			const t_s = f_add(t_center.get_S());
			const t_sw = f_add(t_center.get_SW());
			const t_w = f_add(t_center.get_W());
			const t_nw = f_add(t_center.get_NW());
			const t_n_nw = f_add(t_n.get_NW());
			const t_n_ne = f_add(t_n.get_NE());
			const t_ne_ne = f_add(t_ne.get_NE());
			const t_e_ne = f_add(t_e.get_NE());
			const t_e_se = f_add(t_e.get_SE());
			const t_se_se = f_add(t_se.get_SE());
			const t_s_se = f_add(t_s.get_SE());
			const t_s_sw = f_add(t_s.get_SW());
			const t_sw_sw = f_add(t_sw.get_SW());
			const t_w_sw = f_add(t_w.get_SW());
			const t_w_nw = f_add(t_w.get_NW());
			const t_nw_nw = f_add(t_nw.get_NW());
			return result;
		});

	},

	define: (game) => {
		pops.define(game);
	},

};

return result;
