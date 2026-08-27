const define_probes = #include('../default/game/probes');
const define_diplomacy = #include('../default/game/diplomacy');
const probe_operation = #include('../default/game/event/probe_operation');

const make_player = (id, energy, technologies, target) => {
	let infiltrated = {};
	let relations = {};
	let offers = {};
	let trades = {};
	let loan_offers = {};
	let loans = {};
	let major_atrocities = 0;
	let sanction_turns = 0;
	let integrity_blemishes = 0;
	let research = {technologies: technologies, target: target, progress: target == '' ? 0 : 9};
	let player = {
		id: id,
		name: 'Player ' + #to_string(id),
		energy_credits: energy,
		probe_rating: 0,
	};
	player.get_energy_credits = () => { return player.energy_credits; };
	player.set_energy_credits = (value) => { player.energy_credits = value; };
	player.read_energy_credits = () => { return player.energy_credits; };
	player.get_research_state = () => { return research; };
	player.set_research_state = (value) => { research = value; };
	player.has_technology = (id) => {
		for (known of research.technologies) {
			if (known == id) { return true; }
		}
		return false;
	};
	player.get_major_atrocities = () => { return major_atrocities; };
	player.set_major_atrocities = (value) => { major_atrocities = value; };
	player.get_sanction_turns = () => { return sanction_turns; };
	player.set_sanction_turns = (value) => { sanction_turns = value; };
	player.get_integrity_blemishes = () => { return integrity_blemishes; };
	player.set_integrity_blemishes = (value) => { integrity_blemishes = value; };
	player.has_infiltrated = (other) => {
			return #is_defined(infiltrated['p' + #to_string(other.id)]) &&
				infiltrated['p' + #to_string(other.id)];
		};
	player.set_infiltrated = (other, value) => {
			infiltrated['p' + #to_string(other.id)] = value;
		};
	player.get_diplomatic_relation = (other) => {
			const key = 'p' + #to_string(other.id);
			return #is_defined(relations[key]) ? relations[key] : 'neutral';
		};
	player.set_diplomatic_relation = (other, value) => {
			relations['p' + #to_string(other.id)] = value;
		};
	player.get_diplomatic_offer = (other) => {
			const key = 'p' + #to_string(other.id);
			return #is_defined(offers[key]) ? offers[key] : '';
		};
	player.set_diplomatic_offer = (other, value) => {
		offers['p' + #to_string(other.id)] = value;
		};
	player.get_diplomatic_trade = (other) => {
		const key = 'p' + #to_string(other.id);
		return #is_defined(trades[key]) ? trades[key] : null;
		};
	player.set_diplomatic_trade = (other, value) => {
		trades['p' + #to_string(other.id)] = value;
		};
	player.clear_diplomatic_trade = (other) => {
		trades['p' + #to_string(other.id)] = #undefined;
		};
	player.get_diplomatic_loan_offer = (other) => {
		const key = 'p' + #to_string(other.id);
		return #is_defined(loan_offers[key]) ? loan_offers[key] : null;
		};
	player.set_diplomatic_loan_offer = (other, value) => {
		loan_offers['p' + #to_string(other.id)] = value;
		};
	player.clear_diplomatic_loan_offer = (other) => {
		loan_offers['p' + #to_string(other.id)] = #undefined;
		};
	player.get_diplomatic_loan = (other) => {
		const key = 'p' + #to_string(other.id);
		return #is_defined(loans[key]) ? loans[key] : null;
		};
	player.set_diplomatic_loan = (other, value) => {
		loans['p' + #to_string(other.id)] = value;
		};
	player.clear_diplomatic_loan = (other) => {
		loans['p' + #to_string(other.id)] = #undefined;
		};
	return player;
};

const make_fixture = (charter_repealed) => {
	const actor = make_player(1, 1000, [], 'PlanetaryNetworks');
	const target_player = make_player(2, 200, ['PlanetaryNetworks'], '');
	let players = {p1: actor, p2: target_player};
	let triggers = [];
	let last_message = '';
	let callbacks = {};
	let values = {};
	let datalinks_queues = 0;

	const probe_tile = {x: 1, y: 1};
	const target_tile = {x: 2, y: 1};
	const nearby_tile = {x: 3, y: 2};
	const headquarters_tile = {x: 4, y: 1};
	probe_tile.is_adjactent_to = (other) => { return other == target_tile; };
	target_tile.is_adjactent_to = (other) => {
		return other == probe_tile || other == nearby_tile;
	};
	nearby_tile.is_adjactent_to = (other) => { return other == target_tile; };
	headquarters_tile.is_adjactent_to = (other) => { return false; };

	const defs = {
		ProbeTeam: {
			id: 'ProbeTeam', weapon: 'ProbeTeam', mineral_cost: 40,
			can_found_base: false, can_terraform: false, abilities: [], morale_set: 'STANDARD',
		},
		Defender: {
			id: 'Defender', weapon: 'HandWeapons', mineral_cost: 40,
			can_found_base: false, can_terraform: false, abilities: [], morale_set: 'STANDARD',
		},
	};
	let units = [];
	const make_unit = (data) => {
		let unit = {
			id: data.id,
			def: data.def,
			owner: data.owner.id,
			movement: #is_defined(data.movement) ? data.movement : 1.0,
			morale: data.morale,
			health: data.health,
			moved_this_turn: #is_defined(data.moved_this_turn) ? data.moved_this_turn : false,
			terraforming: #is_defined(data.terraforming) ? data.terraforming : 'none',
			terraforming_turns_remaining: #is_defined(data.terraforming_turns_remaining)
				? data.terraforming_turns_remaining : 0,
			home_base_id: #is_defined(data.home_base_id) ? data.home_base_id : 0,
			fuel: #is_defined(data.fuel) ? data.fuel : 0,
			transport_id: #is_defined(data.transport_id) ? data.transport_id : 0,
		};
		unit.get_def = () => { return defs[unit.def]; };
		unit.get_tile = () => { return data.tile; };
		unit.get_cargo = () => { return []; };
		unit.set_home_base_id = (value) => { unit.home_base_id = value; };
		return unit;
	};
	const um = {
		get_units: () => { return units; },
		has_unit: (id) => {
			for (unit of units) { if (unit.id == id) { return true; } }
			return false;
		},
		get_unit: (id) => {
			for (unit of units) { if (unit.id == id) { return unit; } }
			return null;
		},
		spawn_unit: (data) => {
			const unit = make_unit(data);
			units :+unit;
			return unit;
		},
		despawn_unit: (doomed) => {
			let remaining = [];
			for (unit of units) { if (unit.id != doomed.id) { remaining :+unit; } }
			units = remaining;
		},
		get_moraleset: (id) => { return [0, 1, 2, 3, 4, 5, 6]; },
	};

	const make_base = (id, tile, owner, facilities) => {
		let current_owner = owner;
		let current_facilities = facilities;
		let minerals = 30;
		let nutrients = 18;
		let queue = [{production_kind: 'unit', id: 'Defender'}];
		const make_pop = (type, worked_tile) => {
			let pop = {type: type, worked_tile: worked_tile};
			pop.get_type = () => { return pop.type; };
			pop.set_type = (value) => { pop.type = value; };
			pop.get = (key) => { return key == 'worked_tile' ? pop.worked_tile : #undefined; };
			pop.set_worked_tile = (value) => { pop.worked_tile = value; };
			return pop;
		};
		let pops = [
			make_pop('WORKER', tile), make_pop('WORKER', #undefined),
			make_pop('WORKER', #undefined), make_pop('WORKER', #undefined),
		];
		return {
			id: id,
			name: 'Base ' + #to_string(id),
			get_owner: () => { return current_owner; },
			set_owner: (value) => { current_owner = value; },
			get_tile: () => { return tile; },
			get_size: () => { return #sizeof(pops); },
			get_pops: () => { return pops; },
			create_pop: (data) => {
				const pop = make_pop(data.type, null);
				pops :+pop;
				return pop;
			},
			destroy_pop: (doomed) => {
				let remaining = [];
				for (pop of pops) { if (pop != doomed) { remaining :+pop; } }
				pops = remaining;
			},
			get: (key) => { return key == 'accumulated_nutrients' ? nutrients : null; },
			set: (key, value) => { if (key == 'accumulated_nutrients') { nutrients = value; } },
			get_facilities: () => { return current_facilities; },
			has_facility: (facility_id) => {
				for (facility of current_facilities) {
					if (facility.id == facility_id) { return true; }
				}
				return false;
			},
			remove_facility: (facility_id) => {
				let remaining = [];
				for (facility of current_facilities) {
					if (facility.id != facility_id) { remaining :+facility; }
				}
				current_facilities = remaining;
			},
			add_facility: (facility_id) => {
				current_facilities :+{id: facility_id, is_project: false};
			},
			get_accumulated_minerals: () => { return minerals; },
			set_accumulated_minerals: (value) => { minerals = value; },
			get_production_queue: () => { return queue; },
			set_production_queue: (value) => { queue = value; },
			can_produce: (kind, production_id) => { return true; },
		};
	};
	const target_base = make_base(
		10, target_tile, target_player, [{id: 'RecyclingTanks', is_project: false}]
	);
	const headquarters = make_base(
		11, headquarters_tile, target_player, [{id: 'Headquarters', is_project: false}]
	);
	let bases = [target_base, headquarters];
	target_tile.get_base = () => { return target_base; };
	probe_tile.get_base = () => { return null; };
	nearby_tile.get_base = () => { return null; };
	headquarters_tile.get_base = () => { return headquarters; };

	const tm = {
		get_tile: (x, y) => {
			if (x == probe_tile.x && y == probe_tile.y) { return probe_tile; }
			if (x == target_tile.x && y == target_tile.y) { return target_tile; }
			if (x == nearby_tile.x && y == nearby_tile.y) { return nearby_tile; }
			return headquarters_tile;
		},
		get_distance: (first, second) => { return 3; },
	};
	const bm = {get_bases: () => { return bases; }};
	const game = {
		um: um,
		tm: tm,
		bm: bm,
		random: {get_int: (minimum, maximum) => { return minimum; }},
		on: (name, callback) => {
			if (!#is_defined(callbacks[name])) { callbacks[name] = []; }
			callbacks[name] :+callback;
		},
		set: (name, value) => { values[name] = value; },
		get: (name) => { return values[name]; },
		get_um: () => { return um; },
		get_tm: () => { return tm; },
		get_bm: () => { return bm; },
		get_player: (id) => { return players['p' + #to_string(id)]; },
		is_turn_complete: (id) => { return false; },
		trigger: (name, data) => { triggers :+{name: name, data: data}; },
		message: (text) => { last_message = text; },
	};
	values.f_social_get_ratings = (player) => { return {probe: player.probe_rating}; };
	values.f_technology_get_next_target = (known, player) => { return ''; };
	values.f_project_queue_planetary_datalinks = () => { datalinks_queues++; };
	values.f_economy_get_base_psych = (game_value, base) => { return 0; };
	values.f_base_process_psych = (game_value, base, psych) => {};
	values.f_base_reset_nutrients = (game_value, base) => {
		base.set('accumulated_nutrients', 0);
	};
	values.f_base_pop_unwork_tile = (base, pop) => { pop.set_worked_tile(#undefined); };
	values.f_base_pop_work_tile = (base, pop, tile) => { pop.set_worked_tile(tile); };
	values.f_council_is_un_charter_repealed = () => { return charter_repealed == true; };
	define_probes(game);
	define_diplomacy(game);
	for (callback of callbacks.start) { callback({}); }

	const probe = um.spawn_unit({
		id: 1, def: 'ProbeTeam', owner: actor, tile: probe_tile,
		morale: 2, health: 1.0, movement: 1.0,
	});
	const defender = um.spawn_unit({
		id: 2, def: 'Defender', owner: target_player, tile: target_tile,
		morale: 2, health: 1.0, movement: 1.0, home_base_id: target_base.id,
	});
	return {
		game: game, actor: actor, target_player: target_player, target_base: target_base,
		probe: probe, defender: defender, nearby_tile: nearby_tile, um: um, triggers: triggers,
		read_message: () => { return last_message; },
		read_datalinks_queues: () => { return datalinks_queues; },
	};
};

const count_pop_type = (base, type) => {
	let count = 0;
	for (pop of base.get_pops()) {
		if (pop.get_type() == type) { count += 1; }
	}
	return count;
};

const result = (success, detected, survives) => {
	return {
		success: success, detected: detected, survives: survives, chance: 85, cost: 0,
		technology_id: '', sabotage_facility_id: '', drain_amount: 0,
		research_loss: 0, population_loss: 0, defender_id: 0,
	};
};

let f = make_fixture();
let e = {caller: 1, game: f.game, data: {unit: f.probe, operation: 'infiltrate', target: f.target_base}};
test.assert(!#is_defined(probe_operation.validate(e)));
f.actor.set_diplomatic_relation(f.target_player, 'treaty');
f.target_player.set_diplomatic_relation(f.actor, 'treaty');
e.resolved = result(true, true, true);
e.applied = probe_operation.apply(e);
test.assert(f.actor.has_infiltrated(f.target_player));
test.assert(f.actor.get_diplomatic_relation(f.target_player) == 'vendetta');
test.assert(f.actor.get_integrity_blemishes() == 1);
test.assert(e.data.unit.morale == 3 && e.data.unit.movement == 0.0);
test.assert(f.read_message() == 'Datalinks infiltrated. The operation was detected.');
probe_operation.rollback(e);
test.assert(!f.actor.has_infiltrated(f.target_player));
test.assert(f.actor.get_diplomatic_relation(f.target_player) == 'treaty');
test.assert(f.actor.get_integrity_blemishes() == 0);
test.assert(f.um.get_unit(1).morale == 2 && f.um.get_unit(1).movement == 1.0);

f = make_fixture();
e = {caller: 1, game: f.game, data: {unit: f.probe, operation: 'steal_technology', target: f.target_base}};
e.resolved = result(true, false, true);
e.resolved.technology_id = 'PlanetaryNetworks';
e.applied = probe_operation.apply(e);
test.assert(f.actor.get_research_state().technologies == ['PlanetaryNetworks']);
test.assert(f.actor.get_research_state().target == '');
test.assert(f.read_datalinks_queues() == 1);
probe_operation.rollback(e);
test.assert(f.actor.get_research_state().technologies == []);
test.assert(f.actor.get_research_state().target == 'PlanetaryNetworks');

f = make_fixture();
e = {caller: 1, game: f.game, data: {unit: f.probe, operation: 'sabotage', target: f.target_base}};
e.resolved = result(true, false, true);
e.applied = probe_operation.apply(e);
test.assert(f.target_base.get_accumulated_minerals() == 0);
probe_operation.rollback(e);
test.assert(f.target_base.get_accumulated_minerals() == 30);

f = make_fixture();
e = {caller: 1, game: f.game, data: {unit: f.probe, operation: 'sabotage', target: f.target_base}};
e.resolved = result(true, false, true);
e.resolved.sabotage_facility_id = 'RecyclingTanks';
e.applied = probe_operation.apply(e);
test.assert(!f.target_base.has_facility('RecyclingTanks'));
probe_operation.rollback(e);
test.assert(f.target_base.has_facility('RecyclingTanks'));

f = make_fixture();
e = {caller: 1, game: f.game, data: {unit: f.probe, operation: 'drain_energy', target: f.target_base}};
e.resolved = result(true, false, true);
e.resolved.drain_amount = 50;
e.applied = probe_operation.apply(e);
test.assert(
	f.actor.read_energy_credits() == 1050 &&
	f.target_player.read_energy_credits() == 150
);
probe_operation.rollback(e);
test.assert(
	f.actor.read_energy_credits() == 1000 &&
	f.target_player.read_energy_credits() == 200
);

f = make_fixture();
e = {caller: 1, game: f.game, data: {
	unit: f.probe, operation: 'incite_drone_riots', target: f.target_base,
}};
test.assert(!#is_defined(probe_operation.validate(e)));
e.resolved = result(true, false, true);
e.applied = probe_operation.apply(e);
test.assert(count_pop_type(f.target_base, 'DRONE') == 1);
probe_operation.rollback(e);
test.assert(count_pop_type(f.target_base, 'DRONE') == 0);

f = make_fixture();
f.target_player.set_research_state({
	technologies: ['PlanetaryNetworks'], target: 'IndustrialBase', progress: 100,
});
e = {caller: 1, game: f.game, data: {
	unit: f.probe, operation: 'assassinate_researchers', target: f.target_base,
}};
test.assert(#is_defined(probe_operation.validate(e)));
f.probe.morale = 3;
test.assert(!#is_defined(probe_operation.validate(e)));
e.resolved = result(true, false, true);
e.resolved.research_loss = 25;
e.applied = probe_operation.apply(e);
test.assert(f.target_player.get_research_state().progress == 75);
probe_operation.rollback(e);
test.assert(f.target_player.get_research_state().progress == 100);

f = make_fixture();
e = {caller: 1, game: f.game, data: {
	unit: f.probe, operation: 'genetic_plague', target: f.target_base,
}};
test.assert(#is_defined(probe_operation.validate(e)));
f.actor.set_research_state({
	technologies: ['RetroviralEngineering'], target: 'PlanetaryNetworks', progress: 9,
});
test.assert(!#is_defined(probe_operation.validate(e)));
e.resolved = result(true, true, true);
e.resolved.population_loss = 2;
e.applied = probe_operation.apply(e);
test.assert(f.target_base.get_size() == 2);
test.assert(f.actor.get_major_atrocities() == 1);
test.assert(f.actor.get_sanction_turns() == 10);
test.assert(f.target_base.get('accumulated_nutrients') == 0);
probe_operation.rollback(e);
test.assert(f.target_base.get_size() == 4);
test.assert(f.actor.get_major_atrocities() == 0);
test.assert(f.actor.get_sanction_turns() == 0);
test.assert(f.target_base.get('accumulated_nutrients') == 18);

f = make_fixture(true);
f.actor.set_research_state({
	technologies: ['RetroviralEngineering'], target: 'PlanetaryNetworks', progress: 9,
});
e = {caller: 1, game: f.game, data: {
	unit: f.probe, operation: 'genetic_plague', target: f.target_base,
}};
e.resolved = result(true, true, true);
e.resolved.population_loss = 2;
e.applied = probe_operation.apply(e);
test.assert(f.target_base.get_size() == 2);
test.assert(f.actor.get_major_atrocities() == 1);
test.assert(f.actor.get_sanction_turns() == 0);
test.assert(f.actor.get_diplomatic_relation(f.target_player) == 'vendetta');
probe_operation.rollback(e);
test.assert(f.target_base.get_size() == 4);
test.assert(f.actor.get_major_atrocities() == 0);
test.assert(f.actor.get_sanction_turns() == 0);

f = make_fixture();
e = {caller: 1, game: f.game, data: {unit: f.probe, operation: 'subvert_unit', target: f.defender}};
test.assert(!#is_defined(probe_operation.validate(e)));
e.resolved = result(true, true, true);
e.resolved.cost = 100;
e.applied = probe_operation.apply(e);
test.assert(f.um.get_unit(2).owner == 1 && f.actor.read_energy_credits() == 900);
probe_operation.rollback(e);
test.assert(f.um.get_unit(2).owner == 2 && f.actor.read_energy_credits() == 1000);

f = make_fixture();
e = {caller: 1, game: f.game, data: {unit: f.probe, operation: 'mind_control_base', target: f.target_base}};
const remote_support = f.um.spawn_unit({
	id: 3, def: 'Defender', owner: f.target_player, tile: f.game.tm.get_tile(4, 1),
	morale: 2, health: 1.0, movement: 1.0, home_base_id: f.target_base.id,
});
const nearby_defender = f.um.spawn_unit({
	id: 4, def: 'Defender', owner: f.target_player, tile: f.nearby_tile,
	morale: 2, health: 1.0, movement: 1.0, home_base_id: 11,
});
test.assert(!#is_defined(probe_operation.validate(e)));
e.resolved = result(true, true, true);
e.resolved.cost = 300;
e.applied = probe_operation.apply(e);
test.assert(f.target_base.get_owner().id == 1);
test.assert(f.um.get_unit(2).owner == 1);
test.assert(f.um.get_unit(4).owner == 1 && f.um.get_unit(4).home_base_id == 0);
test.assert(remote_support.owner == 2 && remote_support.home_base_id == 11);
probe_operation.rollback(e);
test.assert(f.target_base.get_owner().id == 2);
test.assert(f.um.get_unit(2).owner == 2 && f.um.get_unit(2).home_base_id == 10);
test.assert(f.um.get_unit(4).owner == 2 && f.um.get_unit(4).home_base_id == 11);
test.assert(remote_support.owner == 2 && remote_support.home_base_id == 10);

f = make_fixture();
const defending_probe = f.um.spawn_unit({
	id: 5, def: 'ProbeTeam', owner: f.target_player, tile: f.target_base.get_tile(),
	morale: 2, health: 1.0, movement: 1.0, home_base_id: f.target_base.id,
});
e = {caller: 1, game: f.game, data: {
	unit: f.probe, operation: 'infiltrate', target: f.target_base,
}};
test.assert(!#is_defined(probe_operation.validate(e)));
e.resolved = probe_operation.resolve(e);
test.assert(e.resolved.success && e.resolved.detected && e.resolved.defender_id == 5);
e.applied = probe_operation.apply(e);
test.assert(!f.um.has_unit(5));
probe_operation.rollback(e);
test.assert(f.um.has_unit(5) && f.um.get_unit(5).owner == 2);

f = make_fixture();
f.um.spawn_unit({
	id: 5, def: 'ProbeTeam', owner: f.target_player, tile: f.target_base.get_tile(),
	morale: 3, health: 1.0, movement: 1.0, home_base_id: f.target_base.id,
});
e = {caller: 1, game: f.game, data: {
	unit: f.probe, operation: 'infiltrate', target: f.target_base,
}};
e.resolved = result(false, true, false);
e.resolved.defender_id = 5;
e.applied = probe_operation.apply(e);
test.assert(!f.um.has_unit(1) && f.um.has_unit(5));
probe_operation.rollback(e);
test.assert(f.um.has_unit(1) && f.um.has_unit(5));

f = make_fixture();
e = {caller: 1, game: f.game, data: {unit: f.probe, operation: 'drain_energy', target: f.target_base}};
e.resolved = result(false, true, false);
e.applied = probe_operation.apply(e);
test.assert(!f.um.has_unit(1));
probe_operation.rollback(e);
test.assert(f.um.has_unit(1));
