const define_bases = #include('../default/game/bases');
const add_base_pop = #include('../default/game/event/add_base_pop');
const remove_base_pop = #include('../default/game/event/remove_base_pop');
const process_base_growth = #include('../default/game/event/process_base_growth');
const refresh_base_psych = #include('../default/game/event/refresh_base_psych');

const callbacks = {};
let current_psych = 0;
const values = {
	f_economy_get_base_psych: (game, base) => { return current_psych; },
};
let pending_events = [];
let game = null;
game = {
	get_bm: () => {
		return {
			on: (name, callback) => {},
			get_bases: () => { return []; },
		};
	},
	get_tm: () => {
		return {
			get_map_width: () => { return 20; },
			get_map_height: () => { return 10; },
		};
	},
	event: (name, data) => {
		pending_events :+{name: name, data: data};
	},
	on: (name, callback) => {
		callbacks[name] = callback;
	},
	set: (key, value) => {
		values[key] = value;
	},
	get: (key) => {
		return values[key];
	},
	is_master: () => { return true; },
};

define_bases(game);
callbacks.start({});
pending_events = [];
const base_process_psych = values.f_base_process_psych;
let observed_psych = 0 - 1;
values.f_base_process_psych = (target_game, base, psych) => {
	observed_psych = psych;
	base_process_psych(target_game, base, psych);
};

const drain_events = () => {
	let index = 0;
	while (index < #sizeof(pending_events)) {
		const queued = pending_events[index];
		const event = {caller: 0, game: game, data: queued.data};
		if (queued.name == 'add_base_pop') {
			add_base_pop.apply(event);
		} else if (queued.name == 'remove_base_pop') {
			remove_base_pop.apply(event);
		} else if (queued.name == 'refresh_base_psych') {
			refresh_base_psych.apply(event);
		} else {
			throw Error('Unexpected queued event: ' + queued.name);
		}
		index++;
	}
	pending_events = [];
};

const owner = {id: 1, type: 'ai'};
const make_tile = (x) => {
	let working_pop = #undefined;
	return {
		x: x,
		y: 0,
		get_resources: (player) => {
			return {NUTRIENTS: 0, MINERALS: 1, ENERGY: 0};
		},
		get_base: () => { return null; },
		has: (key) => { return key == 'working_pop' && #is_defined(working_pop); },
		get: (key) => { return key == 'working_pop' ? working_pop : #undefined; },
		set_working_pop: (pop) => { working_pop = pop; },
	};
};

const make_base = (initial_size, initial_nutrients, center_nutrients) => {
	let accumulated_nutrients = initial_nutrients;
	let pops = [];
	let tiles = [];
	let base = null;

	const make_pop = (type, tile) => {
		let current_type = type;
		let current_tile = tile;
		return {
			get_base: () => { return base; },
			get_type: () => { return current_type; },
			set_type: (value) => { current_type = value; },
			has: (key) => { return key == 'worked_tile' && #is_defined(current_tile); },
			get: (key) => { return key == 'worked_tile' ? current_tile : #undefined; },
			set_tile: (value) => { current_tile = value; },
		};
	};

	for (let i = 0; i < initial_size + 1; i++) {
		tiles :+make_tile(i + 1);
	}

	base = {
		get_owner: () => { return owner; },
		get_size: () => { return #sizeof(pops); },
		get_pops: () => { return pops; },
		get_intake: () => {
			let nutrients = center_nutrients;
			let minerals = 0;
			for (tile of tiles) {
				if (tile.has('working_pop')) {
					const resources = tile.get_resources(owner);
					nutrients += resources.NUTRIENTS;
					minerals += resources.MINERALS;
				}
			}
			return {NUTRIENTS: nutrients, MINERALS: minerals, ENERGY: 0};
		},
		get_consumption: () => {
			return {NUTRIENTS: #sizeof(pops) * 2, MINERALS: 0, ENERGY: 0};
		},
		get_worked_tiles: () => {
			let result = [];
			for (tile of tiles) {
				if (tile.has('working_pop')) {
					result :+tile;
				}
			}
			return result;
		},
		get_unworked_tiles: () => {
			let result = [];
			for (tile of tiles) {
				if (!tile.has('working_pop')) {
					result :+tile;
				}
			}
			return result;
		},
		is_tile_worked: (tile) => { return tile.has('working_pop'); },
		work_pop_tile: (pop, tile) => {
			pop.set_tile(tile);
			tile.set_working_pop(pop);
		},
		unwork_pop_tile: (pop, tile) => {
			tile.set_working_pop(#undefined);
			pop.set_tile(#undefined);
		},
		create_pop: (data) => {
			const pop = make_pop(data.type, #undefined);
			pops :+pop;
			return pop;
		},
		destroy_pop: (pop) => {
			let remaining = [];
			for (candidate of pops) {
				if (candidate != pop) {
					remaining :+candidate;
				}
			}
			pops = remaining;
		},
		get: (key) => { return key == 'accumulated_nutrients' ? accumulated_nutrients : #undefined; },
		set: (key, value) => {
			if (key == 'accumulated_nutrients') {
				accumulated_nutrients = value;
			}
		},
	};

	for (let pop_index = 0; pop_index < initial_size; pop_index++) {
		const pop = base.create_pop({type: 'WORKER'});
		base.work_pop_tile(pop, tiles[pop_index]);
	}
	return base;
};

const assert_stable_size_four = (base) => {
	values.f_base_rebalance_workers(base, values.f_base_get_stable_worker_count(base, 0));
	values.f_base_process_psych(game, base, 0);
	const state = values.f_base_get_psych(base);
	test.assert(base.get_size() == 4);
	test.assert(state.workers + state.talents == 3);
	test.assert(state.talents == 1);
	test.assert(state.specialists == 1);
	test.assert(state.drones == 0);
	test.assert(state.is_rioting == false);
};

const growing_base = make_base(3, 38, 8);
process_base_growth.apply({caller: 0, game: game, data: {base: growing_base, psych: 0}});
test.assert(growing_base.get_size() == 3);
test.assert(pending_events[0].name == 'add_base_pop');
test.assert(pending_events[1].name == 'refresh_base_psych');
current_psych = 2;
drain_events();
test.assert(observed_psych == 2);
test.assert(growing_base.get('accumulated_nutrients') == 0);
current_psych = 0;
assert_stable_size_four(growing_base);

const starving_base = make_base(5, 0, 0);
process_base_growth.apply({caller: 0, game: game, data: {base: starving_base, psych: 0}});
test.assert(starving_base.get_size() == 5);
test.assert(pending_events[0].name == 'remove_base_pop');
test.assert(pending_events[1].name == 'refresh_base_psych');
drain_events();
test.assert(starving_base.get('accumulated_nutrients') == 0);
assert_stable_size_four(starving_base);
