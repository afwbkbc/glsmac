/*
	populates map with random units and bases
	for testing purposes
 */

return (game) => {

	const players = game.get_players();
	const players_sz = #sizeof(players);

	let random_player = () => {
		return players[(game.random.get_int(0, players_sz - 1))];
	};

	let random_morale = () => {
		return game.random.get_int(0, 6); // TODO: get some constants from api
	};

	let random_health = () => {
		return game.random.get_float(#to_float(0.1), #to_float(1));
	};

	let tiles_with_bases = {};

	// spawn units

	let units_spawned = 0;
	let bases_spawned = 0;

	const tm = game.get_tm();
	let w = tm.get_map_width();
	let h = tm.get_map_height();

	for (let y = 0; y < h; y++) {
		for (let x = 0; x < w; x++) {
			if (x % 2 == y % 2) {
				let owner = random_player();

				let tile = tm.get_tile(x, y);
				if (game.random.get_int(0, 6) == 0) {
					let units_count = game.random.get_int(1, 2);
					for (let i = 0; i < units_count; i++) {
						if (tile.is_land) {
							if (game.random.get_int(0, 4) != 0) {

								game.event('spawn_unit', {
									type: 'MindWorms',
									owner: owner,
									tile: tile,
									morale: random_morale(),
									health: random_health(),
								});

								units_spawned++;
							} else {
								if (tile.features.xenofungus && game.random.get_int(0, 3) == 0) {
									// morale depends on count of fungus tiles around
									let morale = 1;
									for (neighbour of tile.get_surrounding_tiles()) {
										if (morale >= 6) {
											break;
										}
										if (neighbour.features.xenofungus) {
											morale++;
										}
									}

									game.event('spawn_unit', {
										type: 'FungalTower',
										owner: owner,
										tile: tile,
										morale: morale,
										health: random_health(),
									});

									units_spawned++;
								} else {

									game.event('spawn_unit', {
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
							if (game.random.get_int(0, 3) == 0) {

								game.event('spawn_unit', {
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
				if (game.random.get_int(0, 5) == 0) {
					let has_adjactent_bases = false;
					for (neighbour of tile.get_surrounding_tiles()) {
						const key = #to_string(neighbour.x) + 'x' + #to_string(neighbour.y); // TODO: sets
						if (#is_defined(tiles_with_bases[key])) {
							has_adjactent_bases = true;
							break;
						}
					}
					if (!has_adjactent_bases) {
						game.event('spawn_base', {
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
	game.message('Total units spawned: ' + #to_string(units_spawned));
	game.message('Total bases spawned: ' + #to_string(bases_spawned));

};
