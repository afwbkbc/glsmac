let native_lifeform = (id, name, movement_type, base_y) => {
    return [id, {
        name: name,
        type: 'static',
        movement_type: movement_type,
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

return [
    native_lifeform('FungalTower', 'Fungal Tower', 'immovable', 79),
    native_lifeform('MindWorms', 'Mind Worms', 'land', 233),
    native_lifeform('SeaLurk', 'Sea Lurk', 'sea', 310),
    native_lifeform('SporeLauncher', 'Spore Launcher', 'land', 387),
];
