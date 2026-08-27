const is_player = (player) => {
	return (
		#typeof(player) == 'Object' &&
		#typeof(player.get_diplomatic_relation) == 'Callable' &&
		#typeof(player.get_energy_credits) == 'Callable' &&
		#typeof(player.get_sanction_turns) == 'Callable' &&
		#typeof(player.set_sanction_turns) == 'Callable' &&
		#typeof(player.get_integrity_blemishes) == 'Callable' &&
		#typeof(player.set_integrity_blemishes) == 'Callable' &&
		#typeof(player.set_diplomatic_relation) == 'Callable' &&
		#typeof(player.get_diplomatic_offer) == 'Callable' &&
		#typeof(player.set_diplomatic_offer) == 'Callable' &&
		#typeof(player.get_diplomatic_trade) == 'Callable' &&
		#typeof(player.set_diplomatic_trade) == 'Callable' &&
		#typeof(player.clear_diplomatic_trade) == 'Callable' &&
		#typeof(player.get_diplomatic_loan_offer) == 'Callable' &&
		#typeof(player.set_diplomatic_loan_offer) == 'Callable' &&
		#typeof(player.clear_diplomatic_loan_offer) == 'Callable' &&
		#typeof(player.get_diplomatic_loan) == 'Callable' &&
		#typeof(player.set_diplomatic_loan) == 'Callable' &&
		#typeof(player.clear_diplomatic_loan) == 'Callable'
	);
};

const validate_pair = (player, other) => {
	if (!is_player(player) || !is_player(other)) {
		return 'Diplomacy requires two players';
	}
	if (player.id == other.id) {
		return 'A player cannot conduct diplomacy with itself';
	}
};

const snapshot_pair = (player, other) => {
	return {
		player_relation: player.get_diplomatic_relation(other),
		other_relation: other.get_diplomatic_relation(player),
		player_offer: player.get_diplomatic_offer(other),
		other_offer: other.get_diplomatic_offer(player),
		player_trade: player.get_diplomatic_trade(other),
		other_trade: other.get_diplomatic_trade(player),
		player_loan_offer: player.get_diplomatic_loan_offer(other),
		other_loan_offer: other.get_diplomatic_loan_offer(player),
		player_integrity_blemishes: player.get_integrity_blemishes(),
		other_integrity_blemishes: other.get_integrity_blemishes(),
	};
};

const restore_trade = (recipient, proposer, trade) => {
	if (trade == null) {
		recipient.clear_diplomatic_trade(proposer);
	} else {
		recipient.set_diplomatic_trade(proposer, trade);
	}
};

const restore_loan_offer = (recipient, proposer, offer) => {
	if (offer == null) {
		recipient.clear_diplomatic_loan_offer(proposer);
	} else {
		recipient.set_diplomatic_loan_offer(proposer, offer);
	}
};

const restore_pair = (player, other, snapshot) => {
	player.set_diplomatic_relation(other, snapshot.player_relation);
	other.set_diplomatic_relation(player, snapshot.other_relation);
	player.set_diplomatic_offer(other, snapshot.player_offer);
	other.set_diplomatic_offer(player, snapshot.other_offer);
	restore_trade(player, other, snapshot.player_trade);
	restore_trade(other, player, snapshot.other_trade);
	restore_loan_offer(player, other, snapshot.player_loan_offer);
	restore_loan_offer(other, player, snapshot.other_loan_offer);
	player.set_integrity_blemishes(snapshot.player_integrity_blemishes);
	other.set_integrity_blemishes(snapshot.other_integrity_blemishes);
};

const integrity_names = [
	'Noble', 'Faithful', 'Scrupulous', 'Dependable',
	'Ruthless', 'Treacherous', 'Wicked', 'Infamous',
];

const get_integrity_name = (blemishes) => {
	const index = #min(7, #max(0, blemishes));
	return integrity_names[index];
};

const get_betrayal_penalty = (relation) => {
	if (relation == 'pact') {
		return 2;
	}
	return relation == 'treaty' ? 1 : 0;
};

