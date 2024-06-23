return {
	name: 'Spartans',
	colors: {
		text: #to_color(136, 166, 166),
		border: #to_color(0, 0, 0),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'spartans.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
	},
};
