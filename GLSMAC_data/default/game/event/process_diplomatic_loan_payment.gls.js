const MAX_ENERGY_CREDITS = 1000000000;

return {

	validate: (e) => {
		const error = e.game.get('f_diplomacy_validate_pair')(e.data.borrower, e.data.lender);
		if (#is_defined(error)) {
			return error;
		}
		if (e.caller != 0) {
			return 'Only master is allowed to process diplomatic loan payments';
		}
		if (e.data.borrower.get_diplomatic_loan(e.data.lender) == null) {
			return 'No diplomatic loan payment is due';
		}
	},

	apply: (e) => {
		const borrower = e.data.borrower;
		const lender = e.data.lender;
		const loan = borrower.get_diplomatic_loan(lender);
		const snapshot = {
			loan: loan,
			borrower_energy: borrower.get_energy_credits(),
			lender_energy: lender.get_energy_credits(),
		};
		const at_war = (
			borrower.get_diplomatic_relation(lender) == 'vendetta' ||
			lender.get_diplomatic_relation(borrower) == 'vendetta'
		);
		const sanctioned = (
			borrower.get_sanction_turns() > 0 || lender.get_sanction_turns() > 0
		);
		let paid = 0;
		let penalty = 0;
		let balance = loan.balance;
		if (at_war) {
			penalty = #min(loan.payment, MAX_ENERGY_CREDITS - balance);
			balance += penalty;
		} else if (!sanctioned) {
			const scheduled = #min(loan.payment, balance);
			const lender_capacity = MAX_ENERGY_CREDITS - lender.get_energy_credits();
			paid = #min(scheduled, #min(borrower.get_energy_credits(), lender_capacity));
			balance -= paid;
			if (paid > 0) {
				borrower.set_energy_credits(borrower.get_energy_credits() - paid);
				lender.set_energy_credits(lender.get_energy_credits() + paid);
			}
		}
		if (balance == 0) {
			borrower.clear_diplomatic_loan(lender);
			e.game.message(borrower.name + ' completed its loan payments to ' + lender.name + '.');
		} else {
			borrower.set_diplomatic_loan(lender, {
				balance: balance,
				payment: loan.payment,
			});
		}
		if (paid > 0) {
			e.game.trigger('economy_updated', {player: borrower});
			e.game.trigger('economy_updated', {player: lender});
		}
		e.game.trigger('diplomatic_loan_updated', {
			borrower: borrower,
			lender: lender,
			balance: balance,
			paid: paid,
			penalty: penalty,
			suspended: sanctioned && !at_war,
		});
		return snapshot;
	},

	rollback: (e) => {
		const borrower = e.data.borrower;
		const lender = e.data.lender;
		borrower.set_energy_credits(e.applied.borrower_energy);
		lender.set_energy_credits(e.applied.lender_energy);
		borrower.set_diplomatic_loan(lender, e.applied.loan);
		e.game.trigger('economy_updated', {player: borrower});
		e.game.trigger('economy_updated', {player: lender});
		e.game.trigger('diplomatic_loan_updated', {
			borrower: borrower,
			lender: lender,
			balance: e.applied.loan.balance,
			paid: 0,
			penalty: 0,
		});
	},

};
