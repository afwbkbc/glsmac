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
									for (neighbour of tile.get_surrounding_tiles()) {
										if (morale >= 6) {
											break;
										}
										if (neighbour.has_fungus) {
											morale++;
										}
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
					let has_adjactent_bases = false;
					for (neighbour of tile.get_surrounding_tiles()) {
						if (neighbour.get_base() != null) {
							has_adjactent_bases = true;
							break;
						}
					}
					if (!has_adjactent_bases) {
						#game.bases.spawn(
							owner,
							tile,
							{
								// name: 'base name',
								population: #game.random.get_int(0, 4) * #game.random.get_int(0, 4) + #game.random.get_int(1, 3),
							}
						);
						bases_spawned++;
					}
				}
			}
		}
	}
	#game.message('Total units spawned: ' + #to_string(units_spawned));
	#game.message('Total bases spawned: ' + #to_string(bases_spawned));

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
