const define_diplomacy = #include('../default/game/diplomacy');
const propose_loan = #include('../default/game/event/propose_diplomatic_loan');
const respond_loan = #include('../default/game/event/respond_diplomatic_loan');
const process_payment = #include('../default/game/event/process_diplomatic_loan_payment');
const declare_vendetta = #include('../default/game/event/declare_vendetta');

const callbacks = {};
const values = {};
let triggers = [];
let messages = [];
let event_calls = [];
let players = [];
const game = {
	on: (name, callback) => { callbacks[name] = callback; },
	set: (name, value) => { values[name] = value; },
	get: (name) => { return values[name]; },
	is_turn_complete: (player_id) => { return false; },
	is_master: () => { return true; },
	get_players: () => { return players; },
	event: (name, data) => { event_calls :+{name: name, data: data}; },
	trigger: (name, data) => { triggers :+{name: name, data: data}; },
	message: (text) => { messages :+text; },
};
define_diplomacy(game);
callbacks.start({});

const make_player = (id, name, energy, stale_energy_property) => {
	let relations = {};
	let offers = {};
	let trades = {};
	let loan_offers = {};
	let loans = {};
	let current_energy = energy;
	let sanction_turns = 0;
	let integrity_blemishes = 0;
	let player = null;
	const key = (other) => { return 'p' + #to_string(other.id); };
	player = {
		id: id,
		name: name,
		energy_credits: energy,
		get_diplomatic_relation: (other) => {
			const other_key = key(other);
			return #is_defined(relations[other_key]) ? relations[other_key] : 'neutral';
		},
		set_diplomatic_relation: (other, relation) => {
			const other_key = key(other);
			relations[other_key] = relation;
		},
		get_diplomatic_offer: (other) => {
			const other_key = key(other);
			return #is_defined(offers[other_key]) ? offers[other_key] : '';
		},
		set_diplomatic_offer: (other, offer) => {
			const other_key = key(other);
			offers[other_key] = offer;
		},
		get_diplomatic_trade: (other) => {
			const other_key = key(other);
			return #is_defined(trades[other_key]) ? trades[other_key] : null;
		},
		set_diplomatic_trade: (other, trade) => {
			const other_key = key(other);
			trades[other_key] = #clone(trade);
		},
		clear_diplomatic_trade: (other) => {
			const other_key = key(other);
			trades[other_key] = #undefined;
		},
		get_diplomatic_loan_offer: (other) => {
			const other_key = key(other);
			return #is_defined(loan_offers[other_key]) ? #clone(loan_offers[other_key]) : null;
		},
		set_diplomatic_loan_offer: (other, terms) => {
			const other_key = key(other);
			loan_offers[other_key] = #clone(terms);
		},
		clear_diplomatic_loan_offer: (other) => {
			const other_key = key(other);
			loan_offers[other_key] = #undefined;
		},
		get_diplomatic_loan: (other) => {
			const other_key = key(other);
			return #is_defined(loans[other_key]) ? #clone(loans[other_key]) : null;
		},
		set_diplomatic_loan: (other, terms) => {
			const other_key = key(other);
			loans[other_key] = #clone(terms);
		},
		clear_diplomatic_loan: (other) => {
			const other_key = key(other);
			loans[other_key] = #undefined;
		},
		get_sanction_turns: () => { return sanction_turns; },
		set_sanction_turns: (turns) => { sanction_turns = turns; },
		get_integrity_blemishes: () => { return integrity_blemishes; },
		set_integrity_blemishes: (blemishes) => { integrity_blemishes = blemishes; },
		get_energy_credits: () => { return current_energy; },
		set_energy_credits: (value) => {
			current_energy = value;
			if (!stale_energy_property) {
				player.energy_credits = value;
			}
		},
	};
	return player;
};

const alpha = make_player(1, 'Alpha', 20, false);
const beta = make_player(2, 'Beta', 200, false);
players = [alpha, beta];
const terms = {
	proposer_is_lender: false,
	principal: 100,
	payment: 6,
	turns: 20,
};

let proposal = {
	caller: 1,
	game: game,
	data: {player: alpha, target: beta, terms: terms},
};
test.assert(!#is_defined(propose_loan.validate(proposal)));
proposal.applied = propose_loan.apply(proposal);
test.assert(beta.get_diplomatic_loan_offer(alpha).principal == 100);
test.assert(#is_defined(propose_loan.validate(proposal)));
propose_loan.rollback(proposal);
test.assert(beta.get_diplomatic_loan_offer(alpha) == null);
proposal.applied = propose_loan.apply(proposal);

let response = {
	caller: 2,
	game: game,
	data: {player: beta, proposer: alpha, accept: true},
};
test.assert(!#is_defined(respond_loan.validate(response)));
response.applied = respond_loan.apply(response);
test.assert(beta.get_diplomatic_loan_offer(alpha) == null);
test.assert(alpha.energy_credits == 120);
test.assert(beta.energy_credits == 100);
test.assert(alpha.get_diplomatic_loan(beta).balance == 120);
test.assert(alpha.get_diplomatic_loan(beta).payment == 6);

respond_loan.rollback(response);
test.assert(alpha.energy_credits == 20);
test.assert(beta.energy_credits == 200);
test.assert(alpha.get_diplomatic_loan(beta) == null);
test.assert(beta.get_diplomatic_loan_offer(alpha).principal == 100);

response.data.accept = false;
response.applied = respond_loan.apply(response);
test.assert(beta.get_diplomatic_loan_offer(alpha) == null);
respond_loan.rollback(response);
test.assert(beta.get_diplomatic_loan_offer(alpha).payment == 6);

beta.set_energy_credits(50);
response = {
	caller: 2,
	game: game,
	data: {player: beta, proposer: alpha, accept: true},
};
test.assert(beta.energy_credits == 50);
test.assert(#is_defined(values.f_diplomacy_validate_loan_offer(alpha, beta, terms)));
test.assert(#is_defined(respond_loan.validate(response)));
beta.set_energy_credits(200);
response = {
	caller: 2,
	game: game,
	data: {player: beta, proposer: alpha, accept: true},
};
response.applied = respond_loan.apply(response);
test.assert(alpha.energy_credits == 120);
test.assert(beta.energy_credits == 100);

let payment = {
	caller: 0,
	game: game,
	data: {borrower: alpha, lender: beta},
};
test.assert(!#is_defined(process_payment.validate(payment)));
payment.applied = process_payment.apply(payment);
test.assert(alpha.energy_credits == 114);
test.assert(beta.energy_credits == 106);
test.assert(alpha.get_diplomatic_loan(beta).balance == 114);
process_payment.rollback(payment);
test.assert(alpha.energy_credits == 120);
test.assert(beta.energy_credits == 100);
test.assert(alpha.get_diplomatic_loan(beta).balance == 120);

alpha.set_sanction_turns(3);
payment = {
	caller: 0,
	game: game,
	data: {borrower: alpha, lender: beta},
};
payment.applied = process_payment.apply(payment);
test.assert(alpha.energy_credits == 120);
test.assert(beta.energy_credits == 100);
test.assert(alpha.get_diplomatic_loan(beta).balance == 120);
process_payment.rollback(payment);
alpha.set_sanction_turns(0);

alpha.set_energy_credits(2);
payment = {
	caller: 0,
	game: game,
	data: {borrower: alpha, lender: beta},
};
payment.applied = process_payment.apply(payment);
test.assert(alpha.energy_credits == 0);
test.assert(beta.energy_credits == 102);
test.assert(alpha.get_diplomatic_loan(beta).balance == 118);

alpha.set_diplomatic_relation(beta, 'vendetta');
beta.set_diplomatic_relation(alpha, 'vendetta');
payment = {
	caller: 0,
	game: game,
	data: {borrower: alpha, lender: beta},
};
payment.applied = process_payment.apply(payment);
test.assert(alpha.energy_credits == 0);
test.assert(beta.energy_credits == 102);
test.assert(alpha.get_diplomatic_loan(beta).balance == 124);
process_payment.rollback(payment);
test.assert(alpha.get_diplomatic_loan(beta).balance == 118);

alpha.set_diplomatic_relation(beta, 'treaty');
beta.set_diplomatic_relation(alpha, 'treaty');
alpha.set_energy_credits(10);
beta.set_energy_credits(100);
alpha.set_diplomatic_loan(beta, {balance: 4, payment: 6});
payment = {
	caller: 0,
	game: game,
	data: {borrower: alpha, lender: beta},
};
payment.applied = process_payment.apply(payment);
test.assert(alpha.energy_credits == 6);
test.assert(beta.energy_credits == 104);
test.assert(alpha.get_diplomatic_loan(beta) == null);
process_payment.rollback(payment);
test.assert(alpha.energy_credits == 10);
test.assert(beta.energy_credits == 100);
test.assert(alpha.get_diplomatic_loan(beta).balance == 4);

event_calls = [];
callbacks.turn({});
test.assert(#sizeof(event_calls) == 1);
test.assert(event_calls[0].name == 'process_diplomatic_loan_payment');
test.assert(event_calls[0].data.borrower.id == alpha.id);
test.assert(event_calls[0].data.lender.id == beta.id);

let vendetta = {
	caller: 1,
	game: game,
	data: {player: alpha, target: beta},
};
vendetta.applied = declare_vendetta.apply(vendetta);
test.assert(alpha.get_diplomatic_loan(beta).balance == 4);
declare_vendetta.rollback(vendetta);
test.assert(alpha.get_diplomatic_loan(beta).balance == 4);

alpha.clear_diplomatic_loan(beta);
beta.set_diplomatic_loan_offer(alpha, terms);
vendetta = {
	caller: 1,
	game: game,
	data: {player: alpha, target: beta},
};
vendetta.applied = declare_vendetta.apply(vendetta);
test.assert(beta.get_diplomatic_loan_offer(alpha) == null);
declare_vendetta.rollback(vendetta);
test.assert(beta.get_diplomatic_loan_offer(alpha).principal == 100);
beta.clear_diplomatic_loan_offer(alpha);

proposal.data.terms = {
	proposer_is_lender: false,
	principal: 100,
	payment: 4,
	turns: 20,
};
test.assert(#is_defined(propose_loan.validate(proposal)));
proposal.data.terms = {
	proposer_is_lender: false,
	principal: 100,
	payment: 21,
	turns: 20,
};
test.assert(#is_defined(propose_loan.validate(proposal)));
proposal.data.terms = terms;
alpha.set_diplomatic_relation(beta, 'vendetta');
test.assert(#is_defined(propose_loan.validate(proposal)));
alpha.set_diplomatic_relation(beta, 'treaty');
alpha.set_sanction_turns(10);
test.assert(#is_defined(propose_loan.validate(proposal)));
alpha.set_sanction_turns(0);

proposal.data.target = alpha;
test.assert(#is_defined(propose_loan.validate(proposal)));
proposal.data.target = beta;

beta.set_diplomatic_loan_offer(alpha, terms);
test.assert(#is_defined(propose_loan.validate(proposal)));
beta.clear_diplomatic_loan_offer(alpha);

test.assert(#sizeof(triggers) >= 10);
test.assert(#sizeof(messages) >= 2);

const stale_borrower = make_player(3, 'Stale Borrower', 10, true);
const first_lender = make_player(4, 'First Lender', 0, true);
const second_lender = make_player(5, 'Second Lender', 0, true);
stale_borrower.set_diplomatic_loan(first_lender, {balance: 10, payment: 6});
stale_borrower.set_diplomatic_loan(second_lender, {balance: 10, payment: 6});
let first_payment = {
	caller: 0,
	game: game,
	data: {borrower: stale_borrower, lender: first_lender},
};
let second_payment = {
	caller: 0,
	game: game,
	data: {borrower: stale_borrower, lender: second_lender},
};
process_payment.apply(first_payment);
process_payment.apply(second_payment);
test.assert(stale_borrower.energy_credits == 10);
test.assert(stale_borrower.get_energy_credits() == 0);
test.assert(first_lender.get_energy_credits() == 6);
test.assert(second_lender.get_energy_credits() == 4);
test.assert(stale_borrower.get_diplomatic_loan(first_lender).balance == 4);
test.assert(stale_borrower.get_diplomatic_loan(second_lender).balance == 6);
