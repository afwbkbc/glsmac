return (game) => {

	for (e of [
		'configure_game',
		'define_resource',
		'define_animation',
		'define_moraleset',
		'define_unit',
		'spawn_unit',
		'despawn_unit',
		'move_unit',
		'attack_unit',
		'unit_skip_turn',
		'define_base_pop',
		'spawn_base',
		'add_base_pop',
		'complete_turn',
		'uncomplete_turn',
		'advance_turn',
	]) {
		game.register_event(e, #include('event/' + e));
	}

};
