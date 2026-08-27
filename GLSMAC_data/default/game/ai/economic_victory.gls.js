const update = (game, player) => {
	const get_state = game.get('f_economic_victory_get_state');
	const get_cost = game.get('f_economic_victory_get_cost');
	const get_headquarters = game.get('f_economic_victory_get_headquarters');
	if (!#is_defined(get_state) || !#is_defined(get_cost) || !#is_defined(get_headquarters)) {
		return;
	}

	for (other of game.get_players()) {
		if (other.id == player.id || get_state(other) == null) {
			continue;
		}
		if (player.get_diplomatic_relation(other) != 'vendetta') {
			game.event_as(player.id, 'declare_vendetta', {
				player: player,
				target: other,
			});
			break;
		}
	}

	if (
		get_state(player) != null || !player.has_technology('PlanetaryEconomics') ||
		get_headquarters(player) == null
	) {
		return;
	}
	const cost = get_cost(player);
	if (player.get_energy_credits() >= cost) {
		game.event_as(player.id, 'corner_global_energy_market', {player: player});
	}
};

return {
	update: update,
};
