const globals = {};

const get_nutrients_for_growth = (game, base) => {
	return globals.map_growth_base * (base.get_size() + 1);
};

const get_pending_growth = (base) => {
	const intake = base.get_intake();
	const consumption = base.get_consumption();
	return intake.NUTRIENTS - consumption.NUTRIENTS;
};

const reset_nutrients = (game, base) => {
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
};

const get_tile_score = (base, tile) => {
	const resources = tile.get_resources(base.get_owner());
	return resources.NUTRIENTS * 3 + resources.MINERALS * 2 + resources.ENERGY; // simple logic for now, TODO: improve based on what base needs
};

const find_best_or_worst_tiles = (base, tiles, count, modifier) => { // modifier 1 to find best tiles, -1 to find worst tiles
	let keys = {};
	let result = [];
	for (let i = 0; i < count; i++) {
		if (i >= #sizeof(tiles)) {
			break;
		}
		let best = null;
		for (tile of tiles) {
			const key = #to_string(tile.x) + '_' + #to_string(tile.y);
			if (#is_defined(keys[key])) {
				continue;
			}
			const score = get_tile_score(base, tile) * modifier;
			if (best == null || score > best.score) {
				best = {
					tile: tile,
					key: key,
					score: score,
				};
			}
		}
		if (best != null) {
			keys[best.key] = true;
			result :+best.tile;
		} else {
			break;
		}
	}
	return result;
};

const process_growth = (game, base) => {
	let grow = false;

	let accumulated = base.get('accumulated_nutrients');
	if (!#is_defined(accumulated)) {
		accumulated = 0;
	}
	accumulated += get_pending_growth(base);
	if (accumulated < 0) {
		// TODO: hunger
		accumulated = 0;
	}
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
		const best_tile = (find_best_or_worst_tiles(base, base.get_unworked_tiles(), 1, 1))[0];
		if (best_tile != null) {
			// found tile to work, spawn worker
			// TODO: talents logic
			game.event('add_base_pop', {
				base: base,
				type: 'WORKER',
				worked_tile: best_tile,
			});
		} else {
			// all tiles already worked, spawn doctor
			game.event('add_base_pop', {
				base: base,
				type: 'DOCTOR',
			});
		}
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

const pop_work_tile = (base, pop, tile) => {
	pop_unwork(base, pop);
	pop.set_type('WORKER');
	pop.set('worked_tile', tile);
	tile.set('working_pop', pop);
	base.add_worked_tile(tile);
};

const pop_unwork = (base, pop, new_type) => {
	const tile = pop.get('worked_tile');
	if (#is_defined(tile)) {
		tile.unset('working_pop');
		pop.unset('worked_tile');
		base.remove_worked_tile(tile);
		if (#is_defined(new_type)) {
			pop.set_type(new_type);
		}
	}
};

return (game) => {

	game.on('start', (e) => {

		calculate_growth_base(game);

		const bm = game.get_bm();

		// set bases-related globals
		// TODO: prettier way to do this? needs to be callable from events
		game.set('f_base_get_pending_growth', get_pending_growth);
		game.set('f_base_reset_nutrients', reset_nutrients);
		game.set('f_base_pop_work_tile', pop_work_tile);
		game.set('f_base_pop_unwork_tile', pop_unwork);
		game.set('f_base_find_best_or_worst_tiles', find_best_or_worst_tiles);

		// new turn, process all bases
		game.on('turn', (e) => {
			for (base of bm.get_bases()) {

				process_growth(game, base);

			}
		});

	});
};
