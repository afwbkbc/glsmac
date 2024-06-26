return {
	name: 'Angels',
	colors: {
		text: #to_color(103, 91, 181),
		border: #to_color(103, 91, 181),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'angels.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
		names: #game.factions.import_base_names('angels.txt'),
	},
};
