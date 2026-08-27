const TILES_PER_BASE = 24;
const TURNS_PER_EXPANSION = 6;
const UNITS_PER_BASE = 2;
const RIVAL_POWER_MARGIN = 1.1;

const get_gap_priority = (current, target) => {
	const missing = #max(target - current, 0);
	return missing == 0 ? 0 : #min(100, 50 + missing * 25);
};

const get_pressure_priority = (affected, total) => {
	if (affected <= 0 || total <= 0) {
		return 0;
	}
	return #min(
		100,
		50 + #ceil(
			#to_float(affected * 50) /
			#to_float(total)
		)
	);
};

const get_rival_pressure_priority = (own_power, rival_power) => {
	if (rival_power <= own_power * RIVAL_POWER_MARGIN || rival_power <= 0.0) {
		return 0;
	}
	return #min(
		100,
		50 + #ceil(
			(rival_power - own_power) * 50.0 /
			rival_power
		)
	);
};

const get_desired_base_count = (turn, map_width, map_height, player_count) => {
	const competitors = #max(player_count, 1);
	const map_capacity = #max(
		1,
		#floor(
			#to_float(map_width * map_height) /
			#to_float(competitors * TILES_PER_BASE)
		)
	);
	const expansion_tempo = 1 + #floor(
		#to_float(#max(turn - 1, 0)) /
		#to_float(TURNS_PER_EXPANSION)
	);
	return #min(map_capacity, expansion_tempo);
};

const can_expand_safely = (base_count, colony_count, combat_count, underdefended_bases) => {
	return (
		underdefended_bases <= 0 &&
		combat_count > base_count + colony_count
	);
};

const get_priorities = (context) => {
	const bases = #max(context.base_count, 0);
	const expansion = get_gap_priority(
		bases + #max(context.colony_count, 0),
		#max(context.desired_base_count, 1)
	);
	const terraforming = get_gap_priority(
		#max(context.former_count, 0),
		bases
	);
	const defense = get_pressure_priority(
		#max(context.underdefended_bases, 0),
		bases
	);
	const own_power = #is_defined(context.own_combat_power)
		? #max(context.own_combat_power, 0.0)
		: 0.0;
	const rival_power = #is_defined(context.strongest_rival_power)
		? #max(context.strongest_rival_power, 0.0)
		: 0.0;
	const rival_pressure = get_rival_pressure_priority(own_power, rival_power);
	const mobile_combat_count = #is_defined(context.mobile_combat_count)
		? #max(context.mobile_combat_count, 0)
		: (bases > 0 ? 1 : 0);
	const mobility = get_gap_priority(mobile_combat_count, bases > 0 ? 1 : 0);
	const military = #max(
		#max(
			#max(defense, mobility),
			get_gap_priority(
				#max(context.combat_count, 0),
				bases * UNITS_PER_BASE
			)
		),
		rival_pressure
	);
	const growth = get_pressure_priority(
		#max(context.growth_stalled_bases, 0),
		bases
	);
	const psych = get_pressure_priority(
		#max(context.unstable_bases, 0),
		bases
	);
	const operational_pressure = #max(
		#max(expansion, terraforming),
		#max(defense, #max(growth, psych))
	);
	let development = #max(
		20,
		100 - #floor(#to_float(operational_pressure) * 0.5) -
			#floor(#to_float(military) * 0.25)
	);
	if (context.energy_income <= 0) {
		development = #max(development - 25, 0);
	}
	return {
		expansion: expansion,
		terraforming: terraforming,
		defense: defense,
		mobility: mobility,
		rival_pressure: rival_pressure,
		military: military,
		growth: growth,
		psych: psych,
		development: development,
	};
};

return {
	get_desired_base_count: get_desired_base_count,
	can_expand_safely: can_expand_safely,
	get_gap_priority: get_gap_priority,
	get_pressure_priority: get_pressure_priority,
	get_rival_pressure_priority: get_rival_pressure_priority,
	get_priorities: get_priorities,
};
