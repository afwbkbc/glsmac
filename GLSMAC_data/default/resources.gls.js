const define = (rm, id, levels_y, minusplus_y) => {

	rm.define(id, {
		name: id,
		render: {
			type: 'sprite_map',
			file: 'newicons.pcx',
			yields: {
				grid_x: 174, grid_y: levels_y, grid_margin: 1,
				cell_width: 40, cell_height: 40, cells_count: 8,
			},
			plus: {
				x: 24, y: minusplus_y,
				width: 22, height: 22,
			},
			minus: {
				x: 47, y: minusplus_y,
				width: 22, height: 22,
			},
		}
	});

};

const result = {
	define: (rm) => {

		define(rm, 'Nutrients', 304, 13);
		define(rm, 'Minerals', 345, 36);
		define(rm, 'Energy', 386, 59);

		rm.on('get_yields', (e) => {

			let result = {
				Nutrients: 0,
				Minerals: 0,
				Energy: 0,
			};

			// TODO: fix increments of properties in GSE

			if (e.tile.has_fungus) {
				if (e.player.faction.is_progenitor) { // tmp
					//result.Energy += 2;
					result.Energy = result.Energy + 2;
				}
			} else {
				if (e.tile.is_land) {
					//result.Nutrients += (e.tile.moisture - 1);
					result.Nutrients = result.Nutrients + (e.tile.moisture - 1);
					if (e.tile.rockiness > 1) {
						//result.Minerals++;
						result.Minerals = result.Minerals + 1;
					}
					if (e.tile.has_river) {
						result.Energy = result.Energy + 1;
					}
				} else {
					//result.Nutrients++;
					result.Nutrients = result.Energy + 1;
					if (e.player.faction.id == 'PIRATES') { // tmp
						//result.Minerals++;
						result.Minerals = result.Minerals + 1;
					}
				}
			}
			return result;
		});


	},
};

return result;
