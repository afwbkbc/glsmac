const PSYCH_ALLOCATION = 0.2;
const LABS_ALLOCATION = 0.4;

const get_effective_facilities = (game, base) => {
	const resolver = game.get('f_base_get_effective_facilities');
	return #is_defined(resolver) ? resolver(base) : base.get_facilities();
};

const get_project_effects = (game, base) => {
	const resolver = game.get('f_project_get_effects');
	return #is_defined(resolver) ? resolver(base) : {};
};

const get_efficiency_rating = (game, base) => {
	const resolver = game.get('f_social_get_ratings');
	const ratings = #is_defined(resolver) ? resolver(base.get_owner()) : {effic: 0};
	let facility_bonus = 0;
	for (facility of get_effective_facilities(game, base)) {
		facility_bonus += #is_defined(facility.efficiency_rating_bonus)
			? facility.efficiency_rating_bonus
			: 0;
	}
	return ratings.effic + facility_bonus;
};

const get_headquarters_distance = (game, base) => {
	let distance = 16;
	let found = false;
	const owner = base.get_owner();
	for (candidate of game.get_bm().get_bases()) {
		if (
			candidate.get_owner().id == owner.id &&
			candidate.has_facility('Headquarters')
		) {
			const candidate_distance = game.get_tm().get_distance(
				base.get_tile(),
				candidate.get_tile()
			);
			if (!found || candidate_distance < distance) {
				found = true;
				distance = candidate_distance;
			}
		}
	}
	return distance;
};

const get_base_energy = (game, base) => {
	const gross = #max(base.get_intake().ENERGY, 0);
	const efficiency = get_efficiency_rating(game, base);
	const distance = get_headquarters_distance(game, base);
	const denominator = 64 - ((4 - efficiency) * 8);
	const inefficiency = denominator <= 0
		? gross
		: #min(
			gross,
			#floor(#to_float(gross * distance) / #to_float(denominator))
		);
	return {
		gross: gross,
		inefficiency: inefficiency,
		net: gross - inefficiency,
		efficiency: efficiency,
		distance: distance,
		denominator: denominator,
	};
};

const get_base_economy_multiplier = (game, base) => {
	let result = 0.0;
	for (facility of get_effective_facilities(game, base)) {
		if (#is_defined(facility.economy_multiplier)) {
			result += facility.economy_multiplier;
		}
	}
	const project_effects = get_project_effects(game, base);
	return result + (
		#is_defined(project_effects.economy_multiplier)
			? project_effects.economy_multiplier
			: 0.0
	);
};

const get_base_allocation = (game, base) => {
	const consumption = base.get_consumption();
	const energy = get_base_energy(game, base);
	const total_energy = energy.net - consumption.ENERGY;
	const energy_surplus = #max(total_energy, 0);
	const labs = game.get('f_technology_get_base_labs')(base);
	const psych = #round(#to_float(energy_surplus) * PSYCH_ALLOCATION);
	let psych_bonus = 0;
	let psych_multiplier = 0.0;
	for (facility of get_effective_facilities(game, base)) {
		psych_bonus += facility.psych_bonus;
		psych_multiplier += #is_defined(facility.psych_multiplier)
			? facility.psych_multiplier
			: 0.0;
	}
	psych_bonus += #ceil(#to_float(psych) * psych_multiplier);
	const economy_value = total_energy - labs.value - psych;
	const economy_bonus = #ceil(
		#to_float(#max(economy_value, 0)) * get_base_economy_multiplier(game, base)
	);
	return {
		economy: {
			allocation: 1.0 - labs.allocation - PSYCH_ALLOCATION,
			value: economy_value,
			bonus: economy_bonus,
		},
		labs: labs,
		psych: {
			allocation: PSYCH_ALLOCATION,
			value: psych,
			bonus: psych_bonus,
		},
	};
};

