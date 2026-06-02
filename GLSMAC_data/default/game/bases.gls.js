const pops = #include('pops');

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
		let pop = null;
		// try to remove non-worker pop first
		for (p of base.get_pops()) {
			if (!p.has('worked_tile')) {
				pop = p;
				break;
			}
		}
		if (pop == null) {
			const worst_tile = (find_best_or_worst_tiles(base, base.get_worked_tiles(), 1, 0 - 1))[0];
			if (#is_defined(worst_tile)) {
				const p = worst_tile.get('working_pop');
				if (#is_defined(p)) {
					pop = p;
				} else {
					#print('bug: could not find pop of worked tile');
				}
			} else {
				#print('bug: could not find worst tile for depopulation');
			}
		}
		game.event('remove_base_pop', {
			base: base,
			pop: pop,
		});
		return;
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

	const bm = game.get_bm();

	bm.on('get_base_intake', (e) => {
		let result = {
			NUTRIENTS: 0,
			MINERALS: 0,
			ENERGY: 0,
		};

		const f_add_tile = (tile) => {
			const r = tile.get_resources();
			result.NUTRIENTS = result.NUTRIENTS + r.NUTRIENTS;
			result.MINERALS = result.MINERALS + r.MINERALS;
			result.ENERGY = result.ENERGY + r.ENERGY;
		};

		f_add_tile(e.base.get_tile());
		for (tile of e.base.get_worked_tiles()) {
			f_add_tile(tile);
		}

		return result;
	});

	bm.on('get_base_consumption', (e) => {
		let result = {
			NUTRIENTS: e.base.get_size() * 2, // 2 nutrients per pop
			MINERALS: 0,
			ENERGY: 0,
		};

		// TODO: supported units

		return result;
	});

	bm.on('get_base_workable_tiles', (e) => {
		let result = [];
		const t_center = e.base.get_tile();
		let added_tiles = {};
		const f_add = (tile) => {
			const key = #to_string(tile.x) + '_' + #to_string(tile.y);
			if (tile != t_center && !#is_defined(added_tiles[key])) {
				added_tiles[key] = true;
				result :+tile;
			}
			return tile;
		};
		const t_n = f_add(t_center.get_N());
		const t_ne = f_add(t_center.get_NE());
		const t_e = f_add(t_center.get_E());
		const t_se = f_add(t_center.get_SE());
		const t_s = f_add(t_center.get_S());
		const t_sw = f_add(t_center.get_SW());
		const t_w = f_add(t_center.get_W());
		const t_nw = f_add(t_center.get_NW());
		const t_n_nw = f_add(t_n.get_NW());
		const t_n_ne = f_add(t_n.get_NE());
		const t_ne_ne = f_add(t_ne.get_NE());
		const t_e_ne = f_add(t_e.get_NE());
		const t_e_se = f_add(t_e.get_SE());
		const t_se_se = f_add(t_se.get_SE());
		const t_s_se = f_add(t_s.get_SE());
		const t_s_sw = f_add(t_s.get_SW());
		const t_sw_sw = f_add(t_sw.get_SW());
		const t_w_sw = f_add(t_w.get_SW());
		const t_w_nw = f_add(t_w.get_NW());
		const t_nw_nw = f_add(t_nw.get_NW());
		return result;
	});

	pops.define(game);

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
