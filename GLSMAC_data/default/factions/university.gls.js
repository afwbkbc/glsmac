return {
	name: 'University',
	colors: {
		text: #to_color(216, 224, 244),
		border: #to_color(255, 255, 255),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'univ.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
	},
};
