const RETREAT_HEALTH = 0.5;
const RECOVERED_HEALTH = 0.8;
const THREAT_DISTANCE = 2;
const MAX_GARRISON = 3;
const ASSAULT_DEFENSE_WEIGHT = 2.0;
const ASSAULT_SUPPORT_DISTANCE = 3;
const ASSAULT_SUPPORT_WEIGHT = 0.5;
const ASSAULT_BASE_SIZE_WEIGHT = 0.25;
const ECONOMIC_VICTORY_TARGET_BONUS = 100000.0;
const MIN_DIRECT_ATTACK_SCORE = 0.5;
const MIN_GROUP_ATTACK_SCORE = 0.55;
const ATTACK_SUPPORT_DISTANCE = 1;
const combat_rules = #include('../combat_rules');

const is_triad_blocked = (unit, tile) => {
	return combat_rules.is_triad_attack_blocked(unit, tile);
};

const find_nearest_friendly_base = (tm, player_id, tile, bases) => {
	let nearest = null;
	let nearest_distance = 100000;
	for (base of bases) {
		if (base.get_owner().id != player_id) {
			continue;
		}
		const base_tile = base.get_tile();
		const distance = tm.get_distance(tile, base_tile);
		if (
			nearest == null ||
			distance < nearest_distance ||
			(
				distance == nearest_distance &&
				(
					base_tile.y < nearest.get_tile().y ||
					(base_tile.y == nearest.get_tile().y && base_tile.x < nearest.get_tile().x)
				)
			)
		) {
			nearest = base;
			nearest_distance = distance;
		}
	}
	return nearest;
};

const get_repair_destination = (tm, unit, player_id, bases) => {
	const tile = unit.get_tile();
	const current_base = tile.get_base();
	if (current_base != null && current_base.get_owner().id == player_id) {
		return unit.health < RECOVERED_HEALTH ? current_base : null;
	}
	if (unit.health >= RETREAT_HEALTH) {
		return null;
	}
	return find_nearest_friendly_base(tm, player_id, tile, bases);
};

