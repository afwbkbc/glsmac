#game.on.configure(() => {

    #print('CONFIGURE');

    const rules = #include('rules');

    // TODO

});

#game.on.start(() => {

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
                    let tile = #game.map.get_tile(x, y);
                    let unit = null;
                    if (tile.is_land) {
                        if (#game.random.get_int(0, 2) != 1) {
                            unit = #game.units.spawn('MindWorms', tile);
                        } else {
                            unit = #game.units.spawn('SporeLauncher', tile);
                        }
                    } else {
                        if (#game.random.get_int(0, 1) == 1) {
                            unit = #game.units.spawn('SeaLurk', tile);
                        }
                    }
                }
            }
            x++;
        }
        y++;
    }
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
        #game.units.spawn('MindWorms', unit.get_tile());
    }
});
