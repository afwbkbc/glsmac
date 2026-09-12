const result = {
	define: (game) => {

		game.event('define_base_facility', {
			id: 'RECYCLING_TANKS',
			def: {
				name: 'Recycling Tanks',
				description: 'Increases NUTRIENT, MINERALS and ENERGY output of base square.',
				cost: 32,
				render: {
					type: 'sprite',
					file: 'facs/fac003.pcx',
					x: 0,
					y: 0,
					w: 215,
					h: 215,
				},
			},
		});

	},
};

return result;
