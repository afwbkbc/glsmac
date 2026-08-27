const get_relative_strength = (own_power, other_power) => {
	const total = #max(1.0, own_power + other_power);
	return (other_power - own_power) / total;
};

const get_other_integrity_blemishes = (state) => {
	return #is_defined(state.other_integrity_blemishes)
		? state.other_integrity_blemishes
		: 0;
};

const get_acceptance_score = (state) => {
	const relative_strength = get_relative_strength(state.own_power, state.other_power);
	const base_pressure = #to_float(state.other_bases - state.own_bases) * 4.0;
	const integrity_blemishes = #to_float(get_other_integrity_blemishes(state));
	if (state.offer == 'treaty') {
		let score = 20.0 + relative_strength * 60.0 + base_pressure - integrity_blemishes * 8.0;
		if (state.relation == 'vendetta') {
			score -= 25.0;
		}
		if (state.relation == 'pact') {
			score -= 100.0;
		}
		return score;
	}
	if (state.offer == 'pact') {
		if (state.relation != 'treaty') {
			return 0.0 - 100.0;
		}
		return 5.0 + relative_strength * 35.0 + base_pressure - integrity_blemishes * 12.0;
	}
	return 0.0 - 100.0;
};

const should_accept = (state) => {
	return get_acceptance_score(state) >= 0.0;
};

const get_proposal = (state) => {
	if (state.relation == 'vendetta') {
		const treaty_state = {
			offer: 'treaty',
			relation: state.relation,
			own_power: state.own_power,
			other_power: state.other_power,
			own_bases: state.own_bases,
			other_bases: state.other_bases,
			other_integrity_blemishes: get_other_integrity_blemishes(state),
		};
		const score = get_acceptance_score(treaty_state);
		return score >= 0.0 ? {relation: 'treaty', score: score} : null;
	}
	if (state.relation == 'treaty') {
		const pact_state = {
			offer: 'pact',
			relation: state.relation,
			own_power: state.own_power,
			other_power: state.other_power,
			own_bases: state.own_bases,
			other_bases: state.other_bases,
			other_integrity_blemishes: get_other_integrity_blemishes(state),
		};
		const score = get_acceptance_score(pact_state);
		return score >= 10.0 ? {relation: 'pact', score: score} : null;
	}
	return null;
};

const get_shared_technology_multiplier = (relation, relative_strength) => {
	let multiplier = 2.0;
	if (relation == 'treaty') {
		multiplier = 1.6;
	} else if (relation == 'pact') {
		multiplier = 1.3;
	}
	return multiplier + #max(0.0, relative_strength) * 0.8;
};

const get_trade_acceptance_score = (state) => {
	if (state.relation == 'vendetta') {
		return 0.0 - 100000.0;
	}
	const relative_strength = get_relative_strength(state.own_power, state.other_power);
	const sharing_multiplier = get_shared_technology_multiplier(
		state.relation,
		relative_strength
	);
	let received = #to_float(state.terms.offer_energy);
	let given = #to_float(state.terms.request_energy);
	if (state.terms.offer_technology != '') {
		received += #to_float(state.offer_technology_cost) * 2.5;
	}
	if (state.terms.request_technology != '') {
		given += #to_float(state.request_technology_cost) * sharing_multiplier;
	}
	return received - given - 5.0;
};

const reverse_terms = (terms) => {
	return {
		offer_energy: terms.request_energy,
		offer_technology: terms.request_technology,
		request_energy: terms.offer_energy,
		request_technology: terms.offer_technology,
	};
};

const score_trade_proposal = (state, terms, offer_cost, request_cost) => {
	const recipient_score = get_trade_acceptance_score({
		relation: state.relation,
		own_power: state.other_power,
		other_power: state.own_power,
		terms: terms,
		offer_technology_cost: offer_cost,
		request_technology_cost: request_cost,
	});
	if (recipient_score < 0.0) {
		return null;
	}
	const proposer_score = get_trade_acceptance_score({
		relation: state.relation,
		own_power: state.own_power,
		other_power: state.other_power,
		terms: reverse_terms(terms),
		offer_technology_cost: request_cost,
		request_technology_cost: offer_cost,
	});
	if (proposer_score < 0.0) {
		return null;
	}
	return proposer_score + recipient_score * 0.25;
};

