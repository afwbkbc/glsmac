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

		define(game, 'WORKER', 'Worker', [79, 118], [40], {tile_worker: true});
		define(game, 'TALENT', 'Talent', [1, 40], [1], {tile_worker: true});
		define(game, 'DOCTOR', 'Doctor', [352], [196], {});
		define(game, 'TECHNICIAN', 'Technician', [313], [157], {});
		define(game, 'LIBRARIAN', 'Librarian', [391], [235], {});
		define(game, 'ENGINEER', 'Engineer', [430], [274], {});
		define(game, 'EMPATH', 'Empath', [469], [313], {});
		define(game, 'THINKER', 'Thinker', [508], [352], {});
		define(game, 'TRANSCEND', 'Transcend', [547], [391], {});
		define(game, 'DRONE', 'Drone', [157, 196], [79], {});
		define(game, 'DRONEPLUS', 'Drone', [235, 274], [118], {});

	},
};

return result;
