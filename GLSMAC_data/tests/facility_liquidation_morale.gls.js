const liquidate = #include('../default/game/event/liquidate_base_facility');

let has_commons = true;
const commons = {
	id: 'RecreationCommons',
	energy_maintenance: 1,
	psych_bonus: 4,
};

const make_pop = (initial_type) => {
	let type = initial_type;
	return {
		has: (key) => { return key == 'worked_tile'; },
		get_type: () => { return type; },
		set_type: (value) => { type = value; },
	};
};

const talent = make_pop('TALENT');
const worker_b = make_pop('WORKER');
const worker_c = make_pop('WORKER');
const worker_d = make_pop('WORKER');
const pops = [talent, worker_b, worker_c, worker_d];
const base = {
	get_pops: () => { return pops; },
	has_facility: (id) => { return has_commons && id == commons.id; },
	remove_facility: (id) => {
		test.assert(id == commons.id && has_commons);
		has_commons = false;
	},
	add_facility: (id) => {
		test.assert(id == commons.id && !has_commons);
		has_commons = true;
	},
};

let processed_psych = 0 - 1;
let game = null;
game = {
	get_bm: () => {
		return {get_facility_def: (id) => { return id == commons.id ? commons : null; }};
	},
	get: (key) => {
		if (key == 'f_economy_get_base_psych') {
			return (target_game, target_base) => {
				test.assert(target_game == game && target_base == base);
				return has_commons ? commons.psych_bonus : 0;
			};
		}
		if (key == 'f_base_process_psych') {
			return (target_game, target_base, psych) => {
				test.assert(target_game == game && target_base == base);
				processed_psych = psych;
				let laborers = 0;
				for (pop of pops) {
					pop.set_type(laborers < 3 ? 'WORKER' : 'DRONE');
					laborers++;
				}
				for (pop of pops) {
					if (psych < 2) {
						break;
					}
					if (pop.get_type() == 'DRONE') {
						pop.set_type('WORKER');
						psych -= 2;
					}
				}
				for (pop of pops) {
					if (psych < 2) {
						break;
					}
					if (pop.get_type() == 'WORKER') {
						pop.set_type('TALENT');
						psych -= 2;
					}
				}
			};
		}
		throw Error('Unexpected game callback: ' + key);
	},
};

let event = {
	caller: 0,
	game: game,
	data: {base: base, facility_id: commons.id},
};
test.assert(!#is_defined(liquidate.validate(event)));
event.applied = liquidate.apply(event);
test.assert(!has_commons && processed_psych == 0);
test.assert([talent.get_type(), worker_b.get_type(), worker_c.get_type(), worker_d.get_type()] == ['WORKER', 'WORKER', 'WORKER', 'DRONE']);

liquidate.rollback(event);
test.assert(has_commons);
test.assert([talent.get_type(), worker_b.get_type(), worker_c.get_type(), worker_d.get_type()] == ['TALENT', 'WORKER', 'WORKER', 'WORKER']);
