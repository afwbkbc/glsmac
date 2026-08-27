const unit_abilities = #include('unit_abilities');

const is_artillery = (def) => {
	return #is_defined(def.is_artillery) ? def.is_artillery : def.id == 'SporeLauncher';
};

const has_ability = (def, id) => { return unit_abilities.has(def, id); };

const get_transport_id = (unit) => {
	return #is_defined(unit.transport_id) ? unit.transport_id : 0;
};

const is_airbase_tile = (tile) => {
	return (#is_defined(tile.get_base) && tile.get_base() != null) || (
		#is_defined(tile.terraforming) &&
		#is_defined(tile.terraforming.airbase) &&
		tile.terraforming.airbase
	);
};

const is_naval_base_tile = (tile) => {
	if (!#is_defined(tile.get_base) || tile.get_base() == null) {
		return false;
	}
	if (!#is_defined(tile.get_surrounding_tiles)) {
		return true;
	}
	for (nearby of tile.get_surrounding_tiles()) {
		if (#is_defined(nearby.is_water) && nearby.is_water) {
			return true;
		}
	}
	return false;
};

const is_air_unit_in_flight = (unit) => {
	if (!#is_defined(unit.is_air) || !unit.is_air) {
		return false;
	}
	const def = unit.get_def();
	return (
		(!#is_defined(def.chassis) || def.chassis != 'Copter') &&
		!is_airbase_tile(unit.get_tile())
	);
};

const is_triad_attack_blocked = (attacker, target_tile) => {
	const attacker_def = attacker.get_def();
	if (is_artillery(attacker_def)) {
		return false;
	}
	if (#is_defined(attacker.is_land) && attacker.is_land) {
		const amphibious = has_ability(attacker_def, 'AmphibiousPods');
		if (get_transport_id(attacker) > 0) {
			return !amphibious || !target_tile.is_land;
		}
		const source_tile = attacker.get_tile();
		if (#is_defined(source_tile.is_water) && source_tile.is_water) {
			return !amphibious || (
				!#is_defined(source_tile.get_base) || source_tile.get_base() == null
			);
		}
		if (#is_defined(target_tile.is_water) && target_tile.is_water) {
			return !amphibious || (
				(
					!#is_defined(source_tile.get_base) || source_tile.get_base() == null
				) && (
					!#is_defined(target_tile.get_base) || target_tile.get_base() == null
				)
			);
		}
	}
	return (
		#is_defined(attacker.is_water) && attacker.is_water &&
		#is_defined(target_tile.is_land) && target_tile.is_land &&
		!is_naval_base_tile(target_tile)
	);
};

const can_attack_target = (attacker, defender) => {
	if (is_air_unit_in_flight(defender)) {
		const attacker_def = attacker.get_def();
		return has_ability(attacker_def, 'AirSuperiority') && !(
			#is_defined(attacker.is_land) && attacker.is_land &&
			get_transport_id(attacker) > 0 &&
			!has_ability(attacker_def, 'AmphibiousPods')
		);
	}
	return !is_triad_attack_blocked(attacker, defender.get_tile());
};

const can_advance_after_combat = (attacker, target_tile) => {
	if (target_tile == null) {
		return false;
	}
	if (
		#is_defined(attacker.is_land) && attacker.is_land &&
		#is_defined(target_tile.is_water) && target_tile.is_water
	) {
		return target_tile.get_base() != null;
	}
	if (
		#is_defined(attacker.is_water) && attacker.is_water &&
		#is_defined(target_tile.is_land) && target_tile.is_land
	) {
		return target_tile.get_base() != null;
	}
	return true;
};

const get_morale_multiplier = (unit, bonus) => {
	const value_bonus = #is_defined(bonus) ? bonus : 0;
	return 0.75 + #to_float(unit.morale + value_bonus) * 0.125;
};

const get_base_defender_morale_bonus = (defender, game) => {
	if (!#is_defined(defender.get_tile)) {
		return 0;
	}
	const base = defender.get_tile().get_base();
	if (
		base == null ||
		!#is_defined(base.get_owner) ||
		!#is_defined(base.get_facilities) ||
		base.get_owner().id != defender.owner
	) {
		return 0;
	}
	const resolver = #is_defined(game) && #is_defined(game.get)
		? game.get('f_base_get_effective_facilities')
		: #undefined;
	const facilities = #is_defined(resolver) ? resolver(base) : base.get_facilities();
	let result = 0;
	for (facility of facilities) {
		result += #is_defined(facility.defender_morale_bonus)
			? facility.defender_morale_bonus
			: 0;
	}
	return result;
};

const get_base_defender_morale_minimum = (defender, game) => {
	if (!#is_defined(defender.get_tile)) {
		return 0;
	}
	const base = defender.get_tile().get_base();
	if (
		base == null ||
		!#is_defined(base.get_owner) ||
		!#is_defined(base.get_facilities) ||
		base.get_owner().id != defender.owner
	) {
		return 0;
	}
	const resolver = #is_defined(game) && #is_defined(game.get)
		? game.get('f_base_get_effective_facilities')
		: #undefined;
	const facilities = #is_defined(resolver) ? resolver(base) : base.get_facilities();
	let result = 0;
	for (facility of facilities) {
		result = #max(
			result,
			#is_defined(facility.defender_morale_minimum)
				? facility.defender_morale_minimum
				: 0
		);
	}
	return result;
};

