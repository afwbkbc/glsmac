const define_probes = #include('../default/game/probes');

let callbacks = {};
let values = {};
let bases = [];
let units = [];
let players = {};
const game = {
	on: (name, callback) => {
		if (!#is_defined(callbacks[name])) {
			callbacks[name] = [];
		}
		callbacks[name] :+callback;
	},
	set: (name, value) => { values[name] = value; },
	get: (name) => { return values[name]; },
	get_bm: () => { return {get_bases: () => { return bases; }}; },
	get_um: () => { return {get_units: () => { return units; }}; },
	get_tm: () => { return {get_distance: (first, second) => { return 4; }}; },
	get_player: (id) => { return players['p' + #to_string(id)]; },
};

values.f_social_get_ratings = (player) => { return {probe: player.probe_rating}; };
define_probes(game);
for (callback of callbacks.start) {
	callback({});
}

const make_player = (id, energy, rating, technologies) => {
	return {
		id: id,
		energy_credits: energy,
		probe_rating: rating,
		get_research_state: () => {
			return {technologies: technologies, target: '', progress: 0};
		},
		get_diplomatic_relation: (other) => { return 'neutral'; },
	};
};

const actor = make_player(1, 500, 0, ['InformationNetworks']);
const target_player = make_player(2, 200, 0, ['InformationNetworks', 'PlanetaryNetworks']);
players.p1 = actor;
players.p2 = target_player;

const headquarters = {
	get_owner: () => { return target_player; },
	get_facilities: () => { return [{id: 'Headquarters', is_project: false}]; },
	has_facility: (id) => { return id == 'Headquarters'; },
	get_tile: () => { return {id: 'headquarters'}; },
};
const target_tile = {id: 'target'};
const target_base = {
	get_owner: () => { return target_player; },
	get_facilities: () => { return []; },
	has_facility: (id) => { return false; },
	get_tile: () => { return target_tile; },
	get_size: () => { return 4; },
	get_pops: () => { return [
		{get_type: () => { return 'WORKER'; }},
		{get_type: () => { return 'DRONE'; }},
	]; },
};
bases = [headquarters, target_base];

const probe = {
	morale: 2,
	get_def: () => { return {weapon: 'ProbeTeam'}; },
};
const target_def = {
	mineral_cost: 40,
	can_found_base: false,
	can_terraform: false,
	abilities: [],
};
const target_unit = {
	owner: 2,
	get_tile: () => { return target_base.get_tile(); },
	get_def: () => { return target_def; },
};
units = [target_unit];

test.assert(values.f_probe_is_unit(probe));
test.assert(values.f_probe_get_success_chance(probe, target_player, 'infiltrate') == 85);
test.assert(values.f_probe_get_success_chance(probe, target_player, 'infiltrate', target_base) == 85);
test.assert(values.f_probe_get_subversion_cost(actor, target_unit) == 94);
test.assert(values.f_probe_can_incite_drone_riots(target_base));
test.assert(values.f_probe_get_assassination_research_loss({
	get_research_state: () => { return {target: 'IndustrialBase', progress: 100}; },
}) == 25);
test.assert(values.f_probe_get_plague_population_loss(target_base) == 2);

const defending_probe = {
	id: 7, owner: 2, morale: 2, health: 1.0, transport_id: 0,
	get_tile: () => { return target_base.get_tile(); },
	get_def: () => { return {weapon: 'ProbeTeam'}; },
};
units :+defending_probe;
test.assert(values.f_probe_get_defending_probe(target_player, target_base) == defending_probe);
test.assert(values.f_probe_get_success_chance(
	probe, target_player, 'infiltrate', target_base
) == 65);
units = [target_unit];

target_def.abilities = ['PolymorphicEncryption'];
test.assert(values.f_probe_get_subversion_cost(actor, target_unit) == 187);
target_def.abilities = [];

target_player.probe_rating = 2;
test.assert(values.f_probe_get_success_chance(probe, target_player, 'infiltrate') == 65);
test.assert(values.f_probe_get_subversion_cost(actor, target_unit) == 187);
target_player.probe_rating = 3;
test.assert(values.f_probe_get_subversion_cost(actor, target_unit) == null);
target_player.probe_rating = 0;

test.assert(values.f_probe_get_mind_control_cost(actor, headquarters) == null);
const base_cost = values.f_probe_get_mind_control_cost(actor, target_base);
test.assert(base_cost > 20);
target_base.has_facility = (id) => { return id == 'GenejackFactory'; };
test.assert(
	values.f_probe_get_mind_control_cost(actor, target_base) ==
	#ceil(#to_float(base_cost) * 0.5)
);
target_base.has_facility = (id) => { return id == 'ChildrenSCreche'; };
test.assert(values.f_probe_get_mind_control_cost(actor, target_base) == base_cost * 2);
target_base.has_facility = (id) => { return id == 'PunishmentSphere'; };
test.assert(values.f_probe_get_mind_control_cost(actor, target_base) == base_cost * 2);
target_base.has_facility = (id) => { return id == 'ResearchHospital'; };
test.assert(values.f_probe_get_plague_population_loss(target_base) == 1);
target_base.has_facility = (id) => { return false; };

const unknown = values.f_probe_get_unknown_technologies(actor, target_player);
test.assert(unknown == ['PlanetaryNetworks']);

const hunter_seeker_base = {
	get_owner: () => { return target_player; },
	get_facilities: () => {
		return [{id: 'TheHunterSeekerAlgorithm', is_project: true}];
	},
};
bases :+hunter_seeker_base;
test.assert(values.f_probe_has_project(target_player, 'TheHunterSeekerAlgorithm'));
