const pops = #include('bases/pops');

const result = {

	init: () => {
		//pops.init();
	},

	configure: (game) => {

		game.get_bm().on('get_base_intake', (e) => {
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

		game.get_bm().on('get_base_consumption', (e) => {
			let result = {
				NUTRIENTS: e.base.get_size() * 2, // 2 nutrients per pop
				MINERALS: 0,
				ENERGY: 0,
			};

			// TODO: supported units

			return result;
		});

	},

	define: (game) => {
		pops.define(game);
	},

};

return result;
