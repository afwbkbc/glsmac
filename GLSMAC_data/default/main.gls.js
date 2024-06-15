const units = #include('units');

#game.on.configure((e) => {

	const factions = #include('factions');
	let i = 0;
	let sz = #size(factions);
	while (i < sz) {
		#game.factions.define(factions[i][0], factions[i][1]);
		i++;
	}

	const rules = #include('rules');
	// TODO

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

	let y = 0;
	let w = #game.map.get_width();
	let h = #game.map.get_height();

	while (y < h) {
		let x = 0;
		while (x < w) {
			if (x % 2 == y % 2) {
				if (#game.random.get_int(0, 6) == 0) {
					let owner = random_player();
					let tile = #game.map.get_tile(x, y);
					let units_count = #game.random.get_int(1, 2);
					let i = 0;
					while (i++ < units_count) {
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
			}
			x++;
		}
		y++;
	}
	#game.message('Total units spawned: ' + #to_string(units_spawned));

	// spawn bases

	let i = 0;
	while (i < players_sz) {
		let base_x = 0;
		let base_y = 1;
		while (base_x % 2 != base_y % 2) {
			base_x = #game.random.get_int(0, w - 1);
			base_y = #game.random.get_int(0, h - 1);
		}
		#game.bases.spawn(
			players[i],
			#game.map.get_tile(base_x, base_y)
		);
		i++;
	}

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
