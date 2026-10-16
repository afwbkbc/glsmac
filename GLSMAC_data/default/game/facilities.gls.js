const def = (game, id, name, description, hint, cost, pcx) => {
	game.event('define_base_facility', {
		id: id,
		def: {
			name: name,
			description: description,
			hint: hint,
			cost: cost,
			render: {
				type: 'sprite',
				file: 'facs/' + pcx,
				x: 0,
				y: 0,
				w: 215,
				h: 215,
			},
		},
	});
};

const result = {
	define: (game) => {

		def(game, 'HEADQUARTERS',
			'Headquarters',
			'Administrative center of your colony. +1 ENERGY. No inefficiency. Enemy probe teams may not attempt mind control here.',
			'Efficiency',
			40,
			'fac001.pcx'
		);

		def(game, 'RECYCLING_TANKS',
			'Recycling Tanks',
			'Increases NUTRIENT, MINERALS and ENERGY output of base square.',
			'Bonus Resources',
			32,
			'fac003.pcx'
		);

	},
};

return result;
