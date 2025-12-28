return (glsmac) => {

	const factions = #include('../factions');
	const resources = #include('../resources');
	const units = #include('../units');
	const bases = #include('../bases');

	glsmac.on('configure_state', (e) => {
		factions.configure(e.fm);
	});

	let tiles_with_bases = {};

	#include('events')(glsmac.game);

	glsmac.on('configure_game', (e) => {

		let players = [];
		let players_sz = 0;
		let random_player = () => {
			return players[(e.game.random.get_int(0, players_sz - 1))];
		};

		let random_morale = () => {
			return e.game.random.get_int(0, 6); // TODO: get some constants from api
		};

		let random_health = () => {
			return e.game.random.get_float(#to_float(0.1), #to_float(1));
		};

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

		e.game.on('configure', (e) => {

			const game = e.game; // TODO: rework to not use global object inside events

			e.game.um.on('unit_spawn', (e) => {
				//
			});

			e.game.um.on('unit_despawn', (e) => {
				//
			});

			e.game.bm.on('base_spawn', (e) => {
				add_pops(game, e.base, game.random.get_int(1, 7));
			});

			units.configure(e.game);
			resources.configure(e.game);
		});

		e.game.on('create_world', (e) => {


			// init game data
			resources.define(e.game);
			units.define(e.game);
			bases.define(e.game);

			// init players
			players = e.game.get_players();
			players_sz = #sizeof(players);

			// spawn units

			let units_spawned = 0;
			let bases_spawned = 0;

			let w = e.game.tm.get_map_width();
			let h = e.game.tm.get_map_height();

			for (let y = 0; y < h; y++) {
				for (let x = 0; x < w; x++) {
					if (x % 2 == y % 2) {
						let owner = random_player();

						let tile = e.game.tm.get_tile(x, y);
						if (e.game.random.get_int(0, 6) == 0) {
							let units_count = e.game.random.get_int(1, 2);
							for (let i = 0; i < units_count; i++) {
								if (tile.is_land) {
									if (e.game.random.get_int(0, 4) != 0) {

										e.game.event('spawn_unit', {
											type: 'MindWorms',
											owner: owner,
											tile: tile,
											morale: random_morale(),
											health: random_health(),
										});

										units_spawned++;
									} else {
										if (tile.has_fungus && e.game.random.get_int(0, 3) == 0) {
											// morale depends on count of fungus tiles around
											let morale = 1;
											for (neighbour of tile.get_surrounding_tiles()) {
												if (morale >= 6) {
													break;
												}
												if (neighbour.has_fungus) {
													morale++;
												}
											}

											e.game.event('spawn_unit', {
												type: 'FungalTower',
												owner: owner,
												tile: tile,
												morale: morale,
												health: random_health(),
											});

											units_spawned++;
										} else {

											e.game.event('spawn_unit', {
												type: 'SporeLauncher',
												owner: owner,
												tile: tile,
												morale: random_morale(),
												health: random_health(),
											});

											units_spawned++;
										}
									}
								} else {
									if (e.game.random.get_int(0, 3) == 0) {

										e.game.event('spawn_unit', {
											type: 'SeaLurk',
											owner: owner,
											tile: tile,
											morale: random_morale(),
											health: random_health(),
										});

										units_spawned++;
									}
								}
							}
						}
						if (e.game.random.get_int(0, 5) == 0) {
							let has_adjactent_bases = false;
							for (neighbour of tile.get_surrounding_tiles()) {
								const key = #to_string(neighbour.x) + 'x' + #to_string(neighbour.y); // TODO: sets
								if (#is_defined(tiles_with_bases[key])) {
									has_adjactent_bases = true;
									break;
								}
							}
							if (!has_adjactent_bases) {
								e.game.event('spawn_base', {
									owner: owner,
									tile: tile,
									// name: 'base name',
								});
								tiles_with_bases[#to_string(tile.x) + 'x' + #to_string(tile.y)] = true;
								bases_spawned++;
							}
						}
					}
				}
			}
			e.game.message('Total units spawned: ' + #to_string(units_spawned));
			e.game.message('Total bases spawned: ' + #to_string(bases_spawned));

		});

		e.game.on('error', (e) => {
			//glsmac.ui.error('Game initialization failed', () => {
			//glsmac.reset();
			//});
		});

		e.game.on('turn', (e) => {
			for (base of e.game.bm.get_bases()) {
				add_pops(e.game, base, 1);
			}
		});

	});

};
