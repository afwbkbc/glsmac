const manifest = #include('../content/base_units');
const prototype_rules = #include('prototype_rules');

const index_components = (entries) => {
	let result = {};
	for (entry of entries) {
		result[entry.id] = entry;
	}
	return result;
};

const chassis = index_components(manifest.chassis);
const weapons = index_components(manifest.weapons);
const armors = index_components(manifest.armors);

const get_energy_credits = (player) => {
	return #typeof(player.get_energy_credits) == 'Callable'
		? player.get_energy_credits()
		: player.energy_credits;
};

const has_ability = (def, id) => {
	if (!#is_defined(def.abilities)) {
		return false;
	}
	for (ability of def.abilities) {
		if (ability == id) {
			return true;
		}
	}
	return false;
};

const is_available = (player, def) => {
	return (
		!#is_defined(def.required_technology) ||
		def.required_technology == '' ||
		player.has_technology(def.required_technology)
	) && !prototype_rules.is_prototype(player, def);
};

const is_compatible = (source, target) => {
	if (
		source.id == target.id ||
		(#is_defined(source.is_native) && source.is_native) ||
		(#is_defined(target.is_native) && target.is_native) ||
		source.chassis != target.chassis ||
		!#is_defined(chassis[source.chassis]) ||
		!#is_defined(weapons[source.weapon]) ||
		!#is_defined(weapons[target.weapon]) ||
		!#is_defined(armors[source.armor]) ||
		!#is_defined(armors[target.armor]) ||
		!#is_defined(target.mineral_cost) || target.mineral_cost <= 0
	) {
		return false;
	}
	const source_weapon = weapons[source.weapon];
	const target_weapon = weapons[target.weapon];
	if (
		source.weapon != target.weapon &&
		!(source_weapon.offense != 0 && target_weapon.offense != 0)
	) {
		return false;
	}
	if (
		target_weapon.cost < source_weapon.cost ||
		armors[target.armor].cost < armors[source.armor].cost ||
		has_ability(source, 'HeavyArtillery') != has_ability(target, 'HeavyArtillery')
	) {
		return false;
	}
	if (
		chassis[source.chassis].triad == 'air' &&
		has_ability(source, 'AirSuperiority') != has_ability(target, 'AirSuperiority')
	) {
		return false;
	}
	return true;
};

const find_definition = (game, id) => {
	for (def of game.get_um().get_unit_defs()) {
		if (def.id == id) {
			return def;
		}
	}
	return null;
};

const get_targets = (game, player, source) => {
	let result = [];
	for (target of game.get_um().get_unit_defs()) {
		if (is_compatible(source, target) && is_available(player, target)) {
			result :+target;
		}
	}
	return result;
};

const get_cost = (game, player, source, target) => {
	if (!is_compatible(source, target)) {
		return null;
	}
	const weapon_rise = #max(
		0,
		weapons[target.weapon].cost - weapons[source.weapon].cost
	);
	const armor_rise = #max(
		0,
		armors[target.armor].cost - armors[source.armor].cost
	);
	const new_rows = #ceil(#to_float(target.mineral_cost) / 10.0);
	let cost = (weapon_rise + armor_rise + new_rows) * 10;
	const get_project_effects = #is_defined(game.get)
		? game.get('f_project_get_player_effects')
		: #undefined;
	if (#is_defined(get_project_effects)) {
		const effects = get_project_effects(player);
		if (
			#is_defined(effects.unit_upgrade_cost_multiplier) &&
			effects.unit_upgrade_cost_multiplier < 1.0
		) {
			cost = #floor(
				#to_float(cost) * effects.unit_upgrade_cost_multiplier / 10.0
			) * 10;
		}
	}
	return cost;
};

const get_unit_error = (game, unit, caller) => {
	if (#typeof(unit) != 'Object' || #typeof(unit.get_def) != 'Callable') {
		return 'Unit upgrade requires a unit';
	}
	if (unit.owner != caller) {
		return 'A unit can only be upgraded by its owner';
	}
	if (game.is_turn_complete(caller)) {
		return 'Player has already completed this turn';
	}
	if (unit.health <= 0.0) {
		return 'Destroyed unit cannot be upgraded';
	}
	if (#is_defined(unit.transport_id) && unit.transport_id > 0) {
		return 'Embarked unit must disembark before upgrading';
	}
	if (#is_defined(unit.get_cargo) && #sizeof(unit.get_cargo()) > 0) {
		return 'A transport carrying units cannot be upgraded';
	}
	if (unit.terraforming != 'none') {
		return 'Unit cannot be upgraded while terraforming';
	}
	if (unit.moved_this_turn || unit.movement <= 0.0) {
		return 'Unit must not have used movement this turn';
	}
};

const get_error = (game, unit, caller, target_id) => {
	const unit_error = get_unit_error(game, unit, caller);
	if (#is_defined(unit_error)) {
		return unit_error;
	}
	if (#typeof(target_id) != 'String' || target_id == '') {
		return 'Unit upgrade requires a target design';
	}
	const target = find_definition(game, target_id);
	if (target == null) {
		return 'Unknown unit upgrade target';
	}
	const source = unit.get_def();
	if (!is_compatible(source, target)) {
		return 'Target design is not a legal upgrade for this unit';
	}
	const player = game.get_player(caller);
	if (!is_available(player, target)) {
		return prototype_rules.is_prototype(player, target)
			? 'Target design must be prototyped before units can upgrade to it'
			: 'Target design technology is not available';
	}
	const cost = get_cost(game, player, source, target);
	if (get_energy_credits(player) < cost) {
		return 'Not enough energy credits to upgrade this unit';
	}
};

const get_combat_value = (def) => {
	if (def.offense <= 0) {
		return 0.0;
	}
	return #to_float(def.offense * 2 + def.defense) +
		def.movement_per_turn * 0.5 +
		(#is_defined(def.abilities) ? #to_float(#sizeof(def.abilities)) * 0.25 : 0.0);
};

const choose_ai_target = (game, player, unit) => {
	if (#is_defined(get_unit_error(game, unit, player.id))) {
		return null;
	}
	const source = unit.get_def();
	const source_value = get_combat_value(source);
	if (source_value <= 0.0) {
		return null;
	}
	const energy_credits = get_energy_credits(player);
	const reserve = #max(20, #floor(#to_float(energy_credits) * 0.4));
	const budget = #max(0, energy_credits - reserve);
	let best = null;
	let best_value = source_value;
	let best_cost = 0;
	for (target of get_targets(game, player, source)) {
		const value = get_combat_value(target);
		const cost = get_cost(game, player, source, target);
		if (
			value <= source_value * 1.15 || cost > budget ||
			(
				best != null &&
				(value < best_value || (value == best_value && cost >= best_cost))
			)
		) {
			continue;
		}
		best = target;
		best_value = value;
		best_cost = cost;
	}
	return best;
};

return {
	has_ability: has_ability,
	get_energy_credits: get_energy_credits,
	is_available: is_available,
	is_compatible: is_compatible,
	find_definition: find_definition,
	get_targets: get_targets,
	get_cost: get_cost,
	get_unit_error: get_unit_error,
	get_error: get_error,
	get_combat_value: get_combat_value,
	choose_ai_target: choose_ai_target,
};
