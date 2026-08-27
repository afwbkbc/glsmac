const action_state = #include('../default/game/ai/action_state');

const tile_a = {id: 'a'};
const tile_b = {id: 'b'};
let tile = tile_a;
const unit = {
	id: 7,
	movement: 1.0,
	health: 1.0,
	moved_this_turn: false,
	is_immovable: false,
	terraforming: 'none',
	get_tile: () => { return tile; },
};
let attempts = {};

test.assert(action_state.can_attempt_action(unit, attempts));
action_state.record_action_attempt(unit, attempts);
test.assert(!action_state.can_attempt_action(unit, attempts));

for (let check = 0; check < 9; check++) {
	test.assert(action_state.refresh_pending_actions([unit], attempts));
	test.assert(!action_state.can_attempt_action(unit, attempts));
}
test.assert(!action_state.refresh_pending_actions([unit], attempts));
test.assert(action_state.can_attempt_action(unit, attempts));

action_state.record_action_attempt(unit, attempts);
tile = tile_b;
test.assert(!action_state.refresh_pending_actions([unit], attempts));
test.assert(action_state.can_attempt_action(unit, attempts));

action_state.record_action_attempt(unit, attempts);
unit.movement = 0.0;
test.assert(!action_state.refresh_pending_actions([unit], attempts));
test.assert(!action_state.can_attempt_action(unit, attempts));

unit.movement = 1.0;
unit.terraforming = 'farm';
test.assert(!action_state.can_attempt_action(unit, attempts));
unit.terraforming = 'none';
unit.is_immovable = true;
test.assert(!action_state.can_attempt_action(unit, attempts));

test.assert(!action_state.refresh_pending_actions([], attempts));

let current_locked = false;
let adjacent_locked = false;
const adjacent_tile = {is_locked: () => { return adjacent_locked; }};
const animation_tile = {
	is_locked: () => { return current_locked; },
	get_surrounding_tiles: () => { return [adjacent_tile]; },
};
const animation_unit = {get_tile: () => { return animation_tile; }};
test.assert(!action_state.has_nearby_animation(animation_unit));
adjacent_locked = true;
test.assert(action_state.has_nearby_animation(animation_unit));
adjacent_locked = false;
current_locked = true;
test.assert(action_state.has_nearby_animation(animation_unit));
