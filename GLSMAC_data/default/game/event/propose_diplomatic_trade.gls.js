return {

	validate: (e) => {
		const error = e.game.get('f_diplomacy_validate_pair')(e.data.player, e.data.target);
		if (#is_defined(error)) {
			return error;
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only make their own diplomatic trades';
		}
		if (e.game.is_turn_complete(e.data.player.id)) {
			return 'Player has already completed this turn';
		}
		const trade_error = e.game.get('f_diplomacy_validate_trade')(
			e.data.player,
			e.data.target,
			e.data.terms
		);
		if (#is_defined(trade_error)) {
			return trade_error;
		}
		if (e.data.target.get_diplomatic_trade(e.data.player) != null) {
			return 'A diplomatic trade is already pending';
		}
	},

	apply: (e) => {
		const previous = e.data.target.get_diplomatic_trade(e.data.player);
		e.data.target.set_diplomatic_trade(e.data.player, e.data.terms);
		e.game.trigger('diplomatic_trade_proposed', {
			player: e.data.player,
			target: e.data.target,
			terms: e.data.terms,
		});
		return previous;
	},

	rollback: (e) => {
		if (e.applied == null) {
			e.data.target.clear_diplomatic_trade(e.data.player);
		} else {
			e.data.target.set_diplomatic_trade(e.data.player, e.applied);
		}
		e.game.trigger('diplomatic_trade_updated', {
			player: e.data.player,
			target: e.data.target,
		});
	},

};
