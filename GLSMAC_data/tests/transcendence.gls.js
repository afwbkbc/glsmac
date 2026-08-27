const configure_transcendence = #include('../default/game/transcendence');

let callbacks = {};
let project_base = #undefined;
let declarations = [];
let game_over = false;
let is_master = true;
let turn = 1;
const game = {
	on: (name, callback) => { callbacks[name] = callback; },
	set: (name, callback) => { callbacks[name] = callback; },
	is_master: () => { return is_master; },
	is_game_over: () => { return game_over; },
	get_turn: () => { return turn; },
	get_bm: () => {
		return {
			get_project_base: (id) => {
				test.assert(id == 'TheAscentToTranscendence');
				return project_base;
			},
		};
	},
	event: (name, data) => { declarations :+{name: name, data: data}; },
};

configure_transcendence(game);
callbacks.start({});
test.assert(#is_defined(callbacks.f_check_transcendence_victory));

callbacks.update_base({});
test.assert(declarations == []);
project_base = {get_owner: () => { return {id: 3}; }};
is_master = false;
callbacks.turn({});
test.assert(declarations == []);
is_master = true;
turn = 0;
callbacks.turn({});
test.assert(declarations == []);
turn = 4;
callbacks.update_base({});
test.assert(declarations == [{
	name: 'declare_victory',
	data: {type: 'transcendence', winner_id: 3},
}]);
callbacks.turn({});
test.assert(#sizeof(declarations) == 1);
