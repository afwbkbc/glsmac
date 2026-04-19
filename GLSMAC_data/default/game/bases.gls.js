const process_population = (game, base) => {
	const pops = base.get_pops();

	let grow = false;

	// always grow first worker
	if (#is_empty(pops)) {
		grow = true;
	}

	if (grow) {
		game.event('add_base_pop', {
			base: base,
			type: 'Worker',
		});
	}
};

return (game) => {

	const bm = game.get_bm();

	// new turn, process all bases
	game.on('turn', (e) => {
		for (base of bm.get_bases()) {

			process_population(game, base);

		}
	});

};
