const define_bases = #include('../default/game/bases');

const callbacks = {};
const values = {};
let psych_energy = 2;
let police_rating = 0;
let base_units = [];
let supported_units = [];
const owner = {id: 1};
values.f_economy_get_base_psych = (game, base) => { return psych_energy; };
values.f_social_get_ratings = (player) => {
	return {economy: 0, support: 0, talent: 0, police: police_rating, growth: 0};
};

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
	get_um: () => { return {get_units: (include_embarked) => { return supported_units; }}; },
	event: (name, data) => {},
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

const population_base = {
	get_pops: () => { return [1, 2, 3, 4, 5, 6]; },
};
test.assert(values.f_base_get_stable_worker_count(population_base, 0) == 4);
test.assert(values.f_base_get_stable_worker_count(population_base, 2) == 5);
test.assert(values.f_base_get_stable_worker_count(population_base, 10) == 6);

const make_pop = (initial_type, worked) => {
	let type = initial_type;
	return {
		has: (key) => { return key == 'worked_tile' && worked; },
		get_type: () => { return type; },
		set_type: (value) => { type = value; },
	};
};

const laborers = [
	make_pop('WORKER', true),
	make_pop('WORKER', true),
	make_pop('WORKER', true),
	make_pop('WORKER', true),
	make_pop('WORKER', true),
	make_pop('WORKER', true),
];
const doctor = make_pop('DOCTOR', false);
let pops = laborers + [doctor];
let facilities = [];
const base_tile = {
	get_units: () => { return base_units; },
};
const base = {
	id: 1,
	get_owner: () => { return owner; },
	get_tile: () => { return base_tile; },
	get_pops: () => { return pops; },
	get_size: () => { return #sizeof(pops); },
	get_facilities: () => { return facilities; },
	get_intake: () => { return {NUTRIENTS: 14, MINERALS: 10, ENERGY: 10}; },
	get_consumption: () => { return {NUTRIENTS: 14, MINERALS: 2, ENERGY: 0}; },
};

values.f_base_process_psych(game, base, psych_energy);
let state = values.f_base_get_psych(base);
test.assert(state.talents == 0);
test.assert(state.workers == 5);
test.assert(state.drones == 1);
test.assert(state.specialists == 1);
test.assert(state.psych == 2);
test.assert(state.is_rioting == true);
test.assert(values.f_base_get_pending_production(base) == 0);

psych_energy = 10;
values.f_base_process_psych(game, base, psych_energy);
state = values.f_base_get_psych(base);
test.assert(state.talents == 3);
test.assert(state.workers == 3);
test.assert(state.drones == 0);
test.assert(state.is_rioting == false);
test.assert(values.f_base_get_pending_production(base) == 8);

psych_energy = 0;
doctor.set_type('TECHNICIAN');
let previous = [];
for (pop of pops) {
	previous :+pop.get_type();
}
values.f_base_process_psych(game, base, psych_energy);
state = values.f_base_get_psych(base);
test.assert(state.talents == 0);
test.assert(state.workers == 3);
test.assert(state.drones == 3);
test.assert(state.is_rioting == true);
test.assert(previous == ['TALENT', 'TALENT', 'TALENT', 'WORKER', 'WORKER', 'WORKER', 'TECHNICIAN']);

values.f_project_get_effects = (target_base) => {
	return {
		talent_bonus: 0,
		drone_modifier: -2,
		network_node_drone_modifier: 0,
		prevent_riots: false,
		small_base_drone_modifier: 0,
	};
};
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.workers == 5 && state.drones == 1);
values.f_project_get_effects = #undefined;

values.f_project_get_effects = (target_base) => {
	test.assert(target_base == base);
	return {
		talent_bonus: 0,
		network_node_drone_modifier: 0,
		prevent_riots: true,
		small_base_drone_modifier: 0,
	};
};
state = values.f_base_get_psych(base);
test.assert(state.is_rioting == false);
test.assert(values.f_base_get_pending_production(base) == 8);
values.f_project_get_effects = #undefined;

pops = [
	make_pop('WORKER', true),
	make_pop('WORKER', true),
	make_pop('WORKER', true),
];
facilities = [{drone_modifier: 1, talent_bonus: 0, suppress_psych: false}];
values.f_project_get_effects = (target_base) => {
	return {
		talent_bonus: 0,
		network_node_drone_modifier: 0,
		prevent_riots: false,
		small_base_drone_modifier: -1,
	};
};
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.workers == 3 && state.drones == 0);
values.f_project_get_effects = #undefined;
pops = laborers + [doctor];

facilities = [{drone_modifier: -2, talent_bonus: 0, suppress_psych: false}];
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.workers == 5 && state.drones == 1 && state.talents == 0);

facilities = [{drone_modifier: 1, talent_bonus: 0, suppress_psych: false}];
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.workers == 2 && state.drones == 4 && state.talents == 0);

facilities = [{drone_modifier: -2, talent_bonus: 2, suppress_psych: false}];
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.workers == 5 && state.drones == 0 && state.talents == 1);

