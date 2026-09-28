const define = (game, id, coords) => {

	game.event('define_resource', {
		name: id,
		render: {
			type: 'sprites',
			file: 'newicons.pcx',
			coords: coords,
		}
	});

};

const calculateXenoBonuses = (player) => {
	let bonuses = {
		NUTRIENTS: 0,
		MINERALS: 0,
		ENERGY: 0,
	};

	// TODO: calculated real fungus resources (checking specific techs).
	// NOTE: player object (game::backend::Player) does not expose techs yet.
	// Using a constant placeholder value right now.
	// See also: https://alphacentauri.miraheze.org/wiki/Fungus/Xenofungus
	//   +1 Nutrient each for Centauri Ecology and Centauri Psi
	//   +1 Mineral each for Centauri Genetics, Matter Transmission, and Threshold of Transcendence
	//   +1 Energy each for Centauri Meditation, Secrets of Alpha Centauri, and Temporal Mechanics
	bonuses.NUTRIENTS = 1;

	return bonuses;
};

const result = {

	configure: (game) => {

		game.get_tm().on('get_tile_resources', (e) => {
			let result = {
				NUTRIENTS: 0,
				MINERALS: 0,
				ENERGY: 0,
			};

			if (e.tile.features.xenofungus) {
				const bonuses = calculateXenoBonuses(e.player);
				result.NUTRIENTS = bonuses.NUTRIENTS;
				result.MINERALS = bonuses.MINERALS;
				result.ENERGY = bonuses.ENERGY;
			} else {

				// nutrients
				if (e.tile.is_land) {
					if (e.tile.rockiness < 3) {
						result.NUTRIENTS = e.tile.moisture - 1;
					}
					if (e.tile.features.jungle) {
						result.NUTRIENTS = result.NUTRIENTS + 1;
					}
				} else {
					result.NUTRIENTS = 1;
				}
				if (e.tile.bonuses.nutrient) {
					result.NUTRIENTS = result.NUTRIENTS + 2;
				}

				// minerals
				if (e.tile.is_land) {
					if (e.tile.rockiness > 1) {
						result.MINERALS = 1;
					}
				}
				if (e.tile.bonuses.minerals) {
					result.MINERALS = result.MINERALS + 2;
				}

				// energy
				if (e.tile.is_land) {
					//result.ENERGY = e.tile.elevation / 1000; // TODO: only with solar collector
					if (e.tile.features.river) {
						result.ENERGY = result.ENERGY + 1; // TODO: fix += with properties
					}
				} else {
					result.ENERGY = 1;
				}
				if (e.tile.bonuses.energy) {
					result.ENERGY = result.ENERGY + 2;
				}

			}

			// base
			const base = e.tile.get_base();
			if (base != null) {
				// TODO: reuse terraforming logic
				if (result.NUTRIENTS < 2) {
					result.NUTRIENTS = 2;
				}
				if (result.MINERALS < 1) {
					result.MINERALS = 1;
				}
				const min_energy = e.tile.features.river ? 2 : 1;
				if (result.ENERGY < min_energy) {
					result.ENERGY = min_energy;
				}
				if (base.has_facility('HEADQUARTERS')) {
					result.ENERGY = result.ENERGY + 1;
				}
			} else {
				// TODO: terraforming
			}

			// TODO: tech-based limitations

			return result;
		});

	},

	define: (game) => {
		define(game, 'NUTRIENTS', [
			[184, 314, 201, 331],
			[223, 312, 245, 334],
			[262, 310, 288, 336],
			[301, 308, 331, 338],
			[339, 305, 376, 342],
			[380, 305, 417, 342],
			[421, 305, 458, 342],
			[462, 305, 499, 342],
		]);
		define(game, 'MINERALS', [
			[185, 356, 202, 373],
			[223, 355, 245, 377],
			[263, 353, 290, 378],
			[302, 350, 332, 380],
			[339, 346, 376, 383],
			[380, 346, 417, 383],
			[421, 346, 458, 383],
			[462, 346, 499, 383],
		]);
		define(game, 'ENERGY', [
			[186, 397, 202, 414],
			[224, 395, 245, 417],
			[262, 392, 289, 419],
			[301, 390, 332, 421],
			[339, 387, 376, 424],
			[380, 387, 417, 424],
			[421, 387, 458, 424],
			[462, 387, 499, 424],
		]);
		game.event('define_no_resource', {
			render: {
				type: 'sprites',
				file: 'newicons.pcx',
				coords: [2, 175, 22, 194],
			}
		});

	},

};

return result;
