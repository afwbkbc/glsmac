const rules = #include('council_rules');
const council_ai = #include('ai/council');

return (game) => {
	game.on('start', (e) => {
		let resolution_pending = false;

		const process_session = () => {
			if (!game.is_master() || game.is_game_over()) { return; }
			const session = rules.get_session(game);
			if (session == null) {
				resolution_pending = false;
				return;
			}
			for (player of rules.get_voters(game)) {
				if (
					player.type == 'ai' &&
					player.get_council_state().vote_id == rules.vote_pending
				) {
					game.event_as(player.id, 'cast_council_vote', {
						player: player,
						vote_id: council_ai.choose_vote(game, player, session),
					});
				}
			}
			const tally = rules.get_tally(game);
			if (tally != null && tally.all_voted && !resolution_pending) {
				resolution_pending = true;
				game.event('resolve_planetary_council', {});
			}
		};

		const maybe_call_ai = () => {
			if (!game.is_master() || game.is_game_over() || rules.has_active_session(game)) {
				return;
			}
			const governor = rules.get_governor(game);
			for (player of rules.get_voters(game)) {
				if (
					player.type == 'ai' &&
					!#is_defined(rules.validate_call(game, player, 'supreme'))
				) {
					game.event_as(player.id, 'call_planetary_council', {
						player: player, proposal: 'supreme',
					});
					return;
				}
			}
			if (governor != null && governor.type == 'ai') {
				for (proposal of rules.get_available_policy_proposals(game, governor)) {
					if (
						council_ai.choose_policy_vote(game, governor, proposal) == rules.vote_yes
					) {
						game.event_as(governor.id, 'call_planetary_council', {
							player: governor, proposal: proposal,
						});
						return;
					}
				}
			}
			for (entry of rules.get_rankings(game)) {
				const player = entry.player;
				const should_challenge = governor == null || (
					governor.id != player.id &&
					player.get_diplomatic_relation(governor) == 'vendetta'
				);
				if (
					player.type == 'ai' && should_challenge &&
					!#is_defined(rules.validate_call(game, player, 'governor'))
				) {
					game.event_as(player.id, 'call_planetary_council', {
						player: player, proposal: 'governor',
					});
					return;
				}
			}
		};

		game.set('f_council_get_votes', (player) => { return rules.get_votes(game, player); });
		game.set('f_council_get_voters', () => { return rules.get_voters(game); });
		game.set('f_council_get_rankings', () => { return rules.get_rankings(game); });
		game.set('f_council_get_total_votes', () => { return rules.get_total_votes(game); });
		game.set('f_council_get_governor', () => { return rules.get_governor(game); });
		game.set('f_council_has_global_trade_pact', () => {
			return rules.has_global_trade_pact(game);
		});
		game.set('f_council_has_salvaged_unity_core', () => {
			return rules.has_salvaged_unity_core(game);
		});
		game.set('f_council_is_un_charter_repealed', () => {
			return rules.is_un_charter_repealed(game);
		});
		game.set('f_council_is_policy_proposal', (proposal) => {
			return rules.is_policy_proposal(proposal);
		});
		game.set('f_council_get_proposal_name', (proposal) => {
			return rules.get_proposal_name(proposal);
		});
		game.set('f_council_get_session', () => { return rules.get_session(game); });
		game.set('f_council_get_tally', () => { return rules.get_tally(game); });
		game.set('f_council_validate_call', (player, proposal) => {
			return rules.validate_call(game, player, proposal);
		});
		game.set('f_council_is_governor', (player) => {
			const governor = rules.get_governor(game);
			return governor != null && governor.id == player.id;
		});
		game.set('f_council_has_intelligence', (player, other) => {
			if (player.id == other.id) { return false; }
			if (player.has_infiltrated(other)) { return true; }
			const governor = rules.get_governor(game);
			return governor != null && governor.id == player.id && !other.get_faction().is_progenitor;
		});

		game.on('council_updated', process_session);
		game.on('turn', (e) => {
			process_session();
			maybe_call_ai();
		});
		process_session();
	});
};
