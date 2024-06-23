return {
	name: 'Peacekeepers',
	colors: {
		text: #to_color(164, 176, 232),
		border: #to_color(164, 176, 232),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'peace.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
	},
};
