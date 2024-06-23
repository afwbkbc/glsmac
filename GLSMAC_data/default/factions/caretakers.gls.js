return {
	name: 'Caretakers',
	colors: {
		text: #to_color(116, 156, 56),
		border: #to_color(116, 156, 56),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'caretake.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
	},
	is_progenitor: true,
};
