return {

	init: (p) => {
		return p.create([
			{
				label: 'GAME',
				open: () => {
					p.menu.open('left_menu_game', {
						align: 'bottom',
						left: p.menu_width,
						bottom: 256 + 166 - p.item_height * 2, // TODO: get from parent item
					});
				},
				close: () => {
					p.menu.close('left_menu_game');
				},
			},
		]);
	},

};
