return {

	init: (p) => {
		return p.create([
			{
				label: 'Start New Game',
				open: () => {
					p.maybe_quit(false);
				},
			},
			{
				label: 'Planetary Council',
				open: () => {
					p.modules.popup.show('planetary_council');
				},
			},
			{
				label: 'Global Market',
				open: () => {
					p.modules.popup.show('economic_victory');
				},
			},
			{
				label: 'Quit',
				open: () => {
					p.maybe_quit(true);
				},
			},
		]);
	},

};
