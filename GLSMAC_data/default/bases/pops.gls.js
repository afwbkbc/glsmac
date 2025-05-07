const define = (game, id, name, renders_human_x, renders_progenitor_x, properties) => {
	// TODO: if (!properties) { ... }
	let rh = [];
	for (x of renders_human_x) {
		rh :+{
			type: 'sprite',
			file: 'newicons.pcx',
			x: x, y: 501,
			w: 38, h: 48,
		};
	}
	let rp = [];
	for (x of renders_progenitor_x) {
		rp :+{
			type: 'sprite',
			file: 'aliencit.pcx',
			x: x, y: 41,
			w: 38, h: 48,
		};
	}
	game.event('define_base_pop', {
		id: id,
		def: {
			name: name,
			renders_human: rh,
			renders_progenitor: rp,
		} + properties
	});
};

const result = {
	define: (game) => {

		define(game, 'Worker', 'Worker', [79, 118], [40], {tile_worker: true});
		define(game, 'Talent', 'Talent', [1, 40], [1], {tile_worker: true});
		define(game, 'Doctor', 'Doctor', [352], [196], {});
		define(game, 'Technician', 'Technician', [313], [157], {});
		define(game, 'Librarian', 'Librarian', [391], [235], {});
		define(game, 'Engineer', 'Engineer', [430], [274], {});
		define(game, 'Empath', 'Empath', [469], [313], {});
		define(game, 'Thinker', 'Thinker', [508], [352], {});
		define(game, 'Transcend', 'Transcend', [547], [391], {});
		define(game, 'Drone', 'Drone', [157, 196], [79], {});
		define(game, 'DronePlus', 'Drone', [235, 274], [118], {});

	},
};

return result;
