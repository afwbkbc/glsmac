const add_pop = #include('../default/game/event/add_base_pop');
const remove_pop = #include('../default/game/event/remove_base_pop');

let calls = [];
let accumulated_nutrients = 0;
let created_pop = null;
let worked_pop = null;
let worked_tile = null;

const make_pop = (type, tile) => {
	return {
		get_type: () => {
			return type;
		},
		get: (key) => {
			if (key == 'worked_tile' && #is_defined(tile)) {
				return tile;
			}
			return #undefined;
		},
	};
};

const base = {
	get: (key) => {
		if (key == 'accumulated_nutrients') {
			return accumulated_nutrients;
		}
		return #undefined;
	},
	set: (key, value) => {
		if (key == 'accumulated_nutrients') {
			accumulated_nutrients = value;
		}
	},
	create_pop: (data) => {
		calls :+'create';
		created_pop = make_pop(data.type);
		return created_pop;
	},
	destroy_pop: (pop) => {
		calls :+'destroy';
	},
};

const helpers = {
	f_base_reset_nutrients: (game, target_base) => {
		calls :+'reset';
		target_base.set('accumulated_nutrients', 0);
	},
	f_base_pop_work_tile: (target_base, pop, tile) => {
		calls :+'work';
		worked_pop = pop;
		worked_tile = tile;
	},
	f_base_pop_unwork_tile: (target_base, pop) => {
		calls :+'unwork';
		worked_pop = null;
		worked_tile = null;
	},
};

const game = {
	get: (key) => {
		return helpers[key];
	},
};

const tile = {id: 'tile'};

accumulated_nutrients = 17;
calls = [];
let event = {
	caller: 0,
	game: game,
	data: {
		base: base,
		type: 'WORKER',
		worked_tile: tile,
	},
};
event.applied = add_pop.apply(event);
test.assert(calls == ['reset', 'create', 'work']);
test.assert(accumulated_nutrients == 0);
test.assert(worked_pop == event.applied.pop);
test.assert(worked_tile == tile);

add_pop.rollback(event);
test.assert(calls == ['reset', 'create', 'work', 'unwork', 'destroy']);
test.assert(accumulated_nutrients == 17);
test.assert(worked_pop == null);
test.assert(worked_tile == null);

const existing_pop = make_pop('WORKER', tile);
accumulated_nutrients = 9;
worked_pop = existing_pop;
worked_tile = tile;
calls = [];
event = {
	caller: 0,
	game: game,
	data: {
		base: base,
		pop: existing_pop,
	},
};
event.applied = remove_pop.apply(event);
test.assert(calls == ['reset', 'unwork', 'destroy']);
test.assert(accumulated_nutrients == 0);
test.assert(worked_pop == null);
test.assert(worked_tile == null);

remove_pop.rollback(event);
test.assert(calls == ['reset', 'unwork', 'destroy', 'create', 'work']);
test.assert(accumulated_nutrients == 9);
test.assert(worked_pop == created_pop);
test.assert(worked_tile == tile);
