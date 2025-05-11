return {

	validate: (e) => {
		if (e.game.is_turn_complete(e.caller)) {
			return 'Turn already completed by this player';
		}
	},

	apply: (e) => {
		e.game.complete_turn(e.caller);
		if (e.game.is_master) {
			let everybody_completed_turn = true;
			for (player of e.game.get_players()) {
				if (!e.game.is_turn_complete(player.id)) {
					everybody_completed_turn = false;
					break;
				}
			}
			if (everybody_completed_turn) {
				e.game.event('advance_turn', {
					turn_id: e.game.get_turn_id() + 1,
				});
			}
		}

	},

	rollback: (e) => {
		e.game.uncomplete_turn(e.caller);
	},

};
