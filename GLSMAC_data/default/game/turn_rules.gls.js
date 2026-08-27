const has_pending_owned_animation = (game, player_id) => {
	for (unit of game.get_um().get_units()) {
		if (unit.owner == player_id && unit.get_tile().is_locked()) {
			return true;
		}
	}
	return false;
};

return {
	has_pending_owned_animation: has_pending_owned_animation,
};
