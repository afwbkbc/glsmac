const globals = {};

const get_nutrients_for_growth = (game, base) => {
	return globals.map_growth_base * (base.get_size() + 1);
};

const get_nutrients_per_turn = (base) => {
	return 2; // TODO: yields
};

const process_growth = (game, base) => {
	let grow = false;

	let accumulated = base.get('accumulated_nutrients');
	if (!#is_defined(accumulated)) {
		accumulated = 0;
	}
	accumulated += get_nutrients_per_turn(base);
	base.set('accumulated_nutrients', accumulated);
	if (base.get_size() == 0) {
		grow = true; // always grow new bases to 1
	}

	if (!grow) {
		if (accumulated >= get_nutrients_for_growth(game, base)) {
			grow = true; // growth from nutrients
		}
	}

	if (grow) {
		game.event('add_base_pop', {
			base: base,
			type: 'Worker',
		});
	}
};

const calculate_growth_base = (game) => {
	const tm = game.get_tm();
	let w = tm.get_map_width();
	let h = tm.get_map_height();

	let map_size = w * h;

	// rough approximations similar to SMAC's standard/small/tiny logic but using GLSMAC wider map ratio
	// used for base growth calculations
	let map_growth_base = 0;
	if (map_size > 5500) {
		map_growth_base = 15; // standard map
	} else if (map_size > 4000) {
		map_growth_base = 14; // between standard and small map, SMAC skips it but makes sense imo
	} else if (map_size > 3000) {
		map_growth_base = 13; // small map
	} else if (map_size > 1800) {
		map_growth_base = 12; // tiny map
	} else if (map_size > 1000) {
		map_growth_base = 11; // below tiny (SMAC doesn't have it but let's do)
	} else {
		map_growth_base = 10; // below tiny (SMAC doesn't have it but let's do)
	}
	globals.map_growth_base = map_growth_base;
	game.set('map_growth_base', map_growth_base);
};

return (game) => {

	game.on('start', (e) => {

		calculate_growth_base(game);

		const bm = game.get_bm();

		// set bases-related globals
		game.set('f_base_get_nutrients_per_turn', get_nutrients_per_turn);
		game.set('f_base_reset_nutrients', (base) => { // TODO: prettier way to do this? needs to be callable from events
			const nfg = get_nutrients_for_growth(game, base);
			let accumulated = base.get('accumulated_nutrients');
			if (!#is_defined(accumulated)) {
				accumulated = 0;
			}
			let updated = accumulated - nfg;
			if (updated < 0) {
				updated = 0;
			}
			base.set('accumulated_nutrients', updated);
		});

		// new turn, process all bases
		game.on('turn', (e) => {
			for (base of bm.get_bases()) {

				process_growth(game, base);

			}
		});

	});
};