const get_base_economy = (game, base) => {
	const consumption = base.get_consumption();
	const total_energy = get_base_energy(game, base).net - consumption.ENERGY;
	const energy_surplus = #max(total_energy, 0);
	const labs_resolver = game.get('f_technology_get_base_labs_value');
	const labs = #is_defined(labs_resolver)
		? labs_resolver(base)
		: #round(#to_float(energy_surplus) * LABS_ALLOCATION);
	const psych = #round(#to_float(energy_surplus) * PSYCH_ALLOCATION);
	const value = total_energy - labs - psych;
	const bonus = #ceil(
		#to_float(#max(value, 0)) * get_base_economy_multiplier(game, base)
	);
	return value + bonus;
};

const get_ranked_bases = (game, player) => {
	let ranked = [];
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id != player.id) {
			continue;
		}
		ranked :+{
			base: base,
			energy: get_base_energy(game, base).net,
			economy: get_base_economy(game, base),
		};
		let index = #sizeof(ranked) - 1;
		while (index > 0) {
			const current = ranked[index];
			const previous = ranked[index - 1];
			if (
				current.energy < previous.energy ||
				(current.energy == previous.energy && current.base.id > previous.base.id)
			) {
				break;
			}
			ranked[index] = previous;
			ranked[index - 1] = current;
			index--;
		}
	}
	return ranked;
};

const get_commerce_technology = (game, player) => {
	let result = 0;
	const definition_resolver = game.get('f_technology_get_definition');
	for (id of player.get_research_state().technologies) {
		const definition = definition_resolver(id);
		if (definition != null) {
			result += definition.commerce_bonus;
		}
	}
	const social_resolver = game.get('f_social_get_commerce_bonus');
	if (#is_defined(social_resolver)) {
		result += social_resolver(player);
	}
	return result;
};

const get_player_commerce_ledger = (game, player) => {
	let ledger = {};
	const own_bases = get_ranked_bases(game, player);
	for (entry of own_bases) {
		ledger['b' + #to_string(entry.base.id)] = {total: 0, partners: []};
	}
	const faction = player.get_faction();
	if (faction.is_progenitor || player.get_sanction_turns() > 0) {
		return ledger;
	}
	const total_resolver = game.get('f_technology_get_total_commerce_bonus');
	const total_technology = total_resolver();
	const commerce_technology = get_commerce_technology(game, player);
	for (partner of game.get_players()) {
		if (
			partner.id == player.id || partner.get_faction().is_progenitor ||
			partner.get_sanction_turns() > 0
		) {
			continue;
		}
		const relation = player.get_diplomatic_relation(partner);
		if (
			(relation != 'treaty' && relation != 'pact') ||
			partner.get_diplomatic_relation(player) != relation
		) {
			continue;
		}
		const partner_bases = get_ranked_bases(game, partner);
		const pair_count = #min(#sizeof(own_bases), #sizeof(partner_bases));
		let index = 0;
		while (index < pair_count) {
			const own = own_bases[index];
			const other = partner_bases[index];
			let pair_value = #ceil(
				#to_float(#max(own.economy + other.economy, 0)) / 8.0
			);
			const has_trade_pact = game.get('f_council_has_global_trade_pact');
			if (#is_defined(has_trade_pact) && has_trade_pact()) {
				pair_value *= 2;
			}
			let value = #floor(
				#to_float(pair_value * (commerce_technology + 1)) /
				#to_float(total_technology + 1)
			);
			if (relation == 'treaty') {
				value = #floor(#to_float(value) / 2.0);
			}
			const is_governor = game.get('f_council_is_governor');
			if (#is_defined(is_governor) && is_governor(player)) {
				value++;
			}
			const result = ledger['b' + #to_string(own.base.id)];
			result.total = result.total + value;
			let partners = result.partners;
			partners :+{
				player_id: partner.id,
				player_name: partner.name,
				relation: relation,
				value: value,
			};
			result.partners = partners;
			index++;
		}
	}
	return ledger;
};

