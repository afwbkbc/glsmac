#game.on.configure(() => {

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

#game.on.start(() => {

    // init players
    players = #game.players.get_all();
    players_sz = #size(players);

    const units = #include('units');
    let i = 0;
    let sz = #size(units);
    while (i < sz) {
        #game.units.define(units[i][0], units[i][1]);
        i++;
    }

    let y = 0;
    let w = #game.map.get_width();
    let h = #game.map.get_height();
    while (y < h) {
        let x = 0;
        while (x < w) {
            if (x % 2 == y % 2) {
                if (#game.random.get_int(0, 1) == 1) {
                    let owner = random_player();
                    let tile = #game.map.get_tile(x, y);
                    let unit = null;
                    if (tile.is_land) {
                        if (#game.random.get_int(0, 2) != 1) {
                            unit = #game.units.spawn('MindWorms', owner, tile, random_morale(), random_health());
                        } else {
                            unit = #game.units.spawn('SporeLauncher', owner, tile, random_morale(), random_health());
                        }
                    } else {
                        if (#game.random.get_int(0, 1) == 1) {
                            unit = #game.units.spawn('SeaLurk', owner, tile, random_morale(), random_health());
                        }
                    }
                }
            }
            x++;
        }
        y++;
    }
});

#game.on.turn(() => {
    #print('NEW TURN');
    #print('PLAYERS: ' + #to_string(#game.players.get_all()));
});

#game.on.spawn_unit((unit) => {
    let def = unit.get_def();
    if (def.name != 'MindWorms') {
        let tile = unit.get_tile();
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
            #game.units.despawn(unit);
        }
    }
});

#game.on.despawn_unit((unit) => {
    if (unit.get_def() == 'SporeLauncher') {
        #game.units.spawn('MindWorms', random_player(), unit.get_tile(), random_morale(), random_health());
    }
});
