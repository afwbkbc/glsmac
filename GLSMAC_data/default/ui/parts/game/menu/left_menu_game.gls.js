return {

	init: (p) => {
		return p.create([
			{
				label: 'Start New Game',
				open: () => {
					p.menu.close('left_menu');
					#print('TODO: START NEW');
				},
			},
			{
				label: 'Quit',
				open: () => {
					p.maybe_quit();
				},
			},
		]);
	},

};
