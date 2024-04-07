const ANIMATION = {
    PSI: 'WEAPON_PSI',
};

const animations = [
    [ANIMATION.PSI, {
        type: 'frames_row',
        file: 'xi.pcx',
        row_x: 2, row_y: 394,
        frame_width: 43, frame_height: 47,
        frame_padding: 2,
        frames_count: 16,
        duration_ms: 160,
        sound: 'psi attack.wav',
    }],
];

const result = {

    ANIMATION: ANIMATION,

    define: () => {

        let sz = #size(animations);
        let i = 0;
        while (i < sz) {
            #game.animations.define(animations[i][0], animations[i][1]);
            i++;
        }

    },

};

return result;
