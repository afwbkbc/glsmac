const is_orbital = (definition) => {
	return #is_defined(definition) && (
		(
			#is_defined(definition.orbital_resource) &&
			definition.orbital_resource != ''
		) || (
			#is_defined(definition.orbital_defense) &&
			definition.orbital_defense
		)
	);
};

const has_space_elevator = (game, player) => {
	const has_project = #is_defined(game.get)
		? game.get('f_project_has')
		: #undefined;
	if (#is_defined(has_project)) {
		return has_project(player, 'TheSpaceElevator');
	}
	for (base of game.get_bm().get_bases()) {
		if (
			base.get_owner().id == player.id &&
			base.has_facility('TheSpaceElevator')
		) {
			return true;
		}
	}
	return false;
};

const has_full_access = (game, base) => {
	return base.has_facility('AerospaceComplex') ||
		has_space_elevator(game, base.get_owner());
};

const get_resource_count = (game, player, resource, changed_id, count_delta) => {
	let count = 0;
	for (definition of game.get_bm().get_facility_defs()) {
		if (
			#is_defined(definition.orbital_resource) &&
			definition.orbital_resource == resource
		) {
			count += player.get_orbital_facility_count(definition.id) +
				(definition.id == changed_id ? count_delta : 0);
		}
	}
	return #max(count, 0);
};

const get_base_resource_bonus_with_delta = (
	game,
	base,
	resource,
	changed_id,
	count_delta
) => {
	let count = get_resource_count(
		game,
		base.get_owner(),
		resource,
		changed_id,
		count_delta
	);
	if (!has_full_access(game, base)) {
		count = #floor(#to_float(count) / 2.0);
	}
	return #min(count, base.get_size());
};

const get_base_resource_bonus = (game, base, resource) => {
	return get_base_resource_bonus_with_delta(game, base, resource, '', 0);
};

const get_marginal_yield = (game, player, definition) => {
	if (
		!is_orbital(definition) ||
		!#is_defined(definition.orbital_resource) ||
		definition.orbital_resource == ''
	) {
		return 0;
	}
	let result = 0;
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id != player.id) {
			continue;
		}
		result += get_base_resource_bonus_with_delta(
			game,
			base,
			definition.orbital_resource,
			definition.id,
			1
		) - get_base_resource_bonus(
			game,
			base,
			definition.orbital_resource
		);
	}
	return result;
};

const apply_launch = (game, base, definition) => {
	if (!is_orbital(definition)) {
		throw Error('Cannot launch non-orbital production');
	}
	const player = base.get_owner();
	const count = player.get_orbital_facility_count(definition.id);
	player.set_orbital_facility_count(definition.id, count + 1);
	if (#is_defined(game.message)) {
		game.message(
			player.name + ' launched ' + definition.name + ' (' +
			#to_string(count + 1) + ' in orbit).'
		);
	}
	return {player: player, id: definition.id, count: count};
};

const rollback_launch = (applied) => {
	applied.player.set_orbital_facility_count(applied.id, applied.count);
};

return {
	is_orbital: is_orbital,
	has_space_elevator: has_space_elevator,
	has_full_access: has_full_access,
	get_base_resource_bonus: get_base_resource_bonus,
	get_marginal_yield: get_marginal_yield,
	apply_launch: apply_launch,
	rollback_launch: rollback_launch,
};
