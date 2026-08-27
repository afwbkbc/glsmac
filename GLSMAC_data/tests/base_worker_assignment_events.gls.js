const work_base_tile = #include('../default/game/event/work_base_tile');
const unwork_base_tile = #include('../default/game/event/unwork_base_tile');

const owner = {id: 1};

const make_tile = (id) => {
	let working_pop = #undefined;
	return {
		id: id,
		get_base: () => { return null; },
		has: (key) => { return key == 'working_pop' && #is_defined(working_pop); },
		get: (key) => { return key == 'working_pop' ? working_pop : #undefined; },
		set_working_pop: (pop) => { working_pop = pop; },
	};
};

let base = null;
const make_pop = (initial_type, initial_tile) => {
	let type = initial_type;
	let tile = initial_tile;
	const pop = {
		get_base: () => { return base; },
		get_type: () => { return type; },
		set_type: (value) => { type = value; },
		has: (key) => { return key == 'worked_tile' && #is_defined(tile); },
		get: (key) => { return key == 'worked_tile' ? tile : #undefined; },
		set_tile: (value) => { tile = value; },
	};
	if (#is_defined(tile)) {
		tile.set_working_pop(pop);
	}
	return pop;
};

const tile_a = make_tile('a');
const tile_b = make_tile('b');
const tile_c = make_tile('c');
const tile_d = make_tile('d');
const tile_e = make_tile('e');
const worker_a = make_pop('WORKER', tile_a);
const worker_b = make_pop('WORKER', tile_b);
const worker_c = make_pop('WORKER', tile_c);
const drone_d = make_pop('DRONE', tile_d);
const pops = [worker_a, worker_b, worker_c, drone_d];

base = {
	get_owner: () => { return owner; },
	get_pops: () => { return pops; },
	get_workable_tiles: () => { return [tile_a, tile_b, tile_c, tile_d, tile_e]; },
	is_tile_worked: (tile) => { return tile.has('working_pop'); },
	work_pop_tile: (pop, tile) => {
		pop.set_tile(tile);
		tile.set_working_pop(pop);
	},
	unwork_pop_tile: (pop, tile) => {
		tile.set_working_pop(#undefined);
		pop.set_tile(#undefined);
	},
};

let psych_calls = 0;
const process_psych = (game, target_base, psych) => {
	test.assert(target_base == base && psych == 0);
	psych_calls++;
	let laborers = 0;
	for (pop of pops) {
		if (pop.has('worked_tile')) {
			pop.set_type(laborers < 3 ? 'WORKER' : 'DRONE');
			laborers++;
		}
	}
	let improvements = 0;
	for (pop of pops) {
		if (!pop.has('worked_tile') && pop.get_type() == 'DOCTOR') {
			improvements++;
		}
	}
	for (pop of pops) {
		if (improvements <= 0) {
			break;
		}
		if (pop.has('worked_tile') && pop.get_type() == 'DRONE') {
			pop.set_type('WORKER');
			improvements--;
		}
	}
	for (pop of pops) {
		if (improvements <= 0) {
			break;
		}
		if (pop.has('worked_tile') && pop.get_type() == 'WORKER') {
			pop.set_type('TALENT');
			improvements--;
		}
	}
};

const helpers = {
	f_base_pop_work_tile: (target_base, pop, tile) => {
		const old_tile = pop.get('worked_tile');
		if (#is_defined(old_tile)) {
			target_base.unwork_pop_tile(pop, old_tile);
		}
		pop.set_type('WORKER');
		target_base.work_pop_tile(pop, tile);
	},
	f_base_pop_unwork_tile: (target_base, pop, type) => {
		const tile = pop.get('worked_tile');
		if (#is_defined(tile)) {
			target_base.unwork_pop_tile(pop, tile);
		}
		if (#is_defined(type)) {
			pop.set_type(type);
		}
	},
	f_economy_get_base_psych: (game, target_base) => { return 0; },
	f_base_process_psych: process_psych,
};

const game = {
	is_turn_complete: (player_id) => { return false; },
	get: (key) => { return helpers[key]; },
};

let event = {
	caller: owner.id,
	game: game,
	data: {base: base, pop: drone_d, tile: tile_e},
};
test.assert(!#is_defined(work_base_tile.validate(event)));
event.applied = work_base_tile.apply(event);
test.assert(psych_calls == 1);
test.assert(!tile_d.has('working_pop') && tile_e.get('working_pop') == drone_d);
test.assert(drone_d.get_type() == 'DRONE');
work_base_tile.rollback(event);
test.assert(!tile_e.has('working_pop') && tile_d.get('working_pop') == drone_d);
test.assert([worker_a.get_type(), worker_b.get_type(), worker_c.get_type(), drone_d.get_type()] == ['WORKER', 'WORKER', 'WORKER', 'DRONE']);

event = {
	caller: owner.id,
	game: game,
	data: {base: base, tile: tile_a},
};
test.assert(!#is_defined(unwork_base_tile.validate(event)));
event.applied = unwork_base_tile.apply(event);
test.assert(psych_calls == 2);
test.assert(!tile_a.has('working_pop') && worker_a.get_type() == 'DOCTOR');
test.assert(worker_b.get_type() == 'TALENT');
test.assert(worker_c.get_type() == 'WORKER');
test.assert(drone_d.get_type() == 'WORKER');
unwork_base_tile.rollback(event);
test.assert(tile_a.get('working_pop') == worker_a);
test.assert([worker_a.get_type(), worker_b.get_type(), worker_c.get_type(), drone_d.get_type()] == ['WORKER', 'WORKER', 'WORKER', 'DRONE']);