const get_social_morale_bonus = (unit, game, defending) => {
	const def = unit.get_def();
	if (
		!#is_defined(unit.get_owner) ||
		(#is_defined(def.is_native) && def.is_native)
	) {
		return 0;
	}
	const resolver = #is_defined(game) && #is_defined(game.get)
		? game.get('f_social_get_morale_bonus')
		: #undefined;
	const social_bonus = #is_defined(resolver)
		? resolver(unit.get_owner(), defending)
		: 0;
	const facility_minimum = defending
		? get_base_defender_morale_minimum(unit, game)
		: 0;
	return facility_minimum > 0 ? #max(social_bonus, facility_minimum) : social_bonus;
};

const get_base_defense_multiplier = (defender, attacker, game) => {
	const base = defender.get_tile().get_base();
	if (
		base == null ||
		!#is_defined(base.get_owner) ||
		!#is_defined(base.get_facilities) ||
		base.get_owner().id != defender.owner
	) {
		return 1.0;
	}
	let multiplier = 1.0;
	const resolver = #is_defined(game) ? game.get('f_base_get_effective_facilities') : #undefined;
	const facilities = #is_defined(resolver) ? resolver(base) : base.get_facilities();
	for (facility of facilities) {
		const facility_defense = #is_defined(facility.defense_multiplier)
			? facility.defense_multiplier
			: 1.0;
		multiplier += #max(facility_defense - 1.0, 0.0);
		if (#is_defined(attacker) && attacker.is_water) {
			const scoped_multiplier = #is_defined(facility.water_defense_multiplier)
				? facility.water_defense_multiplier
				: 1.0;
			multiplier += #max(scoped_multiplier - 1.0, 0.0);
		} else if (#is_defined(attacker) && attacker.is_air) {
			const scoped_multiplier = #is_defined(facility.air_defense_multiplier)
				? facility.air_defense_multiplier
				: 1.0;
			multiplier += #max(scoped_multiplier - 1.0, 0.0);
		}
	}
	return multiplier;
};

const get_project_effects = (unit, game) => {
	if (!#is_defined(game) || !#is_defined(game.get)) {
		return {
			psi_attack_multiplier: 1.0,
			psi_defense_multiplier: 1.0,
			native_fungus_combat: false,
		};
	}
	const resolver = game.get('f_project_get_player_effects');
	return #is_defined(resolver)
		? resolver(unit.get_owner())
		: {
			psi_attack_multiplier: 1.0,
			psi_defense_multiplier: 1.0,
			native_fungus_combat: false,
		};
};

const get_combat_powers = (attacker, defender, game) => {
	const attacker_def = attacker.get_def();
	const defender_def = defender.get_def();
	let attack_strength = #to_float(attacker_def.offense);
	let defence_strength = #to_float(defender_def.defense);
	const is_psi_attack = #is_defined(attacker_def.is_psi_attack)
		? attacker_def.is_psi_attack
		: attacker_def.is_native;
	const is_psi_defense = #is_defined(defender_def.is_psi_defense)
		? defender_def.is_psi_defense
		: defender_def.is_native;
	const is_psi_combat = is_psi_attack || is_psi_defense;
	const is_air_to_air =
		!is_psi_combat && #is_defined(attacker.is_air) && attacker.is_air &&
		is_air_unit_in_flight(defender);
	if (is_psi_combat) {
		attack_strength = defender.is_land ? 3.0 : 1.0;
		defence_strength = defender.is_land ? 2.0 : 1.0;
	} else if (is_air_to_air) {
		defence_strength = #to_float(defender_def.offense);
	}

	let attack_modifier = 1.0;
	let defence_modifier = 1.0;
	const defender_tile = defender.get_tile();
	if (
		has_ability(attacker_def, 'AirSuperiority') &&
		#is_defined(attacker.is_air) && attacker.is_air
	) {
		if (is_air_unit_in_flight(defender)) {
			attack_modifier *= 2.0;
		} else {
			attack_modifier *= 0.5;
		}
	}
	if (defender_tile.rockiness >= 3) {
		defence_modifier += 0.5;
	}
	if (defender_tile.features.xenofungus) {
		if (
			attacker_def.is_native ||
			get_project_effects(attacker, game).native_fungus_combat
		) {
			attack_modifier += 0.5;
		} else {
			defence_modifier += 0.5;
		}
	}
	if (defender_tile.get_base() != null || defender_tile.terraforming.bunker) {
		defence_modifier += 0.25;
	}
	if (!is_psi_combat) {
		if (
			#is_defined(attacker.is_air) && attacker.is_air &&
			has_ability(defender_def, 'AAATracking')
		) {
			defence_modifier *= 2.0;
		}
		if (
			#is_defined(attacker.is_land) && attacker.is_land &&
			#is_defined(attacker_def.movement_per_turn) &&
			attacker_def.movement_per_turn > 1.0 &&
			has_ability(defender_def, 'CommJammer')
		) {
			defence_modifier *= 1.5;
		}
		if (!has_ability(attacker_def, 'BlinkDisplacer')) {
			defence_modifier *= get_base_defense_multiplier(defender, attacker, game);
		}
	} else {
		if (has_ability(attacker_def, 'EmpathSong')) {
			attack_modifier *= 1.5;
		}
		if (has_ability(defender_def, 'HypnoticTrance')) {
			defence_modifier *= 1.5;
		}
		attack_modifier *= get_project_effects(attacker, game).psi_attack_multiplier;
		defence_modifier *= get_project_effects(defender, game).psi_defense_multiplier;
	}
	if (attacker.is_land && !attacker_def.is_native && attacker.movement < 1.0) {
		attack_modifier *= attacker.movement;
	}
	return {
		attack: attack_strength * get_morale_multiplier(
			attacker,
			get_social_morale_bonus(attacker, game, false)
		) * attacker.health * attack_modifier,
		defence: defence_strength * get_morale_multiplier(
			defender,
			get_base_defender_morale_bonus(defender, game) +
				get_social_morale_bonus(defender, game, true)
		) * defender.health * defence_modifier,
	};
};

const get_artillery_powers = (attacker, defender, game) => {
	const attacker_def = attacker.get_def();
	const defender_def = defender.get_def();
	return {
		attack: #to_float(attacker_def.offense) * get_morale_multiplier(
			attacker,
			get_social_morale_bonus(attacker, game, false)
		) * attacker.health,
		defence: #to_float(
			is_artillery(defender_def) ? defender_def.offense : defender_def.defense
		) * get_morale_multiplier(
			defender,
			get_base_defender_morale_bonus(defender, game) +
				get_social_morale_bonus(defender, game, true)
		) * defender.health,
	};
};

