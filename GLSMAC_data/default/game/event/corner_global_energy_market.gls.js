const rules = #include('../economic_victory_rules');

return {

	validate: (e) => {
		if (
			#typeof(e.data.player) != 'Object' ||
			#typeof(e.data.player.has_technology) != 'Callable' ||
			#typeof(e.data.player.get_energy_credits) != 'Callable' ||
			#typeof(e.data.player.set_energy_credits) != 'Callable'
		) {
			return 'Global Energy Market bid requires a player';
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only initiate their own Global Energy Market bid';
		}
		if (e.game.is_game_over()) {
			return 'Game already has a winner';
		}
		if (e.game.is_turn_complete(e.data.player.id)) {
			return 'Player has already completed this turn';
		}
		if (!e.data.player.has_technology('PlanetaryEconomics')) {
			return 'Planetary Economics is required to corner the Global Energy Market';
		}
		if (rules.get_state(e.game, e.data.player) != null) {
			return 'Player already has an active Global Energy Market bid';
		}
		if (rules.get_headquarters(e.game, e.data.player) == null) {
			return 'A Headquarters is required to corner the Global Energy Market';
		}
		const cost = rules.get_cost(e.game, e.data.player);
		if (e.data.player.get_energy_credits() < cost) {
			return 'Global Energy Market bid costs ' + #to_string(cost) +
				' energy credits; only ' +
				#to_string(e.data.player.get_energy_credits()) + ' available';
		}
	},

	apply: (e) => {
		const player = e.data.player;
		const headquarters = rules.get_headquarters(e.game, player);
		const cost = rules.get_cost(e.game, player);
		const previous = {
			energy_credits: player.get_energy_credits(),
			headquarters: headquarters,
			state: rules.get_base_state(headquarters),
		};
		const target_turn = e.game.get_turn() + rules.market_turns;
		player.set_energy_credits(previous.energy_credits - cost);
		rules.set_base_state(headquarters, target_turn, cost);
		e.game.trigger('economic_victory_updated', {player: player});
		e.game.trigger('economy_updated', {player: player});
		e.game.message(
			player.get_faction().name + ' has begun cornering the Global Energy Market. ' +
			'Capture or destroy its Headquarters before M.Y. ' +
			#to_string(target_turn + 2100) + ' to foil the bid.'
		);
		return previous;
	},

	rollback: (e) => {
		e.data.player.set_energy_credits(e.applied.energy_credits);
		if (e.applied.state == null) {
			rules.clear_base_state(e.applied.headquarters);
		} else {
			rules.set_base_state(
				e.applied.headquarters,
				e.applied.state.turn,
				e.applied.state.cost
			);
		}
		e.game.trigger('economic_victory_updated', {player: e.data.player});
		e.game.trigger('economy_updated', {player: e.data.player});
	},

};
