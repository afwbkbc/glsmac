return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only host can advance turn';
		}
		if (#typeof(e.data.turn_id) != 'Int' || e.data.turn_id <= 0) {
			return 'Turn ID must be a positive whole number';
		}
		const current_turn = e.game.get_turn();
		if (
			(e.data.turn_id == 1 && current_turn > 1) ||
			(e.data.turn_id != 1 && e.data.turn_id != current_turn + 1)
		) {
			return 'Turn ID must advance sequentially';
		}
		if (e.data.turn_id != 1) {
			if (#typeof(e.game.get_native_player) == 'Callable') {
				const native = e.game.get_native_player();
				if (native != null && !e.game.is_turn_complete(native.id)) {
					return 'Can\'t advance turn because native life did not complete it';
				}
			}
			for (player of e.game.get_players()) {
				if (!e.game.is_turn_complete(player.id)) {
					return 'Can\'t advance turn because player ' + #to_string(player.id) + ' did not complete it';
				}
			}
		}
	},

	apply: (e) => {
		e.game.advance_turn(e.data.turn_id);
	},

	rollback: (e) => {
		// Advance-turn events are host-authored and never applied speculatively.
	},

};
