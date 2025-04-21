return (game) => {

	for (e of [
		'spawn_unit',
		'despawn_unit'
	]) {
		game.register_event(e, #include('event/' + e));
	}

};
