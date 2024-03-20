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
    return #game.random.get_int(1, 7); // TODO: get some constants from api
};

let random_health = () => {
    return #game.random.get_float(#to_float(0), #to_float(1));
};

units.init();

#game.on.start((e) => {

    // init units
    units.define();

    // init players
    players = #game.players.get_all();
    players_sz = #size(players);

    let units_spawned = 0;

    let y = 0;
    let w = #game.map.get_width();
    let h = #game.map.get_height();
    while (y < h) {
        let x = 0;
        while (x < w) {
            if (x % 2 == y % 2) {
                if (#game.random.get_int(0, 24) == 0) {
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
                                    while (morale < 7 && i < sz) {
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
});

#game.on.turn((e) => {
    #print('NEW TURN');
});

#game.on.unit_spawn((e) => {
    let def = e.unit.get_def();
    if (def.name != 'MindWorms') {
        let tile = e.unit.get_tile();
        let neighbours = [tile.get_W(), tile.get_NW(), tile.get_N(), tile.get_NE(), tile.get_E(), tile.get_SE(), tile.get_S(), tile.get_SW()];
        let nearby_units_count = 0;
        let i = 0;
        let sz = #size(neighbours);
        while (i < sz) {
            if (!#is_empty(neighbours[i].get_units())) {
                nearby_units_count++;
            }
            i++;
        }
        if (nearby_units_count > 2) {
            #game.units.despawn(e.unit);
        }
    }
});

#game.on.unit_despawn((e) => {
    if (e.unit.get_def() == 'SporeLauncher') {
        #game.units.spawn('MindWorms', random_player(), e.unit.get_tile(), random_morale(), random_health());
    }
});
