const capture = #include('../default/game/native_capture');

const human = {
	id: 1,
	type: 'human',
	name: 'Deirdre',
	get_faction: () => { return {id: 'GAIANS'}; },
};
const planet = {
	id: 7,
	type: 'native',
	name: 'Planet',
	get_faction: () => { return {id: 'PLANET'}; },
};
const rival = {
	id: 2,
	type: 'ai',
	name: 'Rival',
	get_faction: () => { return {id: 'HIVE'}; },
};

let units = [];
let tile_units = [];
let bases = [];
let planet_rating = 1;
let inherent_planet = 1;
let ecological_damage = {value: 0};
let random_values = [];
let random_calls = 0;

const target_tile = {
	x: 4,
	y: 4,
	get_units: (include_embarked) => { return tile_units; },
};
const attacker_tile = {x: 3, y: 3};

const make_unit = (id, owner, definition_id, native, tile) => {
	const definition = {
		id: definition_id,
		name: definition_id,
		is_native: native,
		is_artillery: false,
	};
	return {
		id: id,
		def: definition_id,
		owner: owner.id,
		movement: 1.0,
		morale: 1,
		health: 1.0,
		moved_this_turn: false,
		terraforming: 'none',
		terraforming_turns_remaining: 0,
		home_base_id: 0,
		fuel: 0,
		transport_id: 0,
		native_capture_attempted: false,
		is_air: false,
		get_owner: () => { return owner; },
		get_def: () => { return definition; },
		get_tile: () => { return tile; },
	};
};

const attacker = make_unit(1, human, 'ScoutPatrol', false, attacker_tile);
const target = make_unit(10, planet, 'MindWorms', true, target_tile);
const stackmate = make_unit(11, planet, 'LocustsOfChiron', true, target_tile);
const foreign = make_unit(12, rival, 'ScoutPatrol', false, target_tile);
units = [attacker, target, stackmate, foreign];
tile_units = [target, stackmate, foreign];

const game = {
	get: (name) => {
		if (name == 'f_social_get_ratings') {
			return (player) => { return {planet: planet_rating}; };
		}
		if (name == 'f_social_get_faction_modifier') {
			return (player, rating) => { return inherent_planet; };
		}
		if (name == 'f_ecology_get_base_damage') {
			return (base) => { return ecological_damage; };
		}
		return #undefined;
	},
	get_turn: () => { return 50; },
	um: {
		get_units: (include_embarked) => { return units; },
	},
	bm: {
		get_bases: () => { return bases; },
	},
	tm: {
		get_distance: (from, to) => { return 1; },
	},
	random: {
		get_int: (minimum, maximum) => {
			const result = random_values[random_calls];
			random_calls++;
			return result;
		},
	},
};

let result = capture.resolve(game, attacker, target);
test.assert(result.attempted && result.captured);
test.assert(result.reason == 'first_native');
test.assert(result.unit_ids == [10, 11]);
test.assert(random_calls == 0);

inherent_planet = 0;
random_values = [3];
random_calls = 0;
result = capture.resolve(game, attacker, target);
test.assert(result.attempted && !result.captured && result.mark_attempted);
test.assert(result.reason == 'roll_failed');
test.assert(random_calls == 1);

attacker.movement = 0.5;
random_calls = 0;
result = capture.resolve(game, attacker, target);
test.assert(!result.attempted && !result.captured);
test.assert(random_calls == 0);

inherent_planet = 1;
result = capture.resolve(game, attacker, target);
test.assert(result.captured);
attacker.movement = 1.0;

inherent_planet = 0;
target.native_capture_attempted = true;
random_calls = 0;
result = capture.resolve(game, attacker, target);
test.assert(result.attempted && !result.captured && result.mark_attempted);
test.assert(result.reason == 'previously_attempted');
test.assert(random_calls == 0);
target.native_capture_attempted = false;

bases = [{
	id: 5,
	get_owner: () => { return human; },
	get_tile: () => { return attacker_tile; },
}];
ecological_damage = {value: 1};
result = capture.resolve(game, attacker, target);
test.assert(result.attempted && !result.captured && !result.mark_attempted);
test.assert(result.reason == 'agitated');
ecological_damage = {value: 0};
bases = [];

const owned_worm = make_unit(20, human, 'MindWorms', true, attacker_tile);
units :+owned_worm;
random_values = [0, 10];
random_calls = 0;
result = capture.resolve(game, attacker, target);
test.assert(result.captured && result.reason == 'roll');
test.assert(random_calls == 2);

const second_owned_worm = make_unit(21, human, 'MindWorms', true, attacker_tile);
units :+second_owned_worm;
random_values = [0, 0, 0];
random_calls = 0;
result = capture.resolve(game, attacker, target);
test.assert(result.captured);
test.assert(random_calls == 3);

planet_rating = 0;
random_calls = 0;
result = capture.resolve(game, attacker, target);
test.assert(!result.attempted && random_calls == 0);
planet_rating = 1;

