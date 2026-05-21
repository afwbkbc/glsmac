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

		const rm = game.get_rm();

		rm.on('get_tile_resources', (e) => {
			let result = {
				NUTRIENTS: 0,
				MINERALS: 0,
				ENERGY: 0,
			};

			// nutrients
			if (!e.tile.features.xenofungus) {
				if (e.tile.is_land) {
					result.NUTRIENTS = e.tile.moisture - 1;
				} else {
					result.NUTRIENTS = 1;
				}
			} else {
				// TODO: fungus tiles
			}

			// minerals
			if (!e.tile.features.xenofungus) {
				if (e.tile.is_land) {
					if (e.tile.rockiness > 1) {
						result.MINERALS = 1;
					}
				} else {

				}
			} else {
				// TODO: fungus tiles
			}

			// energy
			if (!e.tile.features.xenofungus) {
				if (e.tile.is_land) {
					result.ENERGY = e.tile.elevation / 1000;
					if (e.tile.features.river) {
						result.ENERGY = result.ENERGY + 1; // TODO: fix += with properties
					}
				} else {
					result.ENERGY = 1;
				}
			} else {
				// TODO: fungus tiles
			}

			// TODO: bonuses

			// TODO: terraforming

			return result;
		});

	},

	define: (game) => {
		define(game, 'NUTRIENTS', 304, 13);
		define(game, 'MINERALS', 345, 35);
		define(game, 'ENERGY', 386, 59);
	},

};

return result;
