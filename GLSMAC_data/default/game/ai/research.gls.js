const get_priority = (context, name, fallback) => {
	return #is_defined(context.priorities) && #is_defined(context.priorities[name])
		? context.priorities[name]
		: fallback;
};

const score_technology = (technology, unit_defs, facility_defs, context) => {
	let score = 1000 - technology.cost;
	const development_priority = get_priority(context, 'development', 100);
	const commerce_bonus = #is_defined(technology.commerce_bonus)
		? technology.commerce_bonus
		: 0;
	score += commerce_bonus * (2000 + development_priority * 40);
	for (def of unit_defs) {
		if (def.required_technology != technology.id) {
			continue;
		}
		if (def.can_found_base) {
			score += 5000 + get_priority(
				context,
				'expansion',
				context.needs_colony ? 100 : 0
			) * 550;
		}
		if (def.can_terraform) {
			score += 5000 + get_priority(
				context,
				'terraforming',
				context.needs_former ? 100 : 0
			) * 450;
		}
		if (def.offense > 0) {
			score += 5000 + get_priority(
				context,
				'military',
				context.needs_military ? 100 : 0
			) * 250;
			score += def.offense * 1000 + def.defense * 500;
			score += #round(def.movement_per_turn * 100.0);
		}
	}
	for (def of facility_defs) {
		if (def.required_technology != technology.id) {
			continue;
		}
		const growth_priority = get_priority(
			context,
			'growth',
			context.needs_growth ? 100 : 0
		);
		const psych_priority = get_priority(
			context,
			'psych',
			context.needs_psych ? 100 : 0
		);
		const defense_priority = get_priority(context, 'defense', 0);
		score += 1000 + development_priority * 10;
		score += def.nutrient_bonus * (500 + growth_priority * 45);
		score += def.growth_rating_bonus * (2000 + growth_priority * 300);
		score += def.mineral_bonus * 1500 + def.energy_bonus * 1000;
		score += def.psych_bonus * (100 + psych_priority * 119);
		score += #round(
			def.research_multiplier * #to_float(context.base_labs) *
				#to_float(1000 + development_priority * 10)
		);
		score += def.research_bonus * (3000 + development_priority * 200);
		score += #round(
			#max(def.defense_multiplier - 1.0, 0.0) *
				#to_float(5000 + defense_priority * 400)
		);
		score += #round(
			(
				#max(def.water_defense_multiplier - 1.0, 0.0) +
				#max(def.air_defense_multiplier - 1.0, 0.0)
			) * #to_float(5000 + defense_priority * 400)
		);
		score += #round(
			def.economy_multiplier * #to_float(5000 + development_priority * 300)
		);
		score += (#is_defined(def.efficiency_rating_bonus)
			? def.efficiency_rating_bonus
			: 0) * (5000 + development_priority * 300);
		score += #round(
			def.mineral_multiplier * #to_float(5000 + development_priority * 300)
		);
		score += #round(
			def.psych_multiplier * #to_float(1000 + psych_priority * 100)
		);
		if (def.population_limit > 0 && context.needs_growth) {
			score += 10000 + growth_priority * 500;
		}
		score += (
			#max(0 - def.drone_modifier, 0) + def.talent_bonus -
				#max(def.drone_modifier, 0)
		) * (1000 + psych_priority * 100);
		if (def.suppress_psych && context.needs_psych) {
			score += 50000;
		}
		score += (
			def.unit_morale_bonus + def.unit_morale_land_bonus +
			def.unit_morale_water_bonus + def.unit_morale_air_bonus +
			def.native_lifecycle_bonus
		) * (
			5000 + get_priority(context, 'military', context.needs_military ? 100 : 0) * 250
		);
		score += (#is_defined(def.defender_morale_minimum)
			? def.defender_morale_minimum
			: 0) * (5000 + defense_priority * 250);
		if (#is_defined(def.prototype_cost_waiver) && def.prototype_cost_waiver) {
			score += 5000 + development_priority * 200;
		}
	}
	return score;
};

const choose = (technologies, unit_defs, facility_defs, context) => {
	let best = null;
	let best_score = 0;
	for (technology of technologies) {
		const score = score_technology(technology, unit_defs, facility_defs, context);
		if (
			best == null ||
			score > best_score ||
			(score == best_score && technology.id < best.id)
		) {
			best = technology;
			best_score = score;
		}
	}
	return best;
};

const choose_id = (technology_ids, get_definition, unit_defs, facility_defs, context) => {
	let best_id = '';
	let best_score = 0;
	for (technology_id of technology_ids) {
		const technology = get_definition(technology_id);
		const score = score_technology(technology, unit_defs, facility_defs, context);
		if (
			best_id == '' ||
			score > best_score ||
			(score == best_score && technology_id < best_id)
		) {
			best_id = technology_id;
			best_score = score;
		}
	}
	return best_id;
};

return {
	score_technology: score_technology,
	choose: choose,
	choose_id: choose_id,
};