const get_base_commerce = (game, base) => {
	const ledger = get_player_commerce_ledger(game, base.get_owner());
	const key = 'b' + #to_string(base.id);
	return #is_defined(ledger[key]) ? ledger[key] : {total: 0, partners: []};
};

const get_player_commerce = (game, player) => {
	const ledger = get_player_commerce_ledger(game, player);
	let result = 0;
	for (key in ledger) {
		result += ledger[key].total;
	}
	return result;
};

const get_base_psych = (game, base) => {
	const psych = get_base_allocation(game, base).psych;
	return psych.value + psych.bonus;
};

const get_base_stockpile_energy = (game, base) => {
	if (!#is_defined(base.get_production)) {
		return 0;
	}
	const production = base.get_production();
	if (
		!#is_defined(production) ||
		production.production_kind != 'facility' ||
		!#is_defined(production.mineral_to_energy_divisor) ||
		production.mineral_to_energy_divisor <= 0
	) {
		return 0;
	}
	return #floor(
		#to_float(#max(game.get('f_base_get_pending_production')(base), 0)) /
		#to_float(production.mineral_to_energy_divisor)
	);
};

const get_hurry_cost = (game, base) => {
	const production = base.get_production();
	if (!#is_defined(production)) {
		return 0;
	}
	const accumulated = base.get_accumulated_minerals();
	const production_cost_resolver = #is_defined(game.get)
		? game.get('f_base_get_production_cost')
		: #undefined;
	const production_cost = #is_defined(production_cost_resolver)
		? production_cost_resolver(base, production)
		: production.mineral_cost;
	const missing = #max(production_cost - accumulated, 0);
	if (missing == 0) {
		return 0;
	}
	let cost = missing * 2;
	if (production.production_kind == 'unit') {
		cost += #floor(
			#to_float(missing * missing) / #to_float(production_cost)
		);
	}
	if (accumulated < 10) {
		cost *= 2;
	}
	return cost;
};

const get_player_economy = (game, player) => {
	let result = get_player_commerce(game, player);
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id == player.id) {
			result += get_base_economy(game, base) + get_base_stockpile_energy(game, base);
		}
	}
	return result;
};

const get_liquidation_candidate = (game, player) => {
	let result = null;
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id != player.id) {
			continue;
		}
		for (facility of base.get_facilities()) {
			if (
				facility.energy_maintenance > 0 &&
				(
					result == null ||
					facility.energy_maintenance > result.facility.energy_maintenance ||
					(
						facility.energy_maintenance == result.facility.energy_maintenance &&
						base.id < result.base.id
					)
				)
			) {
				result = {base: base, facility: facility};
			}
		}
	}
	return result;
};

return (game) => {
	game.on('start', (e) => {
		game.set('f_economy_get_base_energy', (base) => { return get_base_energy(game, base); });
		game.set('f_economy_get_base_allocation', get_base_allocation);
		game.set('f_economy_get_base', get_base_economy);
		game.set('f_economy_get_base_commerce', get_base_commerce);
		game.set('f_economy_get_player_commerce_ledger', get_player_commerce_ledger);
		game.set('f_economy_get_base_psych', get_base_psych);
		game.set('f_economy_get_base_stockpile_energy', get_base_stockpile_energy);
		game.set('f_economy_get_player_commerce', get_player_commerce);
		game.set('f_economy_get_commerce_technology', (player) => {
			return get_commerce_technology(game, player);
		});
		game.set('f_economy_get_player', get_player_economy);
		game.set('f_economy_get_hurry_cost', (base) => { return get_hurry_cost(game, base); });
		game.set('f_economy_get_liquidation_candidate', get_liquidation_candidate);
		game.on('turn', (e) => {
			if (!game.is_master()) {
				return;
			}
			for (player of game.get_players()) {
				game.event('settle_player_economy', {
					player: player,
					liquidation_count: 0,
				});
			}
			for (player of game.get_players()) {
				if (player.get_sanction_turns() > 0) {
					game.event('process_diplomatic_sanctions', {player: player});
				}
			}
		});
	});
};
