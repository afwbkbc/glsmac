return {

	validate: (e) => {
		if (!e.game.is_turn_complete(e.caller)) {
			return 'Turn not completed by this player';
		}
	},

	apply: (e) => {
		e.game.uncomplete_turn(e.caller);
	},

	rollback: (e) => {
		e.game.complete_turn(e.caller);
	},

};
