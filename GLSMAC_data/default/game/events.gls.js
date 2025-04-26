return (game) => {

	for (e of [
		'define_resource',
		'define_animation',
		'define_moraleset',
		'define_unit',
		'spawn_unit',
		'despawn_unit',
		'move_unit',
	]) {
		game.register_event(e, #include('event/' + e));
	}

};
