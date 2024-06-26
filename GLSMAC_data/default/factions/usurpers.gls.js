return {
	name: 'Usurpers',
	colors: {
		text: #to_color(212, 208, 116),
		border: #to_color(212, 208, 116),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'usurper.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
		names: #game.factions.import_base_names('usurper.txt'),
	},
	is_progenitor: true,
};
