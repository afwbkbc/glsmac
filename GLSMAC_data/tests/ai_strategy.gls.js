const strategy = #include('../default/game/ai/strategy');

const desired = strategy.get_desired_base_count;

test.assert(desired(1, 20, 10, 2) == 1);
test.assert(desired(6, 20, 10, 2) == 1);
test.assert(desired(7, 20, 10, 2) == 2);
test.assert(desired(13, 20, 10, 2) == 3);
test.assert(desired(19, 20, 10, 2) == 4);
test.assert(desired(100, 20, 10, 2) == 4);

test.assert(desired(100, 20, 10, 7) == 1);
test.assert(desired(100, 40, 20, 7) == 4);
test.assert(desired(100, 80, 40, 7) == 17);

test.assert(desired(0, 20, 10, 0) == 1);

test.assert(!strategy.can_expand_safely(2, 0, 2, 0));
test.assert(strategy.can_expand_safely(2, 0, 3, 0));
test.assert(!strategy.can_expand_safely(2, 1, 3, 0));
test.assert(strategy.can_expand_safely(2, 1, 4, 0));
test.assert(!strategy.can_expand_safely(2, 0, 4, 1));

test.assert(strategy.get_gap_priority(1, 1) == 0);
test.assert(strategy.get_gap_priority(0, 1) == 75);
test.assert(strategy.get_gap_priority(0, 2) == 100);
test.assert(strategy.get_pressure_priority(1, 4) == 63);
test.assert(strategy.get_pressure_priority(0, 4) == 0);
test.assert(strategy.get_rival_pressure_priority(10.0, 0.0) == 0);
test.assert(strategy.get_rival_pressure_priority(10.0, 10.0) == 0);
test.assert(strategy.get_rival_pressure_priority(10.0, 11.0) == 0);
test.assert(strategy.get_rival_pressure_priority(10.0, 12.0) == 59);
test.assert(strategy.get_rival_pressure_priority(10.0, 20.0) == 75);
test.assert(strategy.get_rival_pressure_priority(0.0, 20.0) == 100);

const priorities = (
	bases,
	desired_bases,
	colonies,
	formers,
	combat,
	underdefended,
	stalled,
	unstable,
	income,
	own_power,
	rival_power
) => {
	return strategy.get_priorities({
		base_count: bases,
		desired_base_count: desired_bases,
		colony_count: colonies,
		former_count: formers,
		combat_count: combat,
		underdefended_bases: underdefended,
		growth_stalled_bases: stalled,
		unstable_bases: unstable,
		energy_income: income,
		own_combat_power: #is_defined(own_power) ? own_power : 0.0,
		strongest_rival_power: #is_defined(rival_power) ? rival_power : 0.0,
	});
};

const balanced = priorities(2, 2, 0, 2, 4, 0, 0, 0, 5);
test.assert(balanced.expansion == 0);
test.assert(balanced.terraforming == 0);
test.assert(balanced.defense == 0);
test.assert(balanced.military == 0);
test.assert(balanced.development == 100);

const strained = priorities(2, 4, 0, 0, 1, 1, 1, 1, 5);
test.assert(strained.expansion == 100);
test.assert(strained.terraforming == 100);
test.assert(strained.defense == 75);
test.assert(strained.military == 100);
test.assert(strained.growth == 75);
test.assert(strained.psych == 75);
test.assert(strained.development == 25);

const reserved_expansion = priorities(2, 3, 1, 2, 4, 0, 0, 0, 5);
test.assert(reserved_expansion.expansion == 0);
test.assert(reserved_expansion.development == 100);

const broke_but_stable = priorities(2, 2, 0, 2, 4, 0, 0, 0, 0);
test.assert(broke_but_stable.development == 75);

const outmatched = priorities(2, 2, 0, 2, 4, 0, 0, 0, 5, 10.0, 20.0);
test.assert(outmatched.rival_pressure == 75);
test.assert(outmatched.military == 75);
test.assert(outmatched.development == 82);

const dominant = priorities(2, 2, 0, 2, 4, 0, 0, 0, 5, 20.0, 10.0);
test.assert(dominant.rival_pressure == 0);
test.assert(dominant.military == 0);
test.assert(dominant.development == 100);

const immobile_force = strategy.get_priorities({
	base_count: 2,
	desired_base_count: 2,
	colony_count: 0,
	former_count: 2,
	combat_count: 4,
	mobile_combat_count: 0,
	underdefended_bases: 0,
	growth_stalled_bases: 0,
	unstable_bases: 0,
	energy_income: 5,
	own_combat_power: 20.0,
	strongest_rival_power: 10.0,
});
test.assert(immobile_force.mobility == 75);
test.assert(immobile_force.military == 75);
test.assert(immobile_force.development == 82);
