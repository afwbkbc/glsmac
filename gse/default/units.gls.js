let native_lifeform = (name, base_y) => {
    return [name, {
        type: 'static',
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
    native_lifeform('FungalTower', 79),
    native_lifeform('MindWorms', 233),
    native_lifeform('SeaLurk', 310),
    native_lifeform('SporeLauncher', 387),
];
