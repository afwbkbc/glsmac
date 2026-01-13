const define = (game, id, levels_y, minusplus_y) => {

	game.event('define_resource', {
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

	configure: (game) => {

		game.rm.on('get_yield', (e) => {

			if (e.resource == 'Nutrients') {
				if (!e.tile.features.xenofungus) {
					if (e.tile.is_land) {
						return e.tile.moisture - 1;
					} else {
						return 1;
					}
				} else {
					// TODO: fungus tiles
				}
			}
			if (e.resource == 'Minerals') {
				if (!e.tile.features.xenofungus) {
					if (e.tile.is_land) {
						let result = 0;
						if (e.tile.rockiness > 1) {
							result = result + 1;
						}
						return result;
					} else {
						return 0;
					}
				} else {
					// TODO: fungus tiles
				}
			}
			if (e.resource == 'Energy') {
				if (!e.tile.features.xenofungus) {
					if (e.tile.is_land) {
						let result = e.tile.elevation / 1000;
						if (e.tile.features.river) {
							result = result + 1;
						}
						return result;
					} else {
						return 1;
					}
				} else {
					// TODO: fungus tiles
				}
			}

			// unknown resource
			return 0;

		});
	},

	define: (game) => {
		define(game, 'Nutrients', 304, 13);
		define(game, 'Minerals', 345, 35);
		define(game, 'Energy', 386, 59);
	},

};

return result;
