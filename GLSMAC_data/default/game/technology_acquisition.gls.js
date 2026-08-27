const clone_state = (state) => {
	let technologies = [];
	for (id of state.technologies) {
		technologies :+id;
	}
	return {
		technologies: technologies,
		target: state.target,
		progress: state.progress,
	};
};

const get_next_target = (game, player, known, target) => {
	return target != ''
		? target
		: game.get('f_technology_get_next_target')(known, player);
};

const can_grant = (game, player) => {
	const state = player.get_research_state();
	return get_next_target(game, player, state.technologies, state.target) != '';
};

const apply = (game, player, count) => {
	const previous = clone_state(player.get_research_state());
	let technologies = [];
	for (id of previous.technologies) {
		technologies :+id;
	}
	let target = previous.target;
	let progress = previous.progress;
	let completed_names = [];
	for (let i = 0; i < count; i++) {
		target = get_next_target(game, player, technologies, target);
		if (target == '') {
			break;
		}
		const definition = game.get('f_technology_get_definition')(target);
		if (definition == null) {
			throw Error('Unknown free technology target: ' + target);
		}
		technologies :+target;
		completed_names :+definition.name;
		target = game.get('f_technology_get_next_target')(technologies, player);
	}
	if (#sizeof(completed_names) == 0) {
		return #undefined;
	}
	if (target == '') {
		progress = 0;
	}
	player.set_research_state({
		technologies: technologies,
		target: target,
		progress: progress,
	});
	game.trigger('research_updated', {player: player});
	const queue_datalinks = game.get('f_project_queue_planetary_datalinks');
	if (#is_defined(queue_datalinks)) {
		queue_datalinks();
	}
	return {
		player: player,
		state: previous,
		completed_names: completed_names,
		completed_count: #sizeof(completed_names),
	};
};

const rollback = (game, applied) => {
	applied.player.set_research_state(applied.state);
	game.trigger('research_updated', {player: applied.player});
};

return {
	can_grant: can_grant,
	apply: apply,
	rollback: rollback,
};
