const economic_victory = #include('../default/game/ai/economic_victory');

let own_state = null;
let rival_state = {turn: 30, cost: 1000};
let has_technology = true;
let energy = 1500;
let relation = 'treaty';
let headquarters = {id: 7};
let events = [];

const player = {
	id: 1,
	get_energy_credits: () => { return energy; },
	has_technology: (id) => { return has_technology && id == 'PlanetaryEconomics'; },
	get_diplomatic_relation: (other) => { return relation; },
};
const rival = {id: 2};
const values = {
	f_economic_victory_get_state: (candidate) => {
		return candidate.id == player.id ? own_state : rival_state;
	},
	f_economic_victory_get_cost: (candidate) => { return 1200; },
	f_economic_victory_get_headquarters: (candidate) => {
		return candidate.id == player.id ? headquarters : null;
	},
};
const game = {
	get: (key) => { return values[key]; },
	get_players: () => { return [player, rival]; },
	event_as: (caller, name, data) => { events :+{caller: caller, name: name, data: data}; },
};

economic_victory.update(game, player);
test.assert(#sizeof(events) == 2);
test.assert(events[0].caller == player.id);
test.assert(events[0].name == 'declare_vendetta');
test.assert(events[0].data.target == rival);
test.assert(events[1].name == 'corner_global_energy_market');
test.assert(events[1].data.player == player);

events = [];
relation = 'vendetta';
energy = 1199;
economic_victory.update(game, player);
test.assert(events == []);

energy = 1200;
own_state = {turn: 30, cost: 1200};
economic_victory.update(game, player);
test.assert(events == []);

own_state = null;
has_technology = false;
economic_victory.update(game, player);
test.assert(events == []);

has_technology = true;
headquarters = null;
economic_victory.update(game, player);
test.assert(events == []);