const record_betrayal = (game, player, other) => {
	const relation = player.get_diplomatic_relation(other);
	const penalty = get_betrayal_penalty(relation);
	if (penalty == 0) {
		return 0;
	}
	const updated = #min(7, player.get_integrity_blemishes() + penalty);
	player.set_integrity_blemishes(updated);
	game.trigger('diplomatic_integrity_updated', {
		player: player,
		target: other,
		blemishes: updated,
		integrity: get_integrity_name(updated),
	});
	game.message(
		player.name + ' broke a ' + relation + ' with ' + other.name +
		'; diplomatic integrity is now ' + get_integrity_name(updated) + '.'
	);
	return penalty;
};

const set_bilateral_relation = (game, player, other, relation) => {
	if (relation == 'vendetta') {
		record_betrayal(game, player, other);
	}
	player.set_diplomatic_relation(other, relation);
	other.set_diplomatic_relation(player, relation);
};

const clear_relation_offers = (player, other) => {
	player.set_diplomatic_offer(other, '');
	other.set_diplomatic_offer(player, '');
};

const clear_offers = (player, other) => {
	clear_relation_offers(player, other);
	player.clear_diplomatic_trade(other);
	other.clear_diplomatic_trade(player);
	player.clear_diplomatic_loan_offer(other);
	other.clear_diplomatic_loan_offer(player);
};

const validate_trade = (game, proposer, recipient, terms) => {
	if (#typeof(terms) != 'Object') {
		return 'Diplomatic trade terms must be an object';
	}
	if (
		#typeof(terms.offer_energy) != 'Int' ||
		#typeof(terms.offer_technology) != 'String' ||
		#typeof(terms.request_energy) != 'Int' ||
		#typeof(terms.request_technology) != 'String'
	) {
		return 'Diplomatic trade terms have invalid fields';
	}
	if (
		terms.offer_energy < 0 || terms.offer_energy > 1000000000 ||
		terms.request_energy < 0 || terms.request_energy > 1000000000
	) {
		return 'Diplomatic trade energy is out of range';
	}
	if (terms.offer_energy > 0 && terms.request_energy > 0) {
		return 'Diplomatic trade cannot send energy in both directions';
	}
	if (
		terms.offer_energy == 0 && terms.offer_technology == '' &&
		terms.request_energy == 0 && terms.request_technology == ''
	) {
		return 'Diplomatic trade cannot be empty';
	}
	if (
		terms.offer_technology != '' &&
		terms.offer_technology == terms.request_technology
	) {
		return 'Diplomatic trade cannot exchange a technology for itself';
	}
	if (proposer.get_diplomatic_relation(recipient) == 'vendetta') {
		return 'Regular trade is unavailable during a vendetta';
	}
	if (proposer.get_sanction_turns() > 0 || recipient.get_sanction_turns() > 0) {
		return 'Regular trade is suspended by economic sanctions';
	}
	const proposer_energy = proposer.get_energy_credits();
	const recipient_energy = recipient.get_energy_credits();
	if (proposer_energy < terms.offer_energy) {
		return 'Proposer cannot afford the offered energy';
	}
	if (recipient_energy < terms.request_energy) {
		return 'Recipient cannot afford the requested energy';
	}
	if (recipient_energy + terms.offer_energy > 1000000000) {
		return 'Recipient cannot hold the offered energy';
	}
	if (proposer_energy + terms.request_energy > 1000000000) {
		return 'Proposer cannot hold the requested energy';
	}
	for (technology of [
		[terms.offer_technology, proposer, recipient, 'offered'],
		[terms.request_technology, recipient, proposer, 'requested'],
	]) {
		const id = technology[0];
		if (id == '') {
			continue;
		}
		const definition = game.get('f_technology_get_definition')(id);
		if (definition == null) {
			return 'Diplomatic trade contains an unknown technology';
		}
		if (!technology[1].has_technology(id)) {
			return 'The ' + technology[3] + ' technology is not known by its sender';
		}
		if (technology[2].has_technology(id)) {
			return 'The ' + technology[3] + ' technology is already known by its recipient';
		}
	}
};

