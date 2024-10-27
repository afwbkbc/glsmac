const result = {

	define: () => {

		#game.on.get_tile_yields((e) => {

			let result = {
				Nutrients: 0,
				Minerals: 0,
				Energy: 0,
			};

			// TODO: fix increments of properties in GSE

			if (e.tile.has_fungus) {
				if (e.player.faction.is_progenitor) { // tmp
					//result.Energy += 2;
					result.Energy = result.Energy + 2;
				}
			} else {
				if (e.tile.is_land) {
					//result.Nutrients += (e.tile.moisture - 1);
					result.Nutrients = result.Nutrients + (e.tile.moisture - 1);
					if (e.tile.rockiness > 1) {
						//result.Minerals++;
						result.Minerals = result.Minerals + 1;
					}
					if (e.tile.has_river) {
						result.Energy = result.Energy + 1;
					}
				} else {
					//result.Nutrients++;
					result.Nutrients = result.Energy + 1;
					if (e.player.faction.id == 'PIRATES') { // tmp
						//result.Minerals++;
						result.Minerals = result.Minerals + 1;
					}
				}
			}
			return result;
		});
	},

};

return result;
