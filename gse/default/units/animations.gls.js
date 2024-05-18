const result = {

	ATTACK_PSI: 'ATTACK_PSI',

	DEATH_PSI: 'DEATH_PSI',

	define: () => {

		const animations = [

			[this.ATTACK_PSI, {
				type: 'frames_row',
				file: 'xi.pcx',
				row_x: 2, row_y: 394,
				frame_width: 43, frame_height: 47,
				frame_padding: 2,
				frames_count: 16,
				scale_x: 0.7,
				scale_y: 0.7,
				duration_ms: 250,
				sound: 'psi attack.wav',
			}],

			[this.DEATH_PSI, {
				type: 'frames_row',
				file: 'XF.pcx',
				row_x: 2, row_y: 914,
				frame_width: 100, frame_height: 150,
				frame_center_x: 50, frame_center_y: 125,
				frame_padding: 2,
				frames_count: 16,
				frames_per_row: 8,
				scale_x: 1.0,
				scale_y: 1.0,
				duration_ms: 750,
				sound: 'psi blow.wav',
			}],

		];

		let sz = #size(animations);
		let i = 0;
		while (i < sz) {
			#game.animations.define(animations[i][0], animations[i][1]);
			i++;
		}

	},

};

return result;
