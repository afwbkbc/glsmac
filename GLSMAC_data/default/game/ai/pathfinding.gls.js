const MAX_SEARCHED_TILES = 4096;

const get_tile_key = (tile) => {
	return #to_string(tile.x) + '_' + #to_string(tile.y);
};

const find_path_step = (tm, unit, destination, can_enter) => {
	const source = unit.get_tile();
	const source_x = source.x;
	const source_y = source.y;
	let visited = {};
	let queue_x = [source_x];
	let queue_y = [source_y];
	let queue_first_x = [0 - 1];
	let queue_first_y = [0 - 1];
	const source_key = get_tile_key(source);
	visited[source_key] = true;

	let index = 0;
	while (index < #sizeof(queue_x) && index < MAX_SEARCHED_TILES) {
		const current = tm.get_tile(queue_x[index], queue_y[index]);
		const first_x = queue_first_x[index];
		const first_y = queue_first_y[index];
		index++;

		for (candidate of current.get_surrounding_tiles()) {
			const key = get_tile_key(candidate);
			if (#is_defined(visited[key]) || !can_enter(current, candidate)) {
				continue;
			}
			visited[key] = true;
			const candidate_x = candidate.x;
			const candidate_y = candidate.y;
			const candidate_first_x = first_x < 0 ? candidate_x : first_x;
			const candidate_first_y = first_y < 0 ? candidate_y : first_y;
			if (candidate == destination || candidate.is_adjactent_to(destination)) {
				return tm.get_tile(candidate_first_x, candidate_first_y);
			}
			queue_x :+candidate_x;
			queue_y :+candidate_y;
			queue_first_x :+candidate_first_x;
			queue_first_y :+candidate_first_y;
		}
	}
	return null;
};

const find_best_reachable = (tm, unit, can_enter, score) => {
	const source = unit.get_tile();
	let visited = {};
	let queue_x = [source.x];
	let queue_y = [source.y];
	let queue_first_x = [0 - 1];
	let queue_first_y = [0 - 1];
	let queue_distance = [0];
	const source_key = get_tile_key(source);
	visited[source_key] = true;

	let best = null;
	let best_score = 0;
	let best_distance = 0;
	let best_first_x = 0 - 1;
	let best_first_y = 0 - 1;
	let index = 0;
	while (index < #sizeof(queue_x) && index < MAX_SEARCHED_TILES) {
		const current = tm.get_tile(queue_x[index], queue_y[index]);
		const first_x = queue_first_x[index];
		const first_y = queue_first_y[index];
		const distance = queue_distance[index];
		index++;

		const current_score = score(current, distance);
		if (
			current_score != null &&
			(
				best == null ||
				current_score > best_score ||
				(
					current_score == best_score &&
					(
						distance < best_distance ||
						(
							distance == best_distance &&
							(current.y < best.y || (current.y == best.y && current.x < best.x))
						)
					)
				)
			)
		) {
			best = current;
			best_score = current_score;
			best_distance = distance;
			best_first_x = first_x;
			best_first_y = first_y;
		}

		for (candidate of current.get_surrounding_tiles()) {
			const key = get_tile_key(candidate);
			if (#is_defined(visited[key]) || !can_enter(current, candidate)) {
				continue;
			}
			visited[key] = true;
			queue_x :+candidate.x;
			queue_y :+candidate.y;
			queue_first_x :+(first_x < 0 ? candidate.x : first_x);
			queue_first_y :+(first_y < 0 ? candidate.y : first_y);
			queue_distance :+(distance + 1);
		}
	}
	if (best == null) {
		return null;
	}
	return {
		target: best,
		step: best == source ? null : tm.get_tile(best_first_x, best_first_y),
		distance: best_distance,
	};
};

return {
	find_path_step: find_path_step,
	find_best_reachable: find_best_reachable,
};
