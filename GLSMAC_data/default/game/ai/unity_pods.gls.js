const pathfinding = #include('./pathfinding');

const MAX_EXPLORATION_DISTANCE = 24;

const choose_destination = (game, unit, can_enter) => {
	if (unit.is_air || unit.is_immovable || unit.get_def().weapon == 'AlienArtifact') {
		return null;
	}
	return pathfinding.find_best_reachable(
		game.get_tm(),
		unit,
		can_enter,
		(candidate, distance) => {
			if (!candidate.features.unity_pod || distance > MAX_EXPLORATION_DISTANCE) {
				return null;
			}
			return 100000 - distance * 100;
		}
	);
};

return {
	max_exploration_distance: MAX_EXPLORATION_DISTANCE,
	choose_destination: choose_destination,
};
