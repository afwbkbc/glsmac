return {

	validate: (e) => {
		const error = e.game.get('f_diplomacy_validate_pair')(e.data.player, e.data.proposer);
		if (#is_defined(error)) {
			return error;
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only answer their own diplomatic proposals';
		}
		if (e.game.is_turn_complete(e.data.player.id)) {
			return 'Player has already completed this turn';
		}
		if (#typeof(e.data.accept) != 'Bool') {
			return 'Diplomatic response must accept or reject the proposal';
		}
		if (e.data.player.get_diplomatic_offer(e.data.proposer) == '') {
			return 'No diplomatic proposal is pending';
		}
	},

	apply: (e) => {
		const snapshot = e.game.get('f_diplomacy_snapshot_pair')(e.data.player, e.data.proposer);
		const relation = e.data.player.get_diplomatic_offer(e.data.proposer);
		e.data.player.set_diplomatic_offer(e.data.proposer, '');
		if (e.data.accept) {
			e.game.get('f_diplomacy_set_bilateral_relation')(
				e.data.player,
				e.data.proposer,
				relation
			);
			e.game.get('f_diplomacy_clear_relation_offers')(e.data.player, e.data.proposer);
		}
		e.game.trigger('diplomatic_proposal_resolved', {
			player: e.data.player,
			proposer: e.data.proposer,
			relation: relation,
			accepted: e.data.accept,
		});
		if (e.data.accept) {
			e.game.trigger('diplomacy_updated', {
				player: e.data.player,
				target: e.data.proposer,
				relation: relation,
			});
		}
		return snapshot;
	},

	rollback: (e) => {
		e.game.get('f_diplomacy_restore_pair')(e.data.player, e.data.proposer, e.applied);
		e.game.trigger('diplomatic_proposal_updated', {
			player: e.data.proposer,
			target: e.data.player,
		});
	},

};
