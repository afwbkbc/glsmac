const consider = (best, candidate) => {
	if (
		candidate != null &&
		(
			best == null || candidate.score > best.score ||
			(candidate.score == best.score && candidate.target.id < best.target.id)
		)
	) {
		return candidate;
	}
	return best;
};

const get_base_action = (game, player, probe, base) => {
	const target_player = base.get_owner();
	if (target_player.id == player.id) {
		return null;
	}
	const relation = player.get_diplomatic_relation(target_player);
	if (
		relation == 'treaty' || relation == 'pact' ||
		game.get('f_probe_has_project')(target_player, 'TheHunterSeekerAlgorithm')
	) {
		return null;
	}
	const has_intelligence = game.get('f_council_has_intelligence');
	if (!(#is_defined(has_intelligence)
		? has_intelligence(player, target_player)
		: player.has_infiltrated(target_player))) {
		return {operation: 'infiltrate', target: base, score: 130000};
	}
	if (relation != 'vendetta') {
		return null;
	}

	let best = null;
	const mind_control_cost = game.get('f_probe_get_mind_control_cost')(player, base);
	if (
		mind_control_cost != null && mind_control_cost <= player.energy_credits &&
		mind_control_cost <= #max(100, #floor(#to_float(player.energy_credits) * 0.75))
	) {
		best = consider(best, {
			operation: 'mind_control_base',
			target: base,
			score: 150000 + base.get_size() * 5000 - mind_control_cost * 100,
		});
	}
	const unknown = game.get('f_probe_get_unknown_technologies')(player, target_player);
	if (#sizeof(unknown) > 0) {
		best = consider(best, {
			operation: 'steal_technology', target: base, score: 120000 + #sizeof(unknown) * 1000,
		});
	}
	if (target_player.energy_credits >= 40 && player.energy_credits < 1000000000) {
		best = consider(best, {
			operation: 'drain_energy', target: base,
			score: 90000 + #min(target_player.energy_credits, 500) * 50,
		});
	}
	const research_loss = game.get('f_probe_get_assassination_research_loss')(target_player);
	if (probe.morale >= 3 && research_loss > 0) {
		best = consider(best, {
			operation: 'assassinate_researchers', target: base,
			score: 100000 + research_loss * 200,
		});
	}
	if (game.get('f_probe_can_incite_drone_riots')(base)) {
		best = consider(best, {
			operation: 'incite_drone_riots', target: base,
			score: 80000 + base.get_size() * 2000,
		});
	}
	const plague_loss = game.get('f_probe_get_plague_population_loss')(base);
	if (
		player.has_technology('RetroviralEngineering') && base.get_size() >= 6 &&
		plague_loss > 0
	) {
		const commerce = game.get('f_economy_get_player_commerce')(game, player);
		const sanction_cost = player.get_sanction_turns() > 0 ? 0 : commerce * 10000;
		best = consider(best, {
			operation: 'genetic_plague', target: base,
			score: 95000 + plague_loss * 7000 - sanction_cost,
		});
	}
	if (game.get('f_probe_can_sabotage')(base)) {
		best = consider(best, {
			operation: 'sabotage', target: base,
			score: 70000 + base.get_accumulated_minerals() * 100,
		});
	}
	return best;
};

const get_unit_action = (game, player, probe, unit) => {
	if (unit.owner == player.id) {
		return null;
	}
	const target_player = game.get_player(unit.owner);
	if (
		player.get_diplomatic_relation(target_player) != 'vendetta' ||
		game.get('f_probe_has_project')(target_player, 'TheHunterSeekerAlgorithm') ||
		(#is_defined(unit.transport_id) && unit.transport_id > 0) ||
		(#is_defined(unit.get_cargo) && #sizeof(unit.get_cargo()) > 0)
	) {
		return null;
	}
	const cost = game.get('f_probe_get_subversion_cost')(player, unit);
	if (
		cost == null || cost > player.energy_credits ||
		cost > #max(50, #floor(#to_float(player.energy_credits) * 0.5))
	) {
		return null;
	}
	return {
		operation: 'subvert_unit',
		target: unit,
		score: 80000 + unit.get_def().mineral_cost * 1000 - cost * 100,
	};
};

const choose_adjacent_action = (game, player, probe) => {
	let best = null;
	for (tile of probe.get_tile().get_surrounding_tiles()) {
		const base = tile.get_base();
		if (base != null) {
			best = consider(best, get_base_action(game, player, probe, base));
		}
		for (unit of tile.get_units()) {
			best = consider(best, get_unit_action(game, player, probe, unit));
		}
	}
	return best;
};

const choose_target_base = (game, player, probe, bases) => {
	let best = null;
	for (base of bases) {
		const action = get_base_action(game, player, probe, base);
		if (action == null) {
			continue;
		}
		const candidate = {
			base: base,
			action: action,
			score: action.score - game.get_tm().get_distance(probe.get_tile(), base.get_tile()) * 1000,
		};
		if (
			best == null || candidate.score > best.score ||
			(candidate.score == best.score && base.id < best.base.id)
		) {
			best = candidate;
		}
	}
	return best;
};

return {
	get_base_action: get_base_action,
	get_unit_action: get_unit_action,
	choose_adjacent_action: choose_adjacent_action,
	choose_target_base: choose_target_base,
};
