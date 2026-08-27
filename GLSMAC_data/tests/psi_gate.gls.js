const rules = #include('../default/game/psi_gate_rules');
const teleport = #include('../default/game/event/teleport_unit');

let current_turn = 12;
let turn_complete = false;
const player = {id: 1};

const make_endpoint = (id, name) => {
	let custom = {};
	let base = null;
	const tile = {
		x: id * 2,
		y: 4,
		is_locked: () => { return false; },
		get_base: () => { return base; },
		get_units: (include_embarked) => { return []; },
	};
	base = {
		id: id,
		name: name,
		get_owner: () => { return player; },
		get_tile: () => { return tile; },
		has_facility: (facility) => { return facility == 'PsiGate'; },
		has: (key) => { return #is_defined(custom[key]); },
		get: (key) => { return custom[key]; },
		set: (key, value) => { custom[key] = value; },
		unset: (key) => { custom[key] = #undefined; },
	};
	return base;
};

const source = make_endpoint(1, 'Source');
const destination = make_endpoint(2, 'Destination');
const alternate = make_endpoint(3, 'Alternate');
let unit_tile = source.get_tile();
const unit = {
	id: 7,
	owner: player.id,
	health: 1.0,
	transport_id: 0,
	terraforming: 'none',
	movement: 1.5,
	moved_this_turn: true,
	get_tile: () => { return unit_tile; },
	teleport_to_tile: (tile) => { unit_tile = tile; },
};
const bases = [source, destination, alternate];
const game = {
	get_turn: () => { return current_turn; },
	is_turn_complete: (id) => { return turn_complete; },
	get_bm: () => { return {get_bases: () => { return bases; }}; },
};

let event = {
	caller: player.id,
	game: game,
	data: {unit: unit, destination: destination},
};
test.assert(rules.is_available(game, source));
test.assert(!#is_defined(teleport.validate(event)));
test.assert(#sizeof(rules.get_available_destinations(game, unit, player.id)) == 2);
const old_movement = unit.movement;
const old_moved = unit.moved_this_turn;
event.applied = teleport.apply(event);
test.assert(unit.get_tile() == destination.get_tile());
test.assert(unit.movement == old_movement);
test.assert(unit.moved_this_turn == old_moved);
test.assert(rules.get_base_used_turn(source) == current_turn);
test.assert(rules.get_base_used_turn(destination) == current_turn);
test.assert(#is_defined(teleport.validate({
	caller: player.id,
	game: game,
	data: {unit: unit, destination: alternate},
})));

teleport.rollback(event);
test.assert(unit.get_tile() == source.get_tile());
test.assert(!source.has(rules.used_turn_key));
test.assert(!destination.has(rules.used_turn_key));

source.set(rules.used_turn_key, current_turn - 1);
test.assert(rules.is_available(game, source));
current_turn++;
test.assert(rules.is_available(game, source));

unit.movement = 0.0;
test.assert(#is_defined(teleport.validate({
	caller: player.id,
	game: game,
	data: {unit: unit, destination: destination},
})));
unit.movement = 1.0;
turn_complete = true;
test.assert(#is_defined(teleport.validate({
	caller: player.id,
	game: game,
	data: {unit: unit, destination: destination},
})));
