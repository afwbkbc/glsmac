const move_unit = #include('../default/game/event/move_unit');

const attacker_owner = {id: 1};
const defender_owner = {id: 2};

const make_base = (id, owner, distance, initial_queue, initial_headquarters) => {
	let current_owner = owner;
	let production_queue = initial_queue;
	let has_headquarters = initial_headquarters;
	return {
		id: id,
		get_owner: () => { return current_owner; },
		set_owner: (value) => { current_owner = value; },
		has_facility: (id) => { return id == 'Headquarters' && has_headquarters; },
		remove_facility: (id) => {
			test.assert(id == 'Headquarters' && has_headquarters);
			has_headquarters = false;
		},
		add_facility: (id) => {
			test.assert(id == 'Headquarters' && !has_headquarters);
			has_headquarters = true;
		},
		get_tile: () => { return {distance: distance}; },
		get_production_queue: () => { return production_queue; },
		can_produce: (kind, id) => {
			return current_owner.id != attacker_owner.id || id != 'LockedUnit';
		},
		set_production_queue: (queue) => {
			production_queue = [];
			for (production of queue) {
				production_queue :+{
					production_kind: production.kind,
					id: production.id,
				};
			}
		},
	};
};

const locked_unit = {production_kind: 'unit', id: 'LockedUnit'};
const available_unit = {production_kind: 'unit', id: 'AvailableUnit'};
const captured_base = make_base(9, defender_owner, 0, [locked_unit, available_unit], true);
const higher_id_base = make_base(11, defender_owner, 2, [], false);
const lower_id_base = make_base(10, defender_owner, 2, [], false);
const source = {
	is_land: true,
	features: {river: false, xenofungus: false},
	terraforming: {road: false, forest: false},
	rockiness: 0,
	get_base: () => { return null; },
};
const destination = {
	is_land: true,
	features: {river: false, xenofungus: false},
	terraforming: {road: false, forest: false},
	rockiness: 0,
	get_base: () => { return captured_base; },
};

let current_tile = source;
const capturing_unit = {
	owner: attacker_owner.id,
	movement: 1.0,
	moved_this_turn: false,
	get_owner: () => { return attacker_owner; },
	get_def: () => { return {is_native: false}; },
	get_tile: () => { return current_tile; },
	move_to_tile: (tile, oncomplete) => {
		current_tile = tile;
		oncomplete();
	},
};

let supported_unit = null;
supported_unit = {
	owner: defender_owner.id,
	home_base_id: captured_base.id,
	get_tile: () => { return {distance: 0}; },
	set_home_base_id: (id) => { supported_unit.home_base_id = id; },
};
const unrelated_unit = {
	owner: defender_owner.id,
	home_base_id: lower_id_base.id,
	get_tile: () => { return {distance: 0}; },
	set_home_base_id: (id) => { throw Error('Unrelated unit was rehomed'); },
};

let bases = [captured_base, higher_id_base, lower_id_base];
const game = {
	um: {get_units: () => { return [supported_unit, unrelated_unit]; }},
	bm: {get_bases: () => { return bases; }},
	tm: {
		get_distance: (unit_tile, base_tile) => { return base_tile.distance; },
	},
};

let event = {
	game: game,
	data: {unit: capturing_unit, tile: destination},
	resolved: {is_movement_successful: true},
};
event.applied = move_unit.apply(event);
test.assert(current_tile == destination);
test.assert(captured_base.get_owner() == attacker_owner);
test.assert(!captured_base.has_facility('Headquarters'));
let captured_queue = captured_base.get_production_queue();
test.assert(#sizeof(captured_queue) == 1);
test.assert(captured_queue[0].id == 'AvailableUnit');
test.assert(supported_unit.home_base_id == lower_id_base.id);
test.assert(unrelated_unit.home_base_id == lower_id_base.id);
test.assert(#sizeof(event.applied.rehomed_units) == 1);

move_unit.rollback(event);
test.assert(current_tile == source);
test.assert(captured_base.get_owner() == defender_owner);
test.assert(captured_base.has_facility('Headquarters'));
captured_queue = captured_base.get_production_queue();
test.assert(#sizeof(captured_queue) == 2);
test.assert(captured_queue[0].id == 'LockedUnit');
test.assert(supported_unit.home_base_id == captured_base.id);

bases = [captured_base];
event.applied = move_unit.apply(event);
test.assert(captured_base.get_owner() == attacker_owner);
test.assert(!captured_base.has_facility('Headquarters'));
test.assert(supported_unit.home_base_id == 0);
move_unit.rollback(event);
test.assert(captured_base.get_owner() == defender_owner);
test.assert(captured_base.has_facility('Headquarters'));
test.assert(supported_unit.home_base_id == captured_base.id);
