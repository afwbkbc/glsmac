const rules = #include('rules');
const units = #include('units');

let i = 0;
while (i < #size(units)) {
    #game.units.define(units[i][0], units[i][1]);
    i++;
}

#game.on_start(() => {
    let y = 0;
    while (y < #game.map.height) {
        let x = 0;
        while (x < #game.map.width) {
            if (x % 2 == y % 2) {
                if (#game.random.get_int(0, 1) == 1) {
                    let tile = #game.map.get_tile(x, y);
                    if (tile.is_land) {
                        if (#game.random.get_int(0, 2) != 1) {
                            #game.units.spawn('MindWorms', tile);
                        } else {
                            #game.units.spawn('SporeLauncher', tile);
                        }
                    } else {
                        if (#game.random.get_int(0, 1) == 1) {
                            #game.units.spawn('SeaLurk', tile);
                        }
                    }
                }
            }
            x++;
        }
        y++;
    }
    //#game.units.spawn('MINDWORMS', #game.map.get_tile(0, 0));
});
