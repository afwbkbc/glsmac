const faction = {
	name: 'Gaians',
	colors: {
		text: #to_color(16, 228, 0),
		border: #to_color(0, 252, 0),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'gaians.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
		names: #game.factions.import_base_names('gaians.txt'),
	},
};

return faction;