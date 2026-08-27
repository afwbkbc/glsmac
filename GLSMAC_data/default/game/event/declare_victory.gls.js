const victory_rules = #include('../victory_rules');

return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only the game master can declare victory';
		}
		if (e.game.is_game_over()) {
			return 'Game already has a winner';
		}
		if (
			#typeof(e.data.type) != 'String' ||
			(
				e.data.type != 'conquest' && e.data.type != 'transcendence' &&
				e.data.type != 'economic'
			)
		) {
			return 'Unsupported victory type';
		}
		if (#typeof(e.data.winner_id) != 'Int' || e.data.winner_id < 0) {
			return 'Victory winner ID is invalid';
		}
		let winner = null;
		if (e.data.type == 'conquest') {
			winner = e.game.get_conquest_winner();
		} else if (e.data.type == 'transcendence') {
			winner = victory_rules.get_transcendence_winner(e.game);
		} else {
			winner = victory_rules.get_economic_winner(e.game);
		}
		if (winner == null || winner.id != e.data.winner_id) {
			return 'Player has not met the ' + e.data.type + ' victory condition';
		}
	},

	apply: (e) => {
		e.game.declare_victory(e.data.type, e.data.winner_id);
		const winner = e.game.get_player(e.data.winner_id);
		let result = ' has cornered the Global Energy Market';
		if (e.data.type == 'conquest') {
			result = ' has won by conquest';
		} else if (e.data.type == 'transcendence') {
			result = ' has achieved transcendence';
		}
		e.game.message(
			winner.get_faction().name + result + ' in M.Y. ' +
			#to_string(e.game.get_year()) + '.'
		);
	},

	rollback: (e) => {
		// Victory declarations are host-authored and never applied speculatively.
	},

};
