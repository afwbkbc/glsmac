const define_bases = #include('../default/game/bases');

const callbacks = {};
const values = {};
let events = [];
const game = {
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
		events :+{name: name, data: data};
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
events = [];

const owner = {id: 1};
const make_tile = (x, nutrients, minerals, energy) => {
	let working_pop = #undefined;
	let tile_base = null;
	return {
		x: x,
		y: 0,
		get_resources: (player) => {
			test.assert(player == owner);
			return {
				NUTRIENTS: nutrients,
				MINERALS: minerals,
				ENERGY: energy,
			};
		},
		get: (key) => {
			return key == 'working_pop' ? working_pop : #undefined;
		},
		has: (key) => {
			return key == 'working_pop' && #is_defined(working_pop);
		},
		get_base: () => { return tile_base; },
		set_base: (base) => { tile_base = base; },
		set_working_pop: (pop) => {
			working_pop = pop;
		},
	};
};

const mineral_tile = make_tile(1, 0, 4, 0);
const nutrient_tile = make_tile(2, 2, 0, 0);
const occupied_tile = make_tile(3, 9, 9, 9);
const center_tile = make_tile(4, 9, 9, 9);
occupied_tile.set_working_pop({id: 99});
center_tile.set_base({id: 99});
let accumulated_nutrients = 20;
let base_size = 1;
let intake_nutrients = 2;
let consumption_nutrients = 2;
let unworked_tiles = [mineral_tile, nutrient_tile, occupied_tile, center_tile];
let worked_tiles = [];
let pops = [];
let facilities = [];
const base = {
	get_owner: () => { return owner; },
	get_size: () => { return base_size; },
	get_intake: () => { return {NUTRIENTS: intake_nutrients, MINERALS: 1, ENERGY: 1}; },
	get_consumption: () => { return {NUTRIENTS: consumption_nutrients, MINERALS: 0, ENERGY: 0}; },
	get_unworked_tiles: () => { return unworked_tiles; },
	get_workable_tiles: () => { return unworked_tiles; },
	get_worked_tiles: () => { return worked_tiles; },
	get_pops: () => { return pops; },
	get_facilities: () => { return facilities; },
	get: (key) => {
		test.assert(key == 'accumulated_nutrients');
		return accumulated_nutrients;
	},
	set: (key, value) => {
		test.assert(key == 'accumulated_nutrients');
		accumulated_nutrients = value;
	},
};

const find_tiles = values.f_base_find_best_or_worst_tiles;
const get_assignable_tiles = values.f_base_get_assignable_worker_tiles;
test.assert(get_assignable_tiles(base) == [mineral_tile, nutrient_tile]);
const own_pop = {id: 1};
mineral_tile.set_working_pop(own_pop);
pops = [own_pop];
test.assert(get_assignable_tiles(base) == [mineral_tile, nutrient_tile]);
mineral_tile.set_working_pop(#undefined);
pops = [];
test.assert((find_tiles(base, [mineral_tile, nutrient_tile], 1, 1))[0] == mineral_tile);
test.assert((find_tiles(base, [mineral_tile, nutrient_tile], 1, 1, 2))[0] == nutrient_tile);
let reserved_tiles = {};
reserved_tiles['2_0'] = true;
test.assert((find_tiles(base, [mineral_tile, nutrient_tile], 1, 1, 2, true, reserved_tiles))[0] == mineral_tile);

values.f_base_process_growth(game, base);
test.assert(#sizeof(events) == 1);
test.assert(events[0].name == 'add_base_pop');
test.assert(events[0].data.base == base);
test.assert(events[0].data.type == 'WORKER');
test.assert(events[0].data.worked_tile == nutrient_tile);

events = [];
facilities = [{growth_rating_bonus: 2}];
accumulated_nutrients = 15;
values.f_base_process_growth(game, base);
test.assert(events == []);
test.assert(values.f_base_get_nutrients_for_growth(game, base) == 16);
callbacks.turn({});
accumulated_nutrients = 16;
values.f_base_process_growth(game, base);
test.assert(#sizeof(events) == 1);
test.assert(events[0].name == 'add_base_pop');
facilities = [];

values.f_project_get_effects = (target_base) => {
	test.assert(target_base == base);
	return {
		growth_rating_bonus: 10,
		population_limit_bonus: 2,
	};
};
base_size = 1;
test.assert(values.f_base_get_nutrients_for_growth(game, base) == 2);
base_size = 7;
test.assert(values.f_base_get_population_limit(base) == 9);
values.f_project_get_effects = #undefined;

events = [];
base_size = 7;
accumulated_nutrients = 200;
values.f_base_process_growth(game, base);
test.assert(events == []);
test.assert(accumulated_nutrients == 80);
test.assert(values.f_base_get_population_limit(base) == 7);

facilities = [{population_limit: 14}];
values.f_base_process_growth(game, base);
test.assert(#sizeof(events) == 1);
test.assert(events[0].name == 'add_base_pop');
test.assert(values.f_base_get_population_limit(base) == 14);
facilities = [];

const mineral_pop = {has: (key) => { return key == 'worked_tile'; }};
const nutrient_pop = {has: (key) => { return key == 'worked_tile'; }};
mineral_tile.set_working_pop(mineral_pop);
nutrient_tile.set_working_pop(nutrient_pop);
events = [];
base_size = 2;
intake_nutrients = 2;
consumption_nutrients = 4;
unworked_tiles = [];
worked_tiles = [mineral_tile, nutrient_tile];
pops = [mineral_pop, nutrient_pop];
accumulated_nutrients = 0 - 1;

values.f_base_process_growth(game, base);
test.assert(#sizeof(events) == 1);
test.assert(events[0].name == 'remove_base_pop');
test.assert(events[0].data.base == base);
test.assert(events[0].data.pop == mineral_pop);

const specialist_pop = {has: (key) => { return false; }};
pops = [mineral_pop, specialist_pop];
test.assert(values.f_base_select_population_for_reduction(base) == specialist_pop);
pops = [mineral_pop, nutrient_pop];
test.assert(values.f_base_select_population_for_reduction(base) == mineral_pop);

events = [];
base_size = 1;
intake_nutrients = 0;
consumption_nutrients = 2;
worked_tiles = [mineral_tile];
pops = [mineral_pop];
accumulated_nutrients = 0 - 1;

values.f_base_process_growth(game, base);
test.assert(#sizeof(events) == 0);
test.assert(accumulated_nutrients == 0);

let worker_tile = mineral_tile;
let worker_type = 'WORKER';
const worker = {
	id: 1,
	has: (key) => { return key == 'worked_tile' && #is_defined(worker_tile); },
	get: (key) => { return key == 'worked_tile' ? worker_tile : #undefined; },
	get_type: () => { return worker_type; },
	set_type: (type) => { worker_type = type; },
};
mineral_tile.set_working_pop(worker);
nutrient_tile.set_working_pop(#undefined);
worked_tiles = [mineral_tile];
unworked_tiles = [nutrient_tile];
const rebalance_base = {
	get_owner: () => { return owner; },
	get_size: () => { return 2; },
	get_intake: () => {
		const resources = #is_defined(worker_tile)
			? worker_tile.get_resources(owner)
			: {NUTRIENTS: 0, MINERALS: 0, ENERGY: 0};
		return {NUTRIENTS: 2 + resources.NUTRIENTS, MINERALS: resources.MINERALS, ENERGY: resources.ENERGY};
	},
	get_consumption: () => { return {NUTRIENTS: 4, MINERALS: 0, ENERGY: 0}; },
	get_unworked_tiles: () => { return unworked_tiles; },
	get_worked_tiles: () => { return worked_tiles; },
	get_pops: () => { return [worker]; },
	is_tile_worked: (tile) => { return #sizeof(worked_tiles) > 0 && worked_tiles[0] == tile; },
	unwork_pop_tile: (pop, tile) => {
		test.assert(pop == worker && tile == worker_tile);
		tile.set_working_pop(#undefined);
		worked_tiles = [];
		worker_tile = #undefined;
	},
	work_pop_tile: (pop, tile) => {
		test.assert(pop == worker && !tile.has('working_pop'));
		worker_tile = tile;
		tile.set_working_pop(pop);
		worked_tiles = [tile];
		unworked_tiles = [mineral_tile];
	},
};

values.f_base_rebalance_workers(rebalance_base);
test.assert(worker_tile == nutrient_tile);
test.assert(worked_tiles == [nutrient_tile]);
test.assert(rebalance_base.get_intake().NUTRIENTS == rebalance_base.get_consumption().NUTRIENTS);

values.f_base_rebalance_workers(rebalance_base, 0);
test.assert(!#is_defined(worker_tile));
test.assert(#sizeof(worked_tiles) == 0);
test.assert(worker_type == 'DOCTOR');

values.f_base_rebalance_workers(rebalance_base, 1);
test.assert(worker_tile == mineral_tile);
test.assert(worked_tiles == [mineral_tile]);
test.assert(worker_type == 'WORKER');
