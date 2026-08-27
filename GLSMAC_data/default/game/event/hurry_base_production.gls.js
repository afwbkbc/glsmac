return {
	validate: (e) => {
		const base = e.data.base;
		const owner = base.get_owner();
		if (e.caller != owner.id) {
			return 'Only base owner can hurry production';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (!#is_defined(base.get_production())) {
			return 'Base has no production to hurry';
		}
		const cost = e.game.get('f_economy_get_hurry_cost')(base);
		if (cost <= 0) {
			return 'Production is already complete';
		}
		if (owner.energy_credits < cost) {
			return 'Not enough energy credits to hurry production';
		}
	},

	apply: (e) => {
		const base = e.data.base;
		const owner = base.get_owner();
		const production = base.get_production();
		const cost = e.game.get('f_economy_get_hurry_cost')(base);
		const previous = {
			energy_credits: owner.energy_credits,
			minerals: base.get_accumulated_minerals(),
		};
		owner.set_energy_credits(owner.energy_credits - cost);
		const production_cost_resolver = e.game.get('f_base_get_production_cost');
		base.set_accumulated_minerals(#is_defined(production_cost_resolver)
			? production_cost_resolver(base, production)
			: production.mineral_cost
		);
		e.game.trigger('economy_updated', {player: owner});
		return previous;
	},

	rollback: (e) => {
		const base = e.data.base;
		const owner = base.get_owner();
		owner.set_energy_credits(e.applied.energy_credits);
		base.set_accumulated_minerals(e.applied.minerals);
		e.game.trigger('economy_updated', {player: owner});
	},
};
