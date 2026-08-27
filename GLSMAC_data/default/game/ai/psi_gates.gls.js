const rules = #include('../psi_gate_rules');

const choose_destination = (game, player, unit, target_tile, all_bases) => {
	if (target_tile == null) {
		return null;
	}
	const tm = game.get_tm();
	const current_distance = tm.get_distance(unit.get_tile(), target_tile);
	let best = null;
	let best_distance = current_distance;
	for (base of all_bases) {
		if (#is_defined(rules.get_teleport_error(game, unit, player.id, base))) {
			continue;
		}
		const distance = tm.get_distance(base.get_tile(), target_tile);
		if (
			distance < best_distance ||
			(distance == best_distance && best != null && base.id < best.id)
		) {
			best = base;
			best_distance = distance;
		}
	}
	return best;
};

const try_teleport = (game, player, unit, target_tile, all_bases) => {
	const destination = choose_destination(
		game,
		player,
		unit,
		target_tile,
		all_bases
	);
	if (destination == null) {
		return false;
	}
	game.event_as(player.id, 'teleport_unit', {
		unit: unit,
		destination: destination,
	});
	return true;
};

return {
	choose_destination: choose_destination,
	try_teleport: try_teleport,
};