facilities = [{drone_modifier: 0, talent_bonus: 0, suppress_psych: true}];
values.f_base_process_psych(game, base, 10);
state = values.f_base_get_psych(base);
test.assert(state.workers == 6 && state.drones == 0 && state.talents == 0);
test.assert(values.f_base_get_stable_worker_count(base, 0) == 7);

const make_unit = (owner_id, health, offense, ability_ids) => {
	const def = {offense: offense, abilities: ability_ids};
	return {
		owner: owner_id,
		health: health,
		get_def: () => { return def; },
	};
};
const normal_police = make_unit(owner.id, 1.0, 1, []);
const nonlethal_police = make_unit(owner.id, 1.0, 1, ['NonLethalMethods']);
const dead_police = make_unit(owner.id, 0.0, 1, ['NonLethalMethods']);
const rival_police = make_unit(2, 1.0, 1, ['NonLethalMethods']);
const noncombat_unit = make_unit(owner.id, 1.0, 0, ['NonLethalMethods']);
const reset_laborers = () => {
	pops = [
		make_pop('WORKER', true),
		make_pop('WORKER', true),
		make_pop('WORKER', true),
		make_pop('WORKER', true),
		make_pop('WORKER', true),
		make_pop('WORKER', true),
	];
	facilities = [];
	psych_energy = 0;
};

reset_laborers();
police_rating = 0 - 2;
base_units = [normal_police];
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.drones == 3);
test.assert(state.police == {
	rating: 0 - 2, unit_limit: 0, unit_multiplier: 1,
	present_units: 1, used_units: 0, extra_units: 0, suppression: 0,
	away_units: 0, pacifism_drones: 0,
});

police_rating = 0 - 1;
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.drones == 2 && state.workers == 4);
test.assert(state.police.suppression == 1 && state.police.used_units == 1);

police_rating = 0;
base_units = [normal_police, nonlethal_police, normal_police];
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.drones == 1 && state.police.suppression == 2);
test.assert(state.police.present_units == 3 && state.police.used_units == 1);

police_rating = 1;
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.drones == 0 && state.talents == 0);
test.assert(state.police.unit_limit == 2 && state.police.suppression == 3);

police_rating = 3;
base_units = [nonlethal_police];
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.drones == 0 && state.talents == 0);
test.assert(state.police.unit_multiplier == 2 && state.police.suppression == 4);

police_rating = 2;
base_units = [dead_police, rival_police, noncombat_unit];
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.drones == 3);
test.assert(state.police.present_units == 0 && state.police.suppression == 0);

police_rating = 0 - 2;
base_units = [normal_police];
values.f_project_get_effects = (target_base) => {
	return {
		talent_bonus: 0,
		network_node_drone_modifier: 0,
		prevent_riots: false,
		small_base_drone_modifier: 0,
		police_rating_bonus: 1,
		extra_police_units: 1,
	};
};
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.drones == 1);
test.assert(state.police.rating == 0 - 1 && state.police.extra_units == 1);
test.assert(state.police.suppression == 2);

values.f_project_get_effects = #undefined;
police_rating = 0 - 1;
test.assert(values.f_base_get_stable_worker_count(base, 0) == 5);

const home_tile = {get_base: () => { return null; }};
const foreign_tile = {get_base: () => { return null; }};
const home_base_tile = {get_base: () => { return base; }};
values.f_territory_get_owner = (tile) => {
	return tile == home_tile || tile == home_base_tile ? owner : {id: 2};
};
const make_supported_unit = (tile, is_air, abilities, offense) => {
	const unit = make_unit(owner.id, 1.0, offense, abilities);
	unit.home_base_id = base.id;
	unit.is_air = is_air;
	unit.get_tile = () => { return tile; };
	return unit;
};
const away_unit = make_supported_unit(foreign_tile, false, [], 1);
const home_unit = make_supported_unit(home_tile, false, [], 1);
const ground_attack_air = make_supported_unit(home_base_tile, true, [], 1);
const interceptor = make_supported_unit(
	home_base_tile,
	true,
	['AirSuperiority'],
	1
);
const away_civilian = make_supported_unit(foreign_tile, false, [], 0);
supported_units = [away_unit, home_unit, ground_attack_air, interceptor, away_civilian];
base_units = [];
reset_laborers();

police_rating = 0 - 3;
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.police.away_units == 2 && state.police.pacifism_drones == 1);
test.assert(state.drones == 4 && state.workers == 2);

police_rating = 0 - 4;
values.f_base_process_psych(game, base, 0);
state = values.f_base_get_psych(base);
test.assert(state.police.pacifism_drones == 2);
test.assert(state.drones == 5 && state.workers == 1);

police_rating = 0 - 5;
values.f_base_process_psych(game, base, 10);
state = values.f_base_get_psych(base);
test.assert(state.police.pacifism_drones == 4);
test.assert(state.talents == 2 && state.drones == 4 && state.workers == 0);
