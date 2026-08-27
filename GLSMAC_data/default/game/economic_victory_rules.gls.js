const MARKET_TURNS = 20;
const MIN_MARKET_COST = 1000;
const MAX_ENERGY_CREDITS = 1000000000;
const TURN_KEY = 'economic_victory_turn';
const COST_KEY = 'economic_victory_cost';

const get_energy = (player) => {
	return #typeof(player.get_energy_credits) == 'Callable'
		? player.get_energy_credits()
		: player.energy_credits;
};

const get_headquarters = (game, player) => {
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id == player.id && base.has_facility('Headquarters')) {
			return base;
		}
	}
	return null;
};

const get_base_state = (base) => {
	if (
		#typeof(base.has) != 'Callable' || #typeof(base.get) != 'Callable' ||
		!base.has(TURN_KEY) || !base.has(COST_KEY) ||
		base.get(TURN_KEY) <= 0 || base.get(COST_KEY) <= 0
	) {
		return null;
	}
	return {
		base: base,
		turn: base.get(TURN_KEY),
		cost: base.get(COST_KEY),
	};
};

const get_state = (game, player) => {
	const headquarters = get_headquarters(game, player);
	return headquarters == null ? null : get_base_state(headquarters);
};

const set_base_state = (base, turn, cost) => {
	base.set(TURN_KEY, turn);
	base.set(COST_KEY, cost);
};

const clear_base_state = (base) => {
	if (base.has(TURN_KEY)) {
		base.unset(TURN_KEY);
	}
	if (base.has(COST_KEY)) {
		base.unset(COST_KEY);
	}
};

const get_headquarters_distance = (game, player, tile) => {
	const headquarters = get_headquarters(game, player);
	return headquarters == null
		? 12
		: #max(1, game.get_tm().get_distance(headquarters.get_tile(), tile));
};

const get_garrison_value = (game, base) => {
	let value = 0;
	for (unit of game.get_um().get_units()) {
		if (unit.owner == base.get_owner().id && unit.get_tile() == base.get_tile()) {
			value += #max(unit.get_def().mineral_cost, 10);
		}
	}
	return value;
};

const get_commerce_technology = (game, player) => {
	const resolver = game.get('f_economy_get_commerce_technology');
	return #is_defined(resolver) ? resolver(player) : 0;
};

const get_market_base_cost = (game, actor, base) => {
	const target = base.get_owner();
	let distance = get_headquarters_distance(game, target, base.get_tile());
	if (base.has_facility('GenejackFactory')) {
		distance *= 2;
	}
	if (base.has_facility('ChildrenSCreche')) {
		distance = #max(1, #floor(#to_float(distance) / 2.0));
	}
	if (base.has_facility('PunishmentSphere')) {
		distance = #max(1, #floor(#to_float(distance) / 2.0));
	}
	const population_value = base.get_size() * 10;
	const garrison_value = get_garrison_value(game, base);
	let cost = #to_float(#max(population_value + garrison_value, 20)) *
		#to_float(get_energy(target) + 1200) /
		#to_float((distance + 4) * 10);
	const relation = actor.get_diplomatic_relation(target);
	if (relation == 'treaty' || relation == 'pact') {
		cost *= 0.5;
	}
	const target_commerce = get_commerce_technology(game, target);
	const actor_commerce = get_commerce_technology(game, actor);
	cost *= #to_float(target_commerce * target_commerce + 1) /
		#to_float(actor_commerce * actor_commerce + 1);
	return #max(1, #ceil(cost));
};

const get_cost = (game, actor) => {
	let cost = 0;
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id != actor.id) {
			cost += get_market_base_cost(game, actor, base);
		}
	}
	return #min(MAX_ENERGY_CREDITS, #max(MIN_MARKET_COST, cost));
};

const get_winner = (game) => {
	let winner = null;
	let winning_turn = 0;
	for (player of game.get_players()) {
		const state = get_state(game, player);
		if (
			state != null && state.turn <= game.get_turn() &&
			(
				winner == null || state.turn < winning_turn ||
				(state.turn == winning_turn && player.id < winner.id)
			)
		) {
			winner = player;
			winning_turn = state.turn;
		}
	}
	return winner;
};

return {
	market_turns: MARKET_TURNS,
	turn_key: TURN_KEY,
	cost_key: COST_KEY,
	get_headquarters: get_headquarters,
	get_base_state: get_base_state,
	get_state: get_state,
	set_base_state: set_base_state,
	clear_base_state: clear_base_state,
	get_market_base_cost: get_market_base_cost,
	get_cost: get_cost,
	get_winner: get_winner,
};