const grant_technology = (game, player, id) => {
	if (id == '') {
		return false;
	}
	const previous = player.get_research_state();
	let technologies = [];
	for (known_id of previous.technologies) {
		technologies :+known_id;
	}
	technologies :+id;
	let target = previous.target;
	let progress = previous.progress;
	if (target == id) {
		target = game.get('f_technology_get_next_target')(technologies, player);
		if (target == '') {
			progress = 0;
		}
	}
	player.set_research_state({
		technologies: technologies,
		target: target,
		progress: progress,
	});
	const queue_datalinks = game.get('f_project_queue_planetary_datalinks');
	if (#is_defined(queue_datalinks)) {
		queue_datalinks();
	}
	return true;
};

const get_loan_parties = (proposer, recipient, terms) => {
	return terms.proposer_is_lender
		? {lender: proposer, borrower: recipient}
		: {lender: recipient, borrower: proposer};
};

const validate_loan_offer = (proposer, recipient, terms) => {
	const pair_error = validate_pair(proposer, recipient);
	if (#is_defined(pair_error)) {
		return pair_error;
	}
	if (#typeof(terms) != 'Object') {
		return 'Diplomatic loan terms must be an object';
	}
	if (
		#typeof(terms.proposer_is_lender) != 'Bool' ||
		#typeof(terms.principal) != 'Int' ||
		#typeof(terms.payment) != 'Int' ||
		#typeof(terms.turns) != 'Int'
	) {
		return 'Diplomatic loan terms have invalid fields';
	}
	if (
		terms.principal <= 0 || terms.principal > 1000000000 ||
		terms.payment <= 0 || terms.payment > 1000000000 ||
		terms.turns <= 0 || terms.turns > 1000
	) {
		return 'Diplomatic loan terms are out of range';
	}
	const repayment = terms.payment * terms.turns;
	if (
		repayment > 1000000000 ||
		repayment < terms.principal ||
		repayment > terms.principal * 4
	) {
		return 'Diplomatic loan repayment is outside supported terms';
	}
	if (
		proposer.get_diplomatic_relation(recipient) == 'vendetta' ||
		recipient.get_diplomatic_relation(proposer) == 'vendetta'
	) {
		return 'Diplomatic loans are unavailable during a vendetta';
	}
	if (proposer.get_sanction_turns() > 0 || recipient.get_sanction_turns() > 0) {
		return 'Diplomatic loans are suspended by economic sanctions';
	}
	if (
		proposer.get_diplomatic_loan(recipient) != null ||
		recipient.get_diplomatic_loan(proposer) != null
	) {
		return 'Only one active loan is supported between two factions';
	}
	const parties = get_loan_parties(proposer, recipient, terms);
	if (parties.lender.get_energy_credits() < terms.principal) {
		return 'Lender cannot afford the loan principal';
	}
	if (parties.borrower.get_energy_credits() + terms.principal > 1000000000) {
		return 'Borrower cannot hold the loan principal';
	}
};

return (game) => {
	game.on('start', (e) => {
		game.set('f_diplomacy_validate_pair', validate_pair);
		game.set('f_diplomacy_snapshot_pair', snapshot_pair);
		game.set('f_diplomacy_restore_pair', restore_pair);
		game.set('f_diplomacy_get_integrity_name', get_integrity_name);
		game.set('f_diplomacy_get_betrayal_penalty', get_betrayal_penalty);
		game.set('f_diplomacy_set_bilateral_relation', (player, other, relation) => {
			return set_bilateral_relation(game, player, other, relation);
		});
		game.set('f_diplomacy_clear_relation_offers', clear_relation_offers);
		game.set('f_diplomacy_clear_offers', clear_offers);
		game.set('f_diplomacy_validate_trade', (proposer, recipient, terms) => {
			return validate_trade(game, proposer, recipient, terms);
		});
		game.set('f_diplomacy_get_loan_parties', get_loan_parties);
		game.set('f_diplomacy_validate_loan_offer', validate_loan_offer);
		game.set('f_diplomacy_grant_technology', (player, id) => {
			return grant_technology(game, player, id);
		});
		game.on('turn', (e) => {
			if (!game.is_master()) {
				return;
			}
			for (borrower of game.get_players()) {
				for (lender of game.get_players()) {
					if (
						borrower.id != lender.id &&
						borrower.get_diplomatic_loan(lender) != null
					) {
						game.event('process_diplomatic_loan_payment', {
							borrower: borrower,
							lender: lender,
						});
					}
				}
			}
		});
	});
};
