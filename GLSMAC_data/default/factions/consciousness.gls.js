return {
	name: 'Consciousness',
	colors: {
		text: #to_color(44, 128, 104),
		border: #to_color(44, 128, 104),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'cyborg.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
		names: #game.factions.import_base_names('cyborg.txt'),
	},
};
