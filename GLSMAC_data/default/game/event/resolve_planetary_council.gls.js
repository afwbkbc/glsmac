const rules = #include('../council_rules');

const clear_session = (game, governor_id, policy_state) => {
	for (player of game.get_players()) {
		const old = player.get_council_state();
		player.set_council_state({
			is_governor: governor_id < 0 ? old.is_governor : player.id == governor_id,
			last_session_turn: old.last_session_turn,
			proposal: '',
			caller_id: -1,
			candidate_a_id: -1,
			candidate_b_id: -1,
			vote_id: rules.vote_pending,
			global_trade_pact: #is_defined(policy_state)
				? policy_state.global_trade_pact
				: old.global_trade_pact,
			unity_core_salvaged: #is_defined(policy_state)
				? policy_state.unity_core_salvaged
				: old.unity_core_salvaged,
			un_charter_repealed: #is_defined(policy_state)
				? policy_state.un_charter_repealed
				: old.un_charter_repealed,
		});
	}
};

const get_policy_pass_message = (proposal) => {
	if (proposal == 'trade_pact') {
		return 'The Global Trade Pact has passed. Commerce rates are now doubled';
	}
	if (proposal == 'repeal_trade_pact') {
		return 'The Global Trade Pact has been repealed. Commerce rates have returned to normal';
	}
	if (proposal == 'salvage_unity_core') {
		return 'The Unity Fusion Core has been salvaged. Every faction receives 500 energy credits';
	}
	if (proposal == 'repeal_un_charter') {
		return 'The U.N. Charter has been repealed. Future atrocities no longer incur Council sanctions';
	}
	if (proposal == 'reinstate_un_charter') {
		return 'The U.N. Charter has been reinstated. Council atrocity sanctions are active again';
	}
	if (proposal == 'launch_solar_shade') {
		return 'A Solar Shade has been launched. Planetary sea levels will gradually fall';
	}
	if (proposal == 'melt_polar_caps') {
		return 'The polar caps will be melted. Planetary sea levels will gradually rise';
	}
	return 'The U.N. Charter has been reinstated. Council atrocity sanctions are active again';
};

return {
	validate: (e) => {
		if (e.caller != 0) {
			return 'Only the game master can resolve the Planetary Council';
		}
		if (e.game.is_game_over()) {
			return 'Game already has a winner';
		}
		const tally = rules.get_tally(e.game);
		if (tally == null) {
			return rules.has_active_session(e.game)
				? 'Planetary Council session state is inconsistent'
				: 'No Planetary Council vote is active';
		}
		if (!tally.all_voted) {
			return 'Planetary Council is still waiting for votes';
		}
	},

	apply: (e) => {
		const previous = rules.snapshot_states(e.game);
		const previous_energy = rules.snapshot_energy(e.game);
		let previous_climate = null;
		const result = rules.get_result(e.game);
		if (result.proposal == 'governor') {
			clear_session(e.game, result.winner_id, #undefined);
			if (result.winner_id >= 0) {
				const winner = e.game.get_player(result.winner_id);
				e.game.message(
					winner.get_faction().name + ' has been elected Planetary Governor with ' +
					#to_string(result.winner_votes) + ' of ' +
					#to_string(result.total_votes) + ' votes.'
				);
			} else {
				e.game.message('The Planetary Governor proposal failed to win a majority.');
			}
			e.game.trigger('economy_updated', {});
		} else if (rules.is_policy_proposal(result.proposal)) {
			const passed = result.winner_id == rules.vote_yes;
			const updated = #clone(rules.get_policy_state(e.game));
			if (passed) {
				if (result.proposal == 'trade_pact') {
					updated.global_trade_pact = true;
				} else if (result.proposal == 'repeal_trade_pact') {
					updated.global_trade_pact = false;
				} else if (result.proposal == 'salvage_unity_core') {
					updated.unity_core_salvaged = true;
					rules.award_unity_core_energy(e.game);
				} else if (result.proposal == 'repeal_un_charter') {
					updated.un_charter_repealed = true;
				} else if (result.proposal == 'reinstate_un_charter') {
					updated.un_charter_repealed = false;
				} else if (result.proposal == 'launch_solar_shade') {
					previous_climate = rules.queue_climate_change(
						e.game, 0 - rules.council_sea_change
					);
				} else if (result.proposal == 'melt_polar_caps') {
					previous_climate = rules.queue_climate_change(
						e.game, rules.council_sea_change
					);
				}
			}
			clear_session(e.game, (-1), updated);
			if (passed) {
				e.game.message(
					get_policy_pass_message(result.proposal) +
					' with ' + #to_string(result.winner_votes) + ' of ' +
					#to_string(result.total_votes) + ' votes.'
				);
			} else {
				e.game.message(
					rules.get_proposal_name(result.proposal) + ' failed to win a majority.'
				);
			}
			e.game.trigger('economy_updated', {});
		} else {
			clear_session(e.game, (-1), #undefined);
			if (result.winner_id >= 0) {
				const winner = e.game.get_player(result.winner_id);
				e.game.declare_victory('diplomatic', result.winner_id);
				e.game.message(
					winner.get_faction().name + ' has been elected Supreme Leader of Planet in M.Y. ' +
					#to_string(e.game.get_year()) + '.'
				);
			} else {
				e.game.message('The Supreme Leader proposal failed to win a three-quarters majority.');
			}
		}
		e.game.trigger('council_updated', {proposal: ''});
		return {
			states: previous,
			energy: previous_energy,
			climate: previous_climate,
			terminal: result.proposal == 'supreme' && result.winner_id >= 0,
		};
	},

	rollback: (e) => {
		if (!e.applied.terminal) {
			rules.restore_states(e.applied.states);
			rules.restore_energy(e.applied.energy);
			if (e.applied.climate != null) {
				rules.restore_climate_state(e.game, e.applied.climate);
			}
			e.game.trigger('council_updated', {});
		}
		// Successful Supreme Leader resolutions are host-authored terminal events.
	},
};
