const result = {

	define: () => {
		#game.on.tiles_update((e) => {
			for (tile of e.tiles) {
				#print('TILE', tile.x, tile.y);
			}
		});
	},

};

return result;
