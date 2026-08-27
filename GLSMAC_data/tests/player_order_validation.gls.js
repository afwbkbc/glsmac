const move_unit = #include('../default/game/event/move_unit');
const attack_unit = #include('../default/game/event/attack_unit');
const unit_skip_turn = #include('../default/game/event/unit_skip_turn');
const work_base_tile = #include('../default/game/event/work_base_tile');
const unwork_base_tile = #include('../default/game/event/unwork_base_tile');
const terraform_tile = #include('../default/game/event/terraform_tile');
const cancel_terraform = #include('../default/game/event/cancel_terraform');
const found_base = #include('../default/game/event/found_base');

const player_id = 1;
const completed_error = 'Player has already completed this turn';
const game = {
	is_turn_complete: (id) => {
		test.assert(id == player_id);
		return true;
	},
};
const unit = {owner: player_id};
const base = {
	get_owner: () => {
		return {id: player_id};
	},
};

test.assert(move_unit.validate({
	caller: player_id,
	game: game,
	data: {unit: unit},
}) == completed_error);

test.assert(attack_unit.validate({
	caller: player_id,
	game: game,
	data: {
		attacker: unit,
		defender: {owner: 0},
	},
}) == completed_error);

test.assert(attack_unit.validate({
	caller: player_id,
	game: {is_turn_complete: () => { return false; }},
	data: {
		attacker: {owner: player_id, health: 0.0},
		defender: {owner: 2, health: 1.0},
	},
}) == 'Dead unit cannot attack');

test.assert(attack_unit.validate({
	caller: player_id,
	game: {is_turn_complete: () => { return false; }},
	data: {
		attacker: {owner: player_id, health: 1.0},
		defender: {owner: 2, health: 0.0},
	},
}) == 'Dead unit cannot be attacked');

const active_game = {is_turn_complete: () => { return false; }};
const dead_unit = {owner: player_id, health: 0.0};
test.assert(move_unit.validate({
	caller: player_id,
	game: active_game,
	data: {unit: dead_unit},
}) == 'Dead unit cannot move');
test.assert(unit_skip_turn.validate({
	caller: player_id,
	game: active_game,
	data: {unit: dead_unit},
}) == 'Dead unit cannot skip its turn');
test.assert(terraform_tile.validate({
	caller: player_id,
	game: active_game,
	data: {unit: dead_unit, type: 'farm'},
}) == 'Dead unit cannot terraform');
test.assert(cancel_terraform.validate({
	caller: player_id,
	game: active_game,
	data: {unit: dead_unit},
}) == 'Dead unit cannot cancel terraforming');
test.assert(found_base.validate({
	caller: player_id,
	game: active_game,
	data: {unit: dead_unit},
}) == 'Dead unit cannot found a base');

test.assert(unit_skip_turn.validate({
	caller: player_id,
	game: game,
	data: {unit: unit},
}) == completed_error);

test.assert(work_base_tile.validate({
	caller: player_id,
	game: game,
	data: {base: base},
}) == completed_error);

test.assert(unwork_base_tile.validate({
	caller: player_id,
	game: game,
	data: {base: base},
}) == completed_error);

const center_tile = {
	get_base: () => { return {}; },
	has: (key) => { return false; },
};
const available_game = {
	is_turn_complete: (id) => { return false; },
};
const available_base = {
	get_owner: () => { return {id: player_id}; },
	get_workable_tiles: () => { return [center_tile]; },
};
const available_pop = {
	get_base: () => { return available_base; },
};
test.assert(work_base_tile.validate({
	caller: player_id,
	game: available_game,
	data: {base: available_base, pop: available_pop, tile: center_tile},
}) == 'Base centers cannot be worked');
