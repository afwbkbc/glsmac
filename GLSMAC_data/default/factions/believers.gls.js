return {
	name: 'Believers',
	colors: {
		text: #to_color(224, 156, 28),
		border: #to_color(236, 92, 0),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'believe.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
		names: #game.factions.import_base_names('believe.txt'),
	},
};
