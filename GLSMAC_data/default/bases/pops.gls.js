const define = (bm, id, name, renders_human_x, renders_progenitor_x, properties) => {
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
	bm.define_pop(id, {
		name: name,
		renders_human: rh,
		renders_progenitor: rp,
	} + properties);
};

const result = {
	define: (bm) => {

		define(bm, 'Worker', 'Worker', [79, 118], [40], {tile_worker: true});
		define(bm, 'Talent', 'Talent', [1, 40], [1], {tile_worker: true});
		define(bm, 'Doctor', 'Doctor', [352], [196], {});
		define(bm, 'Technician', 'Technician', [313], [157], {});
		define(bm, 'Librarian', 'Librarian', [391], [235], {});
		define(bm, 'Engineer', 'Engineer', [430], [274], {});
		define(bm, 'Empath', 'Empath', [469], [313], {});
		define(bm, 'Thinker', 'Thinker', [508], [352], {});
		define(bm, 'Transcend', 'Transcend', [547], [391], {});
		define(bm, 'Drone', 'Drone', [157, 196], [79], {});
		define(bm, 'DronePlus', 'Drone', [235, 274], [118], {});

	},
};

return result;
