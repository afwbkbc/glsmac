const result = {

	configure: (game) => {

		game.um.on('unit_turn', (e) => {
			const def = e.unit.get_def();
			if (!e.unit.moved_this_turn) {
				if (e.unit.health < def.health_max) {
					e.unit.health = #min(e.unit.health + def.health_per_turn, def.health_max);
				}
			}
			if (!def.is_immovable) {
				e.unit.movement = def.movement_per_turn;
			}
		});

	},
};

return result;
