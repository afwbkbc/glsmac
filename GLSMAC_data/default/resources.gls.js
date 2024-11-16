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
	},
};

return result;
