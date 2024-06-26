return {
	name: 'Morganites',
	colors: {
		text: #to_color(255, 255, 0),
		border: #to_color(255, 255, 0),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'morgan.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
		names: #game.factions.import_base_names('morgan.txt'),
	},
};
