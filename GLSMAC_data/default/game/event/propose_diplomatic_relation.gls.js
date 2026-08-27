return {

	validate: (e) => {
		const error = e.game.get('f_diplomacy_validate_pair')(e.data.player, e.data.target);
		if (#is_defined(error)) {
			return error;
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only make their own diplomatic proposals';
		}
		if (e.game.is_turn_complete(e.data.player.id)) {
			return 'Player has already completed this turn';
		}
		if (e.data.relation != 'treaty' && e.data.relation != 'pact') {
			return 'Diplomatic proposal must be a treaty or pact';
		}
		const current = e.data.player.get_diplomatic_relation(e.data.target);
		if (current == e.data.relation) {
			return 'Players already have that diplomatic relation';
		}
		if (e.data.relation == 'pact' && current != 'treaty') {
			return 'A pact requires an existing treaty';
		}
		if (e.data.target.get_diplomatic_offer(e.data.player) != '') {
			return 'A diplomatic proposal is already pending';
		}
	},

	apply: (e) => {
		const previous = e.data.target.get_diplomatic_offer(e.data.player);
		e.data.target.set_diplomatic_offer(e.data.player, e.data.relation);
		e.game.trigger('diplomatic_proposal', {
			player: e.data.player,
			target: e.data.target,
			relation: e.data.relation,
		});
		return previous;
	},

	rollback: (e) => {
		e.data.target.set_diplomatic_offer(e.data.player, e.applied);
		e.game.trigger('diplomatic_proposal_updated', {
			player: e.data.player,
			target: e.data.target,
		});
	},

};
