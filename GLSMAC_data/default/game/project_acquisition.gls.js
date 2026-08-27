const apply_empath_guild = (game, base) => {
	const owner = base.get_owner();
	let infiltrated_players = [];
	for (other of game.get_players()) {
		if (other.id == owner.id || owner.has_infiltrated(other)) {
			continue;
		}
		owner.set_infiltrated(other, true);
		infiltrated_players :+other;
	}
	if (#sizeof(infiltrated_players) == 0) {
		return #undefined;
	}
	return {
		player: owner,
		infiltrated_players: infiltrated_players,
	};
};

const rollback_empath_guild = (applied) => {
	for (other of applied.infiltrated_players) {
		applied.player.set_infiltrated(other, false);
	}
};

return {
	apply_empath_guild: apply_empath_guild,
	rollback_empath_guild: rollback_empath_guild,
};
