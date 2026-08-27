const snapshots = #include('./entity_snapshots');

const CAPTURE_MOVE_COST = 1.0;
const EXCLUDED_DEFINITIONS = {
	AlienArtifact: true,
	FungalTower: true,
};

const get_planet_rating = (game, player) => {
	const resolver = game.get('f_social_get_ratings');
	return #is_defined(resolver) ? resolver(player).planet : 0;
};

const get_inherent_planet_rating = (game, player) => {
	const resolver = game.get('f_social_get_faction_modifier');
	return #is_defined(resolver) ? resolver(player, 'planet') : 0;
};

const count_owned_definition = (game, player, definition_id) => {
	let count = 0;
	for (unit of game.um.get_units(true)) {
		if (unit.owner == player.id && unit.def == definition_id && unit.health > 0.0) {
			count++;
		}
	}
	return count;
};

const get_nearest_owned_base = (game, player, tile) => {
	let nearest = null;
	let nearest_distance = 0;
	for (base of game.bm.get_bases()) {
		if (base.get_owner().id != player.id) {
			continue;
		}
		const distance = game.tm.get_distance(tile, base.get_tile());
		if (
			nearest == null || distance < nearest_distance ||
			(distance == nearest_distance && base.id < nearest.id)
		) {
			nearest = base;
			nearest_distance = distance;
		}
	}
	return nearest;
};

const is_ecologically_agitated = (game, player, tile) => {
	const get_damage = game.get('f_ecology_get_base_damage');
	if (!#is_defined(get_damage)) {
		return false;
	}
	const base = get_nearest_owned_base(game, player, tile);
	if (base == null) {
		return false;
	}
	const damage = get_damage(base);
	if (#typeof(damage) == 'Object') {
		return #is_defined(damage.value) && damage.value > 0;
	}
	return (
		#typeof(damage) == 'Int' || #typeof(damage) == 'Float'
	) && damage > 0;
};

const get_stack_ids = (tile, owner_id) => {
	let ids = [];
	for (unit of tile.get_units(true)) {
		if (unit.owner == owner_id && unit.health > 0.0) {
			ids :+unit.id;
		}
	}
	return ids;
};

const no_attempt = () => {
	return {
		attempted: false,
		captured: false,
		mark_attempted: false,
		reason: '',
		unit_ids: [],
	};
};

const resolve = (game, attacker, target) => {
	if (
		#typeof(attacker.get_owner) != 'Callable' ||
		#typeof(target.get_owner) != 'Callable'
	) {
		return no_attempt();
	}
	const attacker_def = attacker.get_def();
	const target_def = target.get_def();
	const player = attacker.get_owner();
	const target_owner = target.get_owner();
	if (
		player.type == 'native' || target_owner.type != 'native' ||
		attacker.is_air || attacker_def.is_artillery || attacker_def.id == 'SporeLauncher' ||
		!target_def.is_native || #is_defined(EXCLUDED_DEFINITIONS[target_def.id])
	) {
		return no_attempt();
	}
	const planet = get_planet_rating(game, player);
	if (planet <= 0) {
		return no_attempt();
	}
	const owned_count = count_owned_definition(game, player, target_def.id);
	const guaranteed = get_inherent_planet_rating(game, player) > 0 && owned_count == 0;
	if (!guaranteed && attacker.movement < CAPTURE_MOVE_COST) {
		return no_attempt();
	}
	const unit_ids = get_stack_ids(target.get_tile(), target_owner.id);
	if (#sizeof(unit_ids) == 0) {
		return no_attempt();
	}
	if (is_ecologically_agitated(game, player, target.get_tile())) {
		return {
			attempted: true,
			captured: false,
			mark_attempted: false,
			reason: 'agitated',
			unit_ids: unit_ids,
		};
	}
	if (target.native_capture_attempted) {
		return {
			attempted: true,
			captured: false,
			mark_attempted: true,
			reason: 'previously_attempted',
			unit_ids: unit_ids,
		};
	}
	let captured = guaranteed;
	if (!captured && game.random.get_int(0, 3) < planet) {
		const turn = #is_defined(game.get_turn) ? game.get_turn() : 0;
		const population_roll = game.random.get_int(0, #floor(#to_float(turn) / 5.0) + 99);
		captured = population_roll >= 10 * owned_count;
		if (!captured) {
			captured = game.random.get_int(0, 9) == 0;
		}
	}
	return {
		attempted: true,
		captured: captured,
		mark_attempted: !captured,
		reason: captured ? (guaranteed ? 'first_native' : 'roll') : 'roll_failed',
		unit_ids: unit_ids,
	};
};

const get_capture_home_base_id = (game, player, tile) => {
	let nearest = null;
	let nearest_distance = 0;
	for (base of game.bm.get_bases()) {
		const distance = game.tm.get_distance(tile, base.get_tile());
		if (
			nearest == null || distance < nearest_distance ||
			(distance == nearest_distance && base.id < nearest.id)
		) {
			nearest = base;
			nearest_distance = distance;
		}
	}
	return nearest != null && nearest.get_owner().id == player.id && nearest.get_size() >= 3
		? nearest.id : 0;
};

const apply = (game, player, tile, resolved) => {
	let captured = [];
	for (id of resolved.unit_ids) {
		if (!game.um.has_unit(id)) {
			throw Error('Native capture unit no longer exists: ' + #to_string(id));
		}
		captured :+snapshots.snapshot_unit(game.um.get_unit(id));
	}
	snapshots.despawn_unit_snapshots(game, captured);
	snapshots.spawn_unit_snapshots_as(game, captured, player, true);
	const home_base_id = get_capture_home_base_id(game, player, tile);
	if (home_base_id > 0) {
		for (snapshot of captured) {
			game.um.get_unit(snapshot.id).set_home_base_id(home_base_id);
		}
	}
	return {units: captured};
};

const rollback = (game, applied) => {
	snapshots.despawn_unit_snapshots(game, applied.units);
	snapshots.spawn_unit_snapshots(game, applied.units);
};

return {
	move_cost: CAPTURE_MOVE_COST,
	get_planet_rating: get_planet_rating,
	get_inherent_planet_rating: get_inherent_planet_rating,
	count_owned_definition: count_owned_definition,
	resolve: resolve,
	apply: apply,
	rollback: rollback,
};
