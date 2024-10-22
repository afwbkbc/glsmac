const define_pop = (id, name, renders_human_x, renders_progenitor_x) => {
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
	});
};

const result = {
	define: () => {

		define_pop('Worker', 'Worker', [79, 118], [40]);
		define_pop('Talent', 'Talent', [1, 40], [1]);
		define_pop('Doctor', 'Doctor', [352], [196]);
		define_pop('Technician', 'Technician', [313], [157]);
		define_pop('Librarian', 'Librarian', [391], [235]);
		define_pop('Engineer', 'Engineer', [430], [274]);
		define_pop('Empath', 'Empath', [469], [313]);
		define_pop('Thinker', 'Thinker', [508], [352]);
		define_pop('Transcend', 'Transcend', [547], [391]);
		define_pop('Drone', 'Drone', [157, 196], [79]);
		define_pop('DronePlus', 'Drone', [235, 274], [118]);

	},
};

return result;