attacker.is_air = true;
result = capture.resolve(game, attacker, target);
test.assert(!result.attempted);
attacker.is_air = false;

const tamed_target = make_unit(30, rival, 'MindWorms', true, target_tile);
result = capture.resolve(game, attacker, tamed_target);
test.assert(!result.attempted);

const tower = make_unit(31, planet, 'FungalTower', true, target_tile);
result = capture.resolve(game, attacker, tower);
test.assert(!result.attempted);

let stored = {};
const key = (id) => { return 'u' + #to_string(id); };
const transfer_tile = {
	x: 8,
	y: 6,
	get_units: (include_embarked) => {
		let result = [];
		for (id in stored) {
			if (stored[id] != null && stored[id].get_tile() == transfer_tile) {
				result :+stored[id];
			}
		}
		return result;
	},
};
const home_tile = {x: 6, y: 6};
const players = {p1: human, p7: planet};
const make_stored_unit = (data) => {
	let unit = {};
	unit.id = data.id;
	unit.def = data.def;
	unit.owner = data.owner.id;
	unit.movement = #is_defined(data.movement) ? data.movement : 1.0;
	unit.morale = data.morale;
	unit.health = data.health;
	unit.moved_this_turn = #is_defined(data.moved_this_turn) ? data.moved_this_turn : false;
	unit.terraforming = #is_defined(data.terraforming) ? data.terraforming : 'none';
	unit.terraforming_turns_remaining = #is_defined(data.terraforming_turns_remaining)
		? data.terraforming_turns_remaining : 0;
	unit.home_base_id = #is_defined(data.home_base_id) ? data.home_base_id : 0;
	unit.fuel = data.fuel;
	unit.transport_id = #is_defined(data.transport_id) ? data.transport_id : 0;
	unit.native_capture_attempted = #is_defined(data.native_capture_attempted)
		? data.native_capture_attempted : false;
	unit.get_owner = () => { return data.owner; };
	unit.get_tile = () => { return data.tile; };
	unit.get_def = () => { return {id: data.def, name: data.def, is_native: true}; };
	unit.set_home_base_id = (id) => { unit.home_base_id = id; };
	const unit_key = key(unit.id);
	stored[unit_key] = unit;
	return unit;
};

const isle = make_stored_unit({
	id: 100, def: 'IsleOfTheDeep', owner: planet, tile: transfer_tile,
	morale: 2, health: 0.8, fuel: 0, movement: 1.5, native_capture_attempted: true,
});
const cargo = make_stored_unit({
	id: 101, def: 'MindWorms', owner: planet, tile: transfer_tile,
	morale: 1, health: 0.7, fuel: 0, movement: 0.5, transport_id: 100,
});
const transfer_game = {
	get_player: (id) => {
		const player_key = 'p' + #to_string(id);
		return players[player_key];
	},
	um: {
		has_unit: (id) => {
			const unit_key = key(id);
			return #is_defined(stored[unit_key]) && stored[unit_key] != null;
		},
		get_unit: (id) => {
			const unit_key = key(id);
			return stored[unit_key];
		},
		get_units: (include_embarked) => {
			let result = [];
			for (id in stored) {
				if (stored[id] != null) { result :+stored[id]; }
			}
			return result;
		},
		despawn_unit: (unit) => {
			const unit_key = key(unit.id);
			stored[unit_key] = null;
		},
		spawn_unit: (data) => { return make_stored_unit(data); },
	},
	tm: {
		get_tile: (x, y) => { return x == transfer_tile.x ? transfer_tile : home_tile; },
		get_distance: (from, to) => { return 1; },
	},
	bm: {
		get_bases: () => {
			return [{
				id: 5,
				get_owner: () => { return human; },
				get_tile: () => { return home_tile; },
				get_size: () => { return 4; },
			}];
		},
	},
};

const applied = capture.apply(transfer_game, human, transfer_tile, {
	unit_ids: [isle.id, cargo.id],
});
test.assert(stored.u100.owner == human.id && stored.u101.owner == human.id);
test.assert(stored.u100.movement == 0.0 && stored.u101.movement == 0.0);
test.assert(stored.u100.moved_this_turn && stored.u101.moved_this_turn);
test.assert(stored.u100.home_base_id == 5 && stored.u101.home_base_id == 5);
test.assert(stored.u101.transport_id == 100);
test.assert(!stored.u100.native_capture_attempted);

capture.rollback(transfer_game, applied);
test.assert(stored.u100.owner == planet.id && stored.u101.owner == planet.id);
test.assert(stored.u100.movement == 1.5 && stored.u101.movement == 0.5);
test.assert(stored.u100.health == 0.8 && stored.u101.health == 0.7);
test.assert(stored.u100.home_base_id == 0 && stored.u101.home_base_id == 0);
test.assert(stored.u101.transport_id == 100);
test.assert(stored.u100.native_capture_attempted);
