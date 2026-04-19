return (glsmac) => {

	const factions = #include('../factions');
	const resources = #include('../resources');
	const units = #include('../units');
	const bases = #include('../bases');

	glsmac.on('configure_state', (e) => {
		factions.configure(e.fm);
	});

	#include('events')(glsmac.game);

	glsmac.on('configure_game', (e) => {

		const game = e.game;

		// will be populated on start
		const pop_types = [
			'Worker',
			'Talent',
			'Doctor',
			'Librarian',
		];

		let add_pops = (game, base, count) => {
			for (let i = 0; i < count; i++) {
				game.event('add_base_pop', {
					base: base,
					type: pop_types[(game.random.get_int(0, #sizeof(pop_types) - 1))],
				})
			}
		};

		game.on('configure', (e) => {

			const um = game.get_um();
			const bm = game.get_bm();

			um.on('unit_spawn', (e) => {
				//
			});

			um.on('unit_despawn', (e) => {
				//
			});

			bm.on('base_spawn', (e) => {
				add_pops(game, e.base, game.random.get_int(1, 7));
			});

			units.configure(game);
			resources.configure(game);

			resources.define(game);
			units.define(game);
			bases.define(game);

			let worldscript = 'default';
			if (#is_defined(glsmac.config.worldscript)) {
				worldscript = glsmac.config.worldscript;
			}
			game.on('create_world', (e) => {
				const generator = #include('world/' + worldscript);
				generator(e.game);
			});

			game.on('error', (e) => {
				glsmac.ui.error('Game initialization failed', () => {
					if (
						#is_defined(glsmac.config.quickstart) ||
						#is_defined(glsmac.config.host) ||
						#is_defined(glsmac.config.join)
					) {
						glsmac.exit();
					} else {
						glsmac.reset();
					}
				});
			});

			game.on('turn', (e) => {
				for (base of bm.get_bases()) {
					add_pops(game, base, 1);
				}
			});

		});

	});

};
