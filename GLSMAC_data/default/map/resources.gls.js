const define = (id, levels_y, minusplus_y) => {

	return; // TODO
	
	#game.map.define_resource(id, {
		name: id,
		render: {
			type: 'sprite',
			file: 'newicons.pcx',
			levels: {
				grid_x: 174, grid_y: levels_y, grid_margin: 1,
				cell_width: 40, cell_height: 40, cell_count: 8,
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
	define: () => {
		define('Nutrients', 304, 13);
		define('Minerals', 345, 36);
		define('Energy', 386, 59);
	},
};

return result;
