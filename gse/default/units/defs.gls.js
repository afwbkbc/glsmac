let native_lifeform = (id, name, movement_type, movement_per_turn, base_y) => {
    return [id, {
        name: name,
        type: 'static',
        movement_type: movement_type,
        movement_per_turn: movement_per_turn,
        render: {
            type: 'sprite',
            file: 'Units.pcx',
            x: 2, y: base_y,
            w: 100, h: 75,
            cx: 53, cy: base_y + 51,
            morale_based_xshift: 102
        },
    }];
};

const units = [
    native_lifeform('FungalTower', 'Fungal Tower', 'immovable', 0, 79),
    native_lifeform('MindWorms', 'Mind Worms', 'land', 1, 233),
    native_lifeform('SeaLurk', 'Sea Lurk', 'sea', 4, 310),
    native_lifeform('SporeLauncher', 'Spore Launcher', 'land', 1, 387),
];

return {
    define: () => {
        let i = 0;
        let sz = #size(units);
        while (i < sz) {
            #game.units.define(units[i][0], units[i][1]);
            i++;
        }
    },
}
