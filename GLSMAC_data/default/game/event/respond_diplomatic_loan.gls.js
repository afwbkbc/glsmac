return {

	validate: (e) => {
		const error = e.game.get('f_diplomacy_validate_pair')(e.data.player, e.data.proposer);
		if (#is_defined(error)) {
			return error;
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only answer their own diplomatic loan proposals';
		}
		if (e.game.is_turn_complete(e.data.player.id)) {
			return 'Player has already completed this turn';
		}
		if (#typeof(e.data.accept) != 'Bool') {
			return 'Diplomatic loan response must accept or reject the proposal';
		}
		const terms = e.data.player.get_diplomatic_loan_offer(e.data.proposer);
		if (terms == null) {
			return 'No diplomatic loan proposal is pending';
		}
		if (e.data.accept) {
			return e.game.get('f_diplomacy_validate_loan_offer')(
				e.data.proposer,
				e.data.player,
				terms
			);
		}
	},

	apply: (e) => {
		const player = e.data.player;
		const proposer = e.data.proposer;
		const terms = player.get_diplomatic_loan_offer(proposer);
		const parties = e.game.get('f_diplomacy_get_loan_parties')(proposer, player, terms);
		const snapshot = {
			terms: terms,
			lender_energy: parties.lender.get_energy_credits(),
			borrower_energy: parties.borrower.get_energy_credits(),
			loan: parties.borrower.get_diplomatic_loan(parties.lender),
		};
		player.clear_diplomatic_loan_offer(proposer);
		if (e.data.accept) {
			parties.lender.set_energy_credits(
				parties.lender.get_energy_credits() - terms.principal
			);
			parties.borrower.set_energy_credits(
				parties.borrower.get_energy_credits() + terms.principal
			);
			parties.borrower.set_diplomatic_loan(parties.lender, {
				balance: terms.payment * terms.turns,
				payment: terms.payment,
			});
			e.game.trigger('economy_updated', {player: parties.lender});
			e.game.trigger('economy_updated', {player: parties.borrower});
			e.game.message(
				parties.lender.name + ' loaned ' + #to_string(terms.principal) +
				' energy credits to ' + parties.borrower.name + '.'
			);
		}
		e.game.trigger('diplomatic_loan_resolved', {
			player: player,
			proposer: proposer,
			lender: parties.lender,
			borrower: parties.borrower,
			terms: terms,
			accepted: e.data.accept,
		});
		return snapshot;
	},

	rollback: (e) => {
		const player = e.data.player;
		const proposer = e.data.proposer;
		const parties = e.game.get('f_diplomacy_get_loan_parties')(
			proposer,
			player,
			e.applied.terms
		);
		parties.lender.set_energy_credits(e.applied.lender_energy);
		parties.borrower.set_energy_credits(e.applied.borrower_energy);
		if (e.applied.loan == null) {
			parties.borrower.clear_diplomatic_loan(parties.lender);
		} else {
			parties.borrower.set_diplomatic_loan(parties.lender, e.applied.loan);
		}
		player.set_diplomatic_loan_offer(proposer, e.applied.terms);
		e.game.trigger('economy_updated', {player: parties.lender});
		e.game.trigger('economy_updated', {player: parties.borrower});
		e.game.trigger('diplomatic_loan_updated', {
			player: proposer,
			target: player,
		});
	},

};
