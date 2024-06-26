return {
	name: 'Drones',
	colors: {
		text: #to_color(173, 196, 192),
		border: #to_color(136, 12, 12),
	},
	bases: {
		render: {
			type: 'sprite_grid',
			file: 'drone.pcx',
			grid_x: 1, grid_y: 1,
			cell_width: 100, cell_height: 75,
			cell_padding: 1,
		},
		names: #game.factions.import_base_names('drone.txt'),
	},
};