const get_attack_powers = (attacker, defender, game) => {
	return is_artillery(attacker.get_def())
		? get_artillery_powers(attacker, defender, game)
		: get_combat_powers(attacker, defender, game);
};

const get_attack_score = (attacker, defender, game) => {
	const powers = get_attack_powers(attacker, defender, game);
	const total = powers.attack + powers.defence;
	return total > 0.0 ? powers.attack / total : 0.0;
};

const get_best_defender = (attacker, tile, game) => {
	let best = null;
	let best_attack_score = 2.0;
	for (defender of tile.get_units()) {
		if (
			defender.owner == attacker.owner || defender.health <= 0.0 ||
			!can_attack_target(attacker, defender)
		) {
			continue;
		}
		const attack_score = get_attack_score(attacker, defender, game);
		if (
			best == null ||
			attack_score < best_attack_score ||
			(attack_score == best_attack_score && defender.id < best.id)
		) {
			best = defender;
			best_attack_score = attack_score;
		}
	}
	return best;
};

return {
	is_artillery: is_artillery,
	has_ability: has_ability,
	is_airbase_tile: is_airbase_tile,
	is_naval_base_tile: is_naval_base_tile,
	is_air_unit_in_flight: is_air_unit_in_flight,
	is_triad_attack_blocked: is_triad_attack_blocked,
	can_attack_target: can_attack_target,
	can_advance_after_combat: can_advance_after_combat,
	get_morale_multiplier: get_morale_multiplier,
	get_base_defender_morale_bonus: get_base_defender_morale_bonus,
	get_base_defender_morale_minimum: get_base_defender_morale_minimum,
	get_social_morale_bonus: get_social_morale_bonus,
	get_base_defense_multiplier: get_base_defense_multiplier,
	get_combat_powers: get_combat_powers,
	get_artillery_powers: get_artillery_powers,
	get_attack_powers: get_attack_powers,
	get_attack_score: get_attack_score,
	get_best_defender: get_best_defender,
};
