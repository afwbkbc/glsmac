const factions = #include('factions');
const resources = #include('resources');
const units = #include('units');
const bases = #include('bases');

return ( (gm) => {

	factions.configure(gm.fm);

	gm.on('start', (e) => {

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

		let add_pops = (base, count) => {
			for (let i = 0; i < count; i++) {
				const type = pop_types[(e.game.random.get_int(0, #sizeof(pop_types) - 1))];
				base.create_pop({
					type: type,
				});
			}
		};

		let all_bases = [];

		e.game.on('configure', (e) => {

			units.configure(e.game);
			resources.configure(e.game.rm);

		});

		e.game.on('start', (e) => {

			// init game data
			units.define(e.game);
			bases.define(e.game.bm);

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
										e.game.um.spawn_unit('MindWorms', owner, tile, random_morale(), random_health());
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
											e.game.um.spawn_unit('FungalTower', owner, tile, morale, random_health());
											units_spawned++;
										} else {
											e.game.um.spawn_unit('SporeLauncher', owner, tile, random_morale(), random_health());
											units_spawned++;
										}
									}
								} else {
									if (e.game.random.get_int(0, 3) == 0) {
										e.game.um.spawn_unit('SeaLurk', owner, tile, random_morale(), random_health());
										units_spawned++;
									}
								}
							}
						}
						if (e.game.random.get_int(0, 5) == 0) {
							let has_adjactent_bases = false;
							for (neighbour of tile.get_surrounding_tiles()) {
								if (neighbour.get_base() != null) {
									has_adjactent_bases = true;
									break;
								}
							}
							if (!has_adjactent_bases) {
								let base = e.game.bm.spawn_base(
									owner,
									tile,
									{
										// name: 'base name',
									}
								);
								add_pops(base, e.game.random.get_int(1, 7));
								all_bases :+ base;
								bases_spawned++;
							}
						}
					}
				}
			}
			e.game.message('Total units spawned: ' + #to_string(units_spawned));
			e.game.message('Total bases spawned: ' + #to_string(bases_spawned));

		});

		e.game.on('turn', (e) => {
			for ( base of all_bases ) {
				add_pops(base, 1);
			}
			//
		});

		e.game.um.on('unit_spawn', (e) => {
			//
		});

		e.game.um.on('unit_despawn', (e) => {
			//
		});

		e.game.bm.on('base_spawn', (e) => {
			//
		});

	});

});
