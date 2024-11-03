const define = (id, name, renders_human_x, renders_progenitor_x, properties) => {
	// TODO: if (!properties) { ... }
	let rh = [];
	for ( x of renders_human_x ) {
		rh []= {
			type: 'sprite',
			file: 'newicons.pcx',
			x: x, y: 501,
			w: 38, h: 48,
		};
	}
	let rp = [];
	for ( x of renders_progenitor_x ) {
		rp []= {
			type: 'sprite',
			file: 'aliencit.pcx',
			x: x, y: 41,
			w: 38, h: 48,
		};
	}
	#game.bases.define_pop(id, {
		name: name,
		renders_human: rh,
		renders_progenitor: rp,
	} + properties);
};

const result = {
	define: () => {

		define('Worker', 'Worker', [79, 118], [40], {tile_worker: true});
		define('Talent', 'Talent', [1, 40], [1], {tile_worker: true});
		define('Doctor', 'Doctor', [352], [196], {});
		define('Technician', 'Technician', [313], [157], {});
		define('Librarian', 'Librarian', [391], [235], {});
		define('Engineer', 'Engineer', [430], [274], {});
		define('Empath', 'Empath', [469], [313], {});
		define('Thinker', 'Thinker', [508], [352], {});
		define('Transcend', 'Transcend', [547], [391], {});
		define('Drone', 'Drone', [157, 196], [79], {});
		define('DronePlus', 'Drone', [235, 274], [118], {});

	},
};

return result;
