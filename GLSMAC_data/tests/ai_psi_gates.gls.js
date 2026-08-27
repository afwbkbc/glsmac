const psi_gates = #include('../default/game/ai/psi_gates');

const player = {id: 1};
let current_turn = 8;
let unit_tile = null;
let event_data = null;

const make_base = (id, x, has_gate) => {
	let custom = {};
	let base = null;
	const tile = {
		x: x,
		y: 0,
		is_locked: () => { return false; },
		get_base: () => { return base; },
		get_units: (include_embarked) => { return []; },
	};
	base = {
		id: id,
		get_owner: () => { return player; },
		get_tile: () => { return tile; },
		has_facility: (facility) => { return facility == 'PsiGate' && has_gate; },
		has: (key) => { return #is_defined(custom[key]); },
		get: (key) => { return custom[key]; },
		set: (key, value) => { custom[key] = value; },
		unset: (key) => { custom[key] = #undefined; },
	};
	return base;
};

const source = make_base(1, 0, true);
const near = make_base(2, 80, true);
const far = make_base(3, 40, true);
const no_gate = make_base(4, 95, false);
unit_tile = source.get_tile();
const unit = {
	owner: player.id,
	health: 1.0,
	transport_id: 0,
	terraforming: 'none',
	movement: 1.0,
	get_tile: () => { return unit_tile; },
};
const target = {
	x: 100,
	y: 0,
};
const game = {
	get_turn: () => { return current_turn; },
	is_turn_complete: (id) => { return false; },
	get_tm: () => {
		return {
			get_distance: (left, right) => { return #abs(left.x - right.x); },
		};
	},
	event_as: (caller, name, data) => {
		event_data = {caller: caller, name: name, data: data};
	},
};
const all_bases = [source, far, no_gate, near];

test.assert(psi_gates.choose_destination(game, player, unit, target, all_bases) == near);
test.assert(psi_gates.try_teleport(game, player, unit, target, all_bases));
test.assert(event_data.caller == player.id);
test.assert(event_data.name == 'teleport_unit');
test.assert(event_data.data.unit == unit);
test.assert(event_data.data.destination == near);

near.set('psi_gate_used_turn', current_turn);
test.assert(psi_gates.choose_destination(game, player, unit, target, all_bases) == far);
far.set('psi_gate_used_turn', current_turn);
test.assert(psi_gates.choose_destination(game, player, unit, target, all_bases) == null);

current_turn++;
test.assert(psi_gates.choose_destination(game, player, unit, target, all_bases) == near);
