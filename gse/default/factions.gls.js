const faction = (id, name, text_color, border_color, pcx, extra) => {
	return [id, {
		name: name,
		colors: {
			text: text_color,
			border: border_color,
		},
		bases: {
			render: {
				type: 'sprite_grid',
				file: pcx,
				grid_x: 1, grid_y: 1,
				cell_width: 100, cell_height: 75,
				//cell_cx: 50, cell_cy: 38,
				cell_padding: 1,
				/*scale_x: 0.7,
				scale_y: 0.7,*/
			}
		},
	} + extra];
};

const factions = [
	faction('GAIANS', 'Gaians', #to_color(16, 228, 0), #to_color(0, 252, 0), 'gaians.pcx', {}),
	faction('HIVE', 'Hive', #to_color(0, 97, 255), #to_color(0, 0, 255), 'hive.pcx', {}),
	faction('UNIVERSITY', 'University', #to_color(216, 224, 244), #to_color(255, 255, 255), 'univ.pcx', {}),
	faction('MORGANITES', 'Morganites', #to_color(255, 255, 0), #to_color(255, 255, 0), 'morgan.pcx', {}),
	faction('SPARTANS', 'Spartans', #to_color(136, 166, 166), #to_color(0, 0, 0), 'spartans.pcx', {}),
	faction('BELIEVERS', 'Believers', #to_color(224, 156, 28), #to_color(236, 92, 0), 'believe.pcx', {}),
	faction('PEACEKEEPERS', 'Peacekeepers', #to_color(164, 176, 232), #to_color(164, 176, 232), 'peace.pcx', {}),
	faction('CONSCIOUSNESS', 'Consciousness', #to_color(44, 128, 104), #to_color(44, 128, 104), 'cyborg.pcx', {}),
	faction('PIRATES', 'Pirates', #to_color(0, 255, 255), #to_color(0, 255, 255), 'pirates.pcx', {}),
	faction('DRONES', 'Drones', #to_color(173, 196, 192), #to_color(136, 12, 12), 'drone.pcx', {}),
	faction('ANGELS', 'Angels', #to_color(103, 91, 181), #to_color(103, 91, 181), 'angels.pcx', {}),
	faction('PLANETCULT', 'Planet Cult', #to_color(232, 84, 84), #to_color(232, 84, 84), 'fungboy.pcx', {}),
	faction('CARETAKERS', 'Caretakers', #to_color(116, 156, 56), #to_color(116, 156, 56), 'caretake.pcx', {is_progenitor: true}),
	faction('USURPERS', 'Usurpers', #to_color(212, 208, 116), #to_color(212, 208, 116), 'usurper.pcx', {is_progenitor: true}),
];

return factions;