const get_trade_proposal = (state) => {
	if (state.relation == 'vendetta') {
		return null;
	}
	let best = null;
	const consider = (terms, offer_cost, request_cost) => {
		const score = score_trade_proposal(state, terms, offer_cost, request_cost);
		if (score != null && (best == null || score > best.score)) {
			best = {terms: terms, score: score};
		}
	};

	for (own_technology of state.own_technologies) {
		for (other_technology of state.other_technologies) {
			consider({
				offer_energy: 0,
				offer_technology: own_technology.id,
				request_energy: 0,
				request_technology: other_technology.id,
			}, own_technology.cost, other_technology.cost);
		}
	}

	for (other_technology of state.other_technologies) {
		const price = other_technology.cost * 2;
		if (price <= state.own_energy && state.own_energy - price >= 25) {
			consider({
				offer_energy: price,
				offer_technology: '',
				request_energy: 0,
				request_technology: other_technology.id,
			}, 0, other_technology.cost);
		}
	}

	for (own_technology of state.own_technologies) {
		const price = own_technology.cost * 2;
		if (price <= state.other_energy && state.other_energy - price >= 25) {
			consider({
				offer_energy: 0,
				offer_technology: own_technology.id,
				request_energy: price,
				request_technology: '',
			}, own_technology.cost, 0);
		}
	}

	return best;
};

const get_loan_acceptance_score = (state) => {
	if (state.relation == 'vendetta') {
		return 0.0 - 100000.0;
	}
	const principal = #to_float(state.terms.principal);
	const total_repayment = #to_float(state.terms.payment * state.terms.turns);
	const discounted_repayment = total_repayment /
		(1.0 + #to_float(state.terms.turns) * 0.004);
	let relation_bonus = 0.0 - 10.0;
	if (state.relation == 'treaty') {
		relation_bonus = 5.0;
	} else if (state.relation == 'pact') {
		relation_bonus = 12.0;
	}
	const relative_strength = get_relative_strength(state.own_power, state.other_power);
	if (state.own_is_lender) {
		if (state.own_energy - state.terms.principal < 50) {
			return 0.0 - 100000.0;
		}
		const default_risk = #max(0.0, relative_strength) * principal * 0.15;
		const integrity_risk =
			#to_float(get_other_integrity_blemishes(state)) * principal * 0.06;
		return discounted_repayment - principal + relation_bonus - default_risk - integrity_risk;
	}
	const liquidity_value = #to_float(#max(0, 100 - state.own_energy)) * 0.4;
	const payment_pressure = #to_float(#max(
		0,
		state.terms.payment - #max(2, (state.own_energy + state.terms.principal) / 10)
	)) * 2.0;
	return principal - discounted_repayment + liquidity_value + relation_bonus - payment_pressure;
};

const get_loan_proposal = (state) => {
	if (state.relation != 'treaty' && state.relation != 'pact') {
		return null;
	}
	let proposer_is_lender = false;
	let lender_energy = state.other_energy;
	let borrower_energy = state.own_energy;
	if (state.own_energy >= 250 && state.other_energy <= 80) {
		proposer_is_lender = true;
		lender_energy = state.own_energy;
		borrower_energy = state.other_energy;
	} else if (!(state.own_energy <= 80 && state.other_energy >= 250)) {
		return null;
	}
	const available = lender_energy - 100;
	const need = #max(40, 120 - borrower_energy);
	const principal = #floor(#to_float(#min(100, #min(available, need))) / 10.0) * 10;
	if (principal < 20) {
		return null;
	}
	const turns = 20;
	const payment = #ceil(#to_float(principal) * 1.25 / #to_float(turns));
	const terms = {
		proposer_is_lender: proposer_is_lender,
		principal: principal,
		payment: payment,
		turns: turns,
	};
	const proposer_score = get_loan_acceptance_score({
		relation: state.relation,
		own_power: state.own_power,
		other_power: state.other_power,
		own_energy: state.own_energy,
		own_is_lender: proposer_is_lender,
		other_integrity_blemishes: #is_defined(state.other_integrity_blemishes)
			? state.other_integrity_blemishes
			: 0,
		terms: terms,
	});
	const recipient_score = get_loan_acceptance_score({
		relation: state.relation,
		own_power: state.other_power,
		other_power: state.own_power,
		own_energy: state.other_energy,
		own_is_lender: !proposer_is_lender,
		other_integrity_blemishes: #is_defined(state.own_integrity_blemishes)
			? state.own_integrity_blemishes
			: 0,
		terms: terms,
	});
	if (proposer_score < 0.0 || recipient_score < 0.0) {
		return null;
	}
	return {terms: terms, score: proposer_score + recipient_score * 0.25};
};

return {
	get_acceptance_score: get_acceptance_score,
	should_accept: should_accept,
	get_proposal: get_proposal,
	get_trade_acceptance_score: get_trade_acceptance_score,
	get_trade_proposal: get_trade_proposal,
	get_loan_acceptance_score: get_loan_acceptance_score,
	get_loan_proposal: get_loan_proposal,
};
