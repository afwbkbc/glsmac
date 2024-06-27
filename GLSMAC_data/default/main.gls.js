const units = #include('units');
const factions = #include('factions');

#game.on.configure((e) => {

	factions.define();

	// TODO
	const rules = #include('rules');

});

// will be populated on start
let players = [];
let players_sz = 0;
let random_player = () => {
	return players[(#game.random.get_int(0, players_sz - 1))];
};

let random_morale = () => {
	return #game.random.get_int(0, 6); // TODO: get some constants from api
};

let random_health = () => {
	return #game.random.get_float(#to_float(0.1), #to_float(1));
};

units.init();

#game.on.start((e) => {

	// init units
	units.define();

	// init players
	players = #game.players.get_all();
	players_sz = #size(players);

	// spawn units

	let units_spawned = 0;
	let bases_spawned = 0;

	let w = #game.map.get_width();
	let h = #game.map.get_height();

	for (let y = 0; y < h; y++) {
		for (let x = 0; x < w; x++) {
			if (x % 2 == y % 2) {
				let owner = random_player();
				let tile = #game.map.get_tile(x, y);
				if (#game.random.get_int(0, 6) == 0) {
					let units_count = #game.random.get_int(1, 2);
					for (let i = 0; i < units_count; i++) {
						if (tile.is_land) {
							if (#game.random.get_int(0, 4) != 0) {
								#game.units.spawn('MindWorms', owner, tile, random_morale(), random_health());
								units_spawned++;
							} else {
								if (tile.has_fungus && #game.random.get_int(0, 3) == 0) {
									// morale depends on count of fungus tiles around
									let morale = 1;
									let neighbours = tile.get_surrounding_tiles();
									let sz = #size(neighbours);
									let i = 0;
									while (morale < 6 && i < sz) {
										if (neighbours[i].has_fungus) {
											morale++;
										}
										i++;
									}
									#game.units.spawn('FungalTower', owner, tile, morale, random_health());
									units_spawned++;
								} else {
									#game.units.spawn('SporeLauncher', owner, tile, random_morale(), random_health());
									units_spawned++;
								}
							}
						} else {
							if (#game.random.get_int(0, 3) == 0) {
								#game.units.spawn('SeaLurk', owner, tile, random_morale(), random_health());
								units_spawned++;
							}
						}
					}
				}
				if (#game.random.get_int(0, 2) == 0) {
					#game.bases.spawn(
						owner,
						tile,
						{
							// name: 'base name',
						}
					);
					bases_spawned++;
				}
			}
		}
	}
	#game.message('Total units spawned: ' + #to_string(units_spawned));
	#game.message('Total bases spawned: ' + #to_string(bases_spawned));

	// spawn bases

	/*let i = 0;
	while (i < players_sz) {
		let base_x = 0;
		let base_y = 1;
		while (base_x % 2 != base_y % 2) {
			base_x = #game.random.get_int(1, w - 2);
			base_y = #game.random.get_int(1, h - 2);
		}
		#game.bases.spawn(
			players[i],
			#game.map.get_tile(base_x, base_y)
		);
		i++;
	}*/

});

#game.on.turn((e) => {
	//
});

#game.on.unit_spawn((e) => {
	//
});

#game.on.unit_despawn((e) => {
	//
});
