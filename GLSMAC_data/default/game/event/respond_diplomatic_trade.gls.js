return {

	validate: (e) => {
		const error = e.game.get('f_diplomacy_validate_pair')(e.data.player, e.data.proposer);
		if (#is_defined(error)) {
			return error;
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only answer their own diplomatic trades';
		}
		if (e.game.is_turn_complete(e.data.player.id)) {
			return 'Player has already completed this turn';
		}
		if (#typeof(e.data.accept) != 'Bool') {
			return 'Diplomatic trade response must accept or reject the proposal';
		}
		const terms = e.data.player.get_diplomatic_trade(e.data.proposer);
		if (terms == null) {
			return 'No diplomatic trade is pending';
		}
		if (e.data.accept) {
			return e.game.get('f_diplomacy_validate_trade')(
				e.data.proposer,
				e.data.player,
				terms
			);
		}
	},

	apply: (e) => {
		const player = e.data.player;
		const proposer = e.data.proposer;
		const terms = player.get_diplomatic_trade(proposer);
		const snapshot = {
			terms: terms,
			player_energy: player.get_energy_credits(),
			proposer_energy: proposer.get_energy_credits(),
			player_research: player.get_research_state(),
			proposer_research: proposer.get_research_state(),
		};
		player.clear_diplomatic_trade(proposer);
		if (e.data.accept) {
			proposer.set_energy_credits(
				proposer.get_energy_credits() - terms.offer_energy + terms.request_energy
			);
			player.set_energy_credits(
				player.get_energy_credits() - terms.request_energy + terms.offer_energy
			);
			e.game.get('f_diplomacy_grant_technology')(player, terms.offer_technology);
			e.game.get('f_diplomacy_grant_technology')(proposer, terms.request_technology);
			if (terms.offer_energy > 0 || terms.request_energy > 0) {
				e.game.trigger('economy_updated', {player: player});
				e.game.trigger('economy_updated', {player: proposer});
			}
			if (terms.offer_technology != '') {
				e.game.trigger('research_updated', {player: player});
			}
			if (terms.request_technology != '') {
				e.game.trigger('research_updated', {player: proposer});
			}
			e.game.message(proposer.name + ' and ' + player.name + ' completed a diplomatic trade.');
		}
		e.game.trigger('diplomatic_trade_resolved', {
			player: player,
			proposer: proposer,
			terms: terms,
			accepted: e.data.accept,
		});
		return snapshot;
	},

	rollback: (e) => {
		const player = e.data.player;
		const proposer = e.data.proposer;
		player.set_energy_credits(e.applied.player_energy);
		proposer.set_energy_credits(e.applied.proposer_energy);
		player.set_research_state(e.applied.player_research);
		proposer.set_research_state(e.applied.proposer_research);
		player.set_diplomatic_trade(proposer, e.applied.terms);
		e.game.trigger('economy_updated', {player: player});
		e.game.trigger('economy_updated', {player: proposer});
		e.game.trigger('research_updated', {player: player});
		e.game.trigger('research_updated', {player: proposer});
		e.game.trigger('diplomatic_trade_updated', {
			player: proposer,
			target: player,
		});
	},

};
