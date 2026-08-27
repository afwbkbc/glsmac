const abilities = #include('../default/game/unit_abilities');

const unit_def = (ids) => { return {abilities: ids}; };
const unit = (ids) => {
	const def = unit_def(ids);
	return {get_def: () => { return def; }};
};

test.assert(!abilities.has(unit_def([]), 'CleanReactor'));
test.assert(!abilities.has({}, 'CleanReactor'));
test.assert(abilities.has(unit(['CleanReactor']), 'CleanReactor'));

test.assert(abilities.get_support_cost(unit_def([])) == 1);
test.assert(abilities.get_support_cost(unit(['CleanReactor'])) == 0);
test.assert(abilities.get_morale_bonus(unit_def([])) == 0);
test.assert(abilities.get_morale_bonus(unit_def(['HighMorale'])) == 1);
test.assert(abilities.get_police_effect(unit_def([])) == 1);
test.assert(abilities.get_police_effect(unit(['NonLethalMethods'])) == 2);

test.assert(abilities.get_terraforming_rate_multiplier(unit_def([]), 'farm') == 1.0);
test.assert(
	abilities.get_terraforming_rate_multiplier(unit_def(['SuperFormer']), 'farm') == 2.0
);
test.assert(
	abilities.get_terraforming_rate_multiplier(unit_def(['FungicideTanks']), 'farm') == 1.0
);
test.assert(
	abilities.get_terraforming_rate_multiplier(
		unit_def(['FungicideTanks']),
		'remove_fungus'
	) == 2.0
);
test.assert(
	abilities.get_terraforming_rate_multiplier(
		unit_def(['SuperFormer', 'FungicideTanks']),
		'remove_fungus'
	) == 4.0
);