const can_threaten_tile = (unit, tile) => {
	const def = unit.get_def();
	if (
		def.offense <= 0 ||
		unit.health <= 0.0 ||
		(#is_defined(unit.is_immovable) && unit.is_immovable)
	) {
		return false;
	}
	return combat_rules.is_artillery(def) || !is_triad_blocked(unit, tile);
};

const get_required_garrison = (tm, base, player_id, units) => {
	let result = 1;
	for (unit of units) {
		if (
			unit.owner != player_id &&
			can_threaten_tile(unit, base.get_tile()) &&
			tm.get_distance(base.get_tile(), unit.get_tile()) <= THREAT_DISTANCE
		) {
			result++;
			if (result >= MAX_GARRISON) {
				break;
			}
		}
	}
	return result;
};

const get_garrison_count = (base, player_id) => {
	let result = 0;
	for (unit of base.get_tile().get_units()) {
		if (unit.owner == player_id && unit.get_def().offense > 0) {
			result++;
		}
	}
	return result;
};

const get_force_power = (unit) => {
	const def = unit.get_def();
	if (def.offense <= 0 || unit.health <= 0.0) {
		return 0.0;
	}
	return (
		#to_float(def.offense * 2 + def.defense) +
		def.movement_per_turn * 0.5
	) * combat_rules.get_morale_multiplier(unit) * unit.health;
};

const get_reinforcement_score = (tm, unit, base, player_id, units, reservations) => {
	const tile = base.get_tile();
	if (
		base.get_owner().id != player_id ||
		is_triad_blocked(unit, tile)
	) {
		return null;
	}
	const key = #to_string(base.id);
	const reserved = #is_defined(reservations[key]) ? reservations[key] : 0;
	const required = get_required_garrison(tm, base, player_id, units);
	const shortage = required - get_garrison_count(base, player_id) - reserved;
	if (shortage <= 0) {
		return null;
	}
	return shortage * 10000 + required * 1000 - tm.get_distance(unit.get_tile(), tile) * 100;
};

const choose_reinforcement_target = (tm, unit, player_id, bases, units, reservations) => {
	let best = null;
	let best_score = 0;
	for (base of bases) {
		const score = get_reinforcement_score(tm, unit, base, player_id, units, reservations);
		if (score == null) {
			continue;
		}
		const tile = base.get_tile();
		if (
			best == null ||
			score > best_score ||
			(
				score == best_score &&
				(tile.y < best.get_tile().y || (tile.y == best.get_tile().y && tile.x < best.get_tile().x))
			)
		) {
			best = base;
			best_score = score;
		}
	}
	return best;
};

const get_attack_score = (attacker, defender) => {
	return combat_rules.get_attack_score(attacker, defender);
};

const get_attack_commitment_score = (tm, attacker, defender, player_id, units) => {
	const target_tile = defender.get_tile();
	let support = 0.0;
	let defense = 0.0;
	for (unit of units) {
		const def = unit.get_def();
		if (
			unit.owner == player_id &&
			def.offense > 0 &&
			!combat_rules.is_artillery(def) &&
			(!#is_defined(unit.is_immovable) || unit.is_immovable == false) &&
			unit.health >= RETREAT_HEALTH &&
			unit.movement > 0.0 &&
			tm.get_distance(unit.get_tile(), target_tile) <= ATTACK_SUPPORT_DISTANCE &&
			!is_triad_blocked(unit, target_tile)
		) {
			support += combat_rules.get_attack_powers(unit, defender).attack;
		} else if (unit.owner != player_id && unit.health > 0.0 && unit.get_tile() == target_tile) {
			defense += combat_rules.get_attack_powers(attacker, unit).defence;
		}
	}
	const total = support + defense;
	return total > 0.0 ? support / total : 0.0;
};

const can_commit_attack = (tm, attacker, defender, player_id, units) => {
	if (combat_rules.is_artillery(attacker.get_def())) {
		return true;
	}
	if (get_attack_score(attacker, defender) >= MIN_DIRECT_ATTACK_SCORE) {
		return true;
	}
	return #is_defined(tm) && #is_defined(units) &&
		get_attack_commitment_score(tm, attacker, defender, player_id, units) >= MIN_GROUP_ATTACK_SCORE;
};

const choose_attack_target = (attacker, player_id, tiles, tm, units, is_target_allowed) => {
	let best = null;
	let best_score = 0.0;
	const attacker_is_artillery = combat_rules.is_artillery(attacker.get_def());
	for (tile of tiles) {
		if (
			!attacker_is_artillery &&
			is_triad_blocked(attacker, tile)
		) {
			continue;
		}
		const unit = combat_rules.get_best_defender(attacker, tile);
		if (
			unit == null ||
			(#is_defined(is_target_allowed) && !is_target_allowed(unit.owner)) ||
			!can_commit_attack(tm, attacker, unit, player_id, units)
		) {
			continue;
		}
		const score = get_attack_score(attacker, unit);
		if (
			best == null ||
			score > best_score ||
			(
				score == best_score &&
				(
					tile.y < best.get_tile().y ||
					(tile.y == best.get_tile().y && tile.x < best.get_tile().x) ||
					(
						tile.y == best.get_tile().y &&
						tile.x == best.get_tile().x &&
						unit.id < best.id
					)
				)
			)
		) {
			best = unit;
			best_score = score;
		}
	}
	return best;
};

const get_assault_score = (tm, attacker, base, player_id, units) => {
	const base_tile = base.get_tile();
	if (
		base.get_owner().id == player_id ||
		is_triad_blocked(attacker, base_tile)
	) {
		return null;
	}
	let defense = 0.0;
	let support = 0.0;
	for (unit of units) {
		const def = unit.get_def();
		if (unit.owner != player_id && unit.get_tile() == base_tile) {
			defense += combat_rules.get_attack_powers(attacker, unit).defence;
		} else if (
			unit.owner == player_id &&
			can_threaten_tile(unit, base_tile) &&
			unit.health >= RETREAT_HEALTH &&
			tm.get_distance(unit.get_tile(), base_tile) <= ASSAULT_SUPPORT_DISTANCE
		) {
			support += #to_float(def.offense) * combat_rules.get_morale_multiplier(unit) * unit.health;
		}
	}
	const market_target_bonus =
		#typeof(base.has) == 'Callable' &&
		base.has('economic_victory_turn') && base.has('economic_victory_cost')
			? ECONOMIC_VICTORY_TARGET_BONUS
			: 0.0;
	return (
		0.0 - #to_float(tm.get_distance(attacker.get_tile(), base_tile)) -
		defense * ASSAULT_DEFENSE_WEIGHT +
		support * ASSAULT_SUPPORT_WEIGHT +
		#to_float(base.get_size()) * ASSAULT_BASE_SIZE_WEIGHT +
		market_target_bonus
	);
};

const choose_assault_target = (tm, attacker, player_id, bases, units) => {
	let best = null;
	let best_score = 0.0;
	for (base of bases) {
		const score = get_assault_score(tm, attacker, base, player_id, units);
		if (score == null) {
			continue;
		}
		const tile = base.get_tile();
		if (
			best == null ||
			score > best_score ||
			(
				score == best_score &&
				(tile.y < best.get_tile().y || (tile.y == best.get_tile().y && tile.x < best.get_tile().x))
			)
		) {
			best = base;
			best_score = score;
		}
	}
	return best;
};

return {
	find_nearest_friendly_base: find_nearest_friendly_base,
	get_repair_destination: get_repair_destination,
	can_threaten_tile: can_threaten_tile,
	get_required_garrison: get_required_garrison,
	get_garrison_count: get_garrison_count,
	get_force_power: get_force_power,
	get_reinforcement_score: get_reinforcement_score,
	choose_reinforcement_target: choose_reinforcement_target,
	get_attack_score: get_attack_score,
	get_attack_commitment_score: get_attack_commitment_score,
	can_commit_attack: can_commit_attack,
	choose_attack_target: choose_attack_target,
	get_assault_score: get_assault_score,
	choose_assault_target: choose_assault_target,
};
