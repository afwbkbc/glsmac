const rules = #include('../council_rules');

return {
	validate: (e) => {
		if (
			#typeof(e.data.player) != 'Object' ||
			#typeof(e.data.player.get_council_state) != 'Callable' ||
			#typeof(e.data.player.set_council_state) != 'Callable'
		) {
			return 'Planetary Council call requires a player';
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only convene the Planetary Council on their own behalf';
		}
		if (e.game.is_turn_complete(e.data.player.id)) {
			return 'Player has already completed this turn';
		}
		return rules.validate_call(e.game, e.data.player, e.data.proposal);
	},

	apply: (e) => {
		const previous = rules.snapshot_states(e.game);
		const rankings = rules.get_rankings(e.game);
		const voters = rules.get_voters(e.game);
		const is_policy = rules.is_policy_proposal(e.data.proposal);
		for (player of e.game.get_players()) {
			let can_vote = false;
			for (voter of voters) {
				if (voter.id == player.id) { can_vote = true; break; }
			}
			const old = player.get_council_state();
			player.set_council_state({
				is_governor: old.is_governor,
				last_session_turn: e.game.get_turn(),
				proposal: e.data.proposal,
				caller_id: e.data.player.id,
				candidate_a_id: is_policy ? rules.vote_yes : rankings[0].player.id,
				candidate_b_id: is_policy ? rules.vote_no : rankings[1].player.id,
				vote_id: can_vote ? rules.vote_pending : rules.vote_abstain,
				global_trade_pact: old.global_trade_pact,
				unity_core_salvaged: old.unity_core_salvaged,
				un_charter_repealed: old.un_charter_repealed,
			});
		}
		e.game.trigger('council_updated', {proposal: e.data.proposal});
		if (is_policy) {
			e.game.message(
				e.data.player.get_faction().name + ' has convened the Planetary Council: ' +
				rules.get_proposal_name(e.data.proposal) + '.'
			);
		} else {
			e.game.message(
				e.data.player.get_faction().name + ' has convened the Planetary Council to elect ' +
				(e.data.proposal == 'supreme' ? 'a Supreme Leader.' : 'a Planetary Governor.')
			);
		}
		return previous;
	},

	rollback: (e) => {
		rules.restore_states(e.applied);
		e.game.trigger('council_updated', {proposal: ''});
	},
};
