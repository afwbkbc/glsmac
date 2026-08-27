const get_def = (unit_or_def) => {
	return #is_defined(unit_or_def.get_def) ? unit_or_def.get_def() : unit_or_def;
};

const has = (unit_or_def, id) => {
	const def = get_def(unit_or_def);
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

const get_support_cost = (unit_or_def) => {
	return has(unit_or_def, 'CleanReactor') ? 0 : 1;
};

const get_morale_bonus = (unit_or_def) => {
	return has(unit_or_def, 'HighMorale') ? 1 : 0;
};

const get_police_effect = (unit_or_def) => {
	return has(unit_or_def, 'NonLethalMethods') ? 2 : 1;
};

const get_terraforming_rate_multiplier = (unit_or_def, order_id) => {
	let multiplier = has(unit_or_def, 'SuperFormer') ? 2.0 : 1.0;
	if (order_id == 'remove_fungus' && has(unit_or_def, 'FungicideTanks')) {
		multiplier *= 2.0;
	}
	return multiplier;
};

return {
	has: has,
	get_support_cost: get_support_cost,
	get_morale_bonus: get_morale_bonus,
	get_police_effect: get_police_effect,
	get_terraforming_rate_multiplier: get_terraforming_rate_multiplier,
};
