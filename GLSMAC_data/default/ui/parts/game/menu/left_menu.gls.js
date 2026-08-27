return {

	init: (p) => {
		return p.create([
			{
				label: 'GAME',
				open: () => {
					p.menu.open('left_menu_game', {
						align: 'bottom',
						left: p.menu.menu_width,
						bottom: 256 + 166 - p.menu.item_height * 3,
					});
				},
				close: () => {
					p.menu.close('left_menu_game');
				},
			},
			{
				label: 'SOCIAL',
				open: () => {
					p.modules.popup.show('social_engineering');
				},
			},
			{
				label: 'DIPLOMACY',
				open: () => {
					p.modules.popup.show('diplomacy');
				},
			},
		]);
	},

};
