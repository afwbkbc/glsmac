const unit_abilities = #include('../unit_abilities');
const prototype_rules = #include('../prototype_rules');
const economic_victory = #include('../economic_victory_rules');

const get_queue_specs = (base) => {
	let result = [];
	for (production of base.get_production_queue()) {
		result :+{
			kind: production.production_kind,
			id: production.id,
		};
	}
	return result;
};

const cancel_project_queues = (game, project_id, completing_base) => {
	let snapshots = [];
	for (candidate of game.get_bm().get_bases()) {
		if (candidate == completing_base) {
			continue;
		}
		const old_queue = get_queue_specs(candidate);
		let removed = false;
		for (let i = #sizeof(old_queue) - 1; i >= 0; i--) {
			if (old_queue[i].kind == 'project' && old_queue[i].id == project_id) {
				candidate.remove_production(i);
				removed = true;
			}
		}
		if (removed) {
			snapshots :+{base: candidate, queue: old_queue};
		}
	}
	return snapshots;
};

const relocate_headquarters = (game, completing_base) => {
	let previous = [];
	for (candidate of game.get_bm().get_bases()) {
		if (
			candidate != completing_base &&
			candidate.get_owner().id == completing_base.get_owner().id &&
			candidate.has_facility('Headquarters')
		) {
			candidate.remove_facility('Headquarters');
			previous :+candidate;
		}
	}
	return previous;
};

const relocate_economic_victory = (game, previous_headquarters, completing_base) => {
	for (base of previous_headquarters) {
		const state = economic_victory.get_base_state(base);
		if (state != null) {
			economic_victory.clear_base_state(base);
			economic_victory.set_base_state(completing_base, state.turn, state.cost);
			game.trigger('economic_victory_updated', {player: completing_base.get_owner()});
			return {base: base, turn: state.turn, cost: state.cost};
		}
	}
	return #undefined;
};

const get_production_morale = (game, base, production, is_prototype) => {
	let morale = 1 + unit_abilities.get_morale_bonus(production) +
		(is_prototype ? 1 : 0);
	let training_morale_bonus = 0;
	const resolver = game.get('f_base_get_effective_facilities');
	const facilities = #is_defined(resolver) ? resolver(base) : base.get_facilities();
	for (facility of facilities) {
		if (production.is_native) {
			morale += #is_defined(facility.native_lifecycle_bonus)
				? facility.native_lifecycle_bonus
				: 0;
		} else {
			if (#is_defined(production.offense) && production.offense > 0) {
				training_morale_bonus += facility.unit_morale_bonus;
			}
			if (#is_defined(production.is_land) && production.is_land) {
				training_morale_bonus += #is_defined(facility.unit_morale_land_bonus)
					? facility.unit_morale_land_bonus
					: 0;
			} else if (#is_defined(production.is_water) && production.is_water) {
				training_morale_bonus += #is_defined(facility.unit_morale_water_bonus)
					? facility.unit_morale_water_bonus
					: 0;
			} else if (#is_defined(production.is_air) && production.is_air) {
				training_morale_bonus += #is_defined(facility.unit_morale_air_bonus)
					? facility.unit_morale_air_bonus
					: 0;
			}
		}
	}
	if (!production.is_native) {
		const adjust_training_morale = game.get('f_social_get_unit_training_morale_bonus');
		morale += #is_defined(adjust_training_morale)
			? adjust_training_morale(base.get_owner(), training_morale_bonus)
			: training_morale_bonus;
	}
	if (production.is_native) {
		const get_project_effects = game.get('f_project_get_effects');
		if (#is_defined(get_project_effects)) {
			morale += get_project_effects(base).native_lifecycle_bonus;
		}
	}
	const morale_set = game.um.get_moraleset(production.morale_set);
	return #max(0, #min(morale, #sizeof(morale_set) - 1));
};

return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to process base production';
		}
	},

	apply: (e) => {
		const base = e.data.base;
		const old_minerals = base.get_accumulated_minerals();
		const old_queue = get_queue_specs(base);
		const production = base.get_production();
		let produced_unit = #undefined;
		let completed_facility = #undefined;
		let previous_headquarters = [];
		let economic_victory_relocation = #undefined;
		let cancelled_project_queues = [];
		let project_completion_effects = #undefined;
		let consumed_pops = [];
		let pop_type_snapshots = [];
		let network_node_link_state = #undefined;
		let prototype_state = #undefined;
		let orbital_launch = #undefined;

		if (#is_defined(production)) {
			const pending_minerals = e.game.get('f_base_get_pending_production')(base);
			const is_mineral_conversion =
				production.production_kind == 'facility' &&
				#is_defined(production.mineral_to_energy_divisor) &&
				production.mineral_to_energy_divisor > 0;
			const is_orbital =
				production.production_kind == 'facility' && (
					(
						#is_defined(production.orbital_resource) &&
						production.orbital_resource != ''
					) || (
						#is_defined(production.orbital_defense) &&
						production.orbital_defense
					)
				);
			let updated_minerals = old_minerals +
				(is_mineral_conversion ? 0 : pending_minerals);
			const production_cost_resolver = e.game.get('f_base_get_production_cost');
			const production_cost = #is_defined(production_cost_resolver)
				? production_cost_resolver(base, production)
				: production.mineral_cost;
			const population_cost = (
				production.production_kind == 'unit' &&
				#is_defined(production.can_found_base) &&
				production.can_found_base
			) ? 1 : 0;
			const has_population = population_cost == 0 || base.get_size() > population_cost;
			if (
				!is_mineral_conversion &&
				updated_minerals >= production_cost &&
				has_population
			) {
				const existing_project_base = production.production_kind == 'project'
					? e.game.get_bm().get_project_base(production.id)
					: #undefined;
				if (#is_defined(existing_project_base)) {
					base.remove_production(0);
				} else {
					updated_minerals -= production_cost;
					const queue_size = #sizeof(base.get_production_queue());
					if (production.production_kind == 'unit') {
						const is_prototype = prototype_rules.is_prototype(
							base.get_owner(),
							production
						);
						produced_unit = e.game.um.spawn_unit({
							def: production.id,
							owner: base.get_owner(),
							tile: base.get_tile(),
							morale: get_production_morale(
								e.game,
								base,
								production,
								is_prototype
							),
							health: 1.0,
							home_base_id: base.id,
						});
						if (is_prototype) {
							prototype_state = prototype_rules.apply(
								base.get_owner(),
								production
							);
						}
						for (let i = 0; i < population_cost; i++) {
							const pop = e.game.get('f_base_select_population_for_reduction')(base);
							if (pop == null) {
								throw Error('Could not select population for unit production');
							}
							const worked_tile = pop.get('worked_tile');
							consumed_pops :+{
								type: pop.get_type(),
								worked_tile: worked_tile,
							};
							if (#is_defined(worked_tile)) {
								base.unwork_pop_tile(pop, worked_tile);
							}
							base.destroy_pop(pop);
						}
						if (queue_size > 1) {
							base.remove_production(0);
						}
					} else if (
						production.production_kind == 'facility' ||
						production.production_kind == 'project'
					) {
						base.remove_production(0);
						if (is_orbital) {
							orbital_launch = e.game.get('f_orbital_apply_launch')(
								base,
								production
							);
						} else {
							base.add_facility(production.id);
							completed_facility = production.id;
						}
						if (production.id == 'NetworkNode') {
							const linked_key = 'network_node_artifact_linked';
							network_node_link_state = {
								defined: base.has(linked_key),
								value: base.get(linked_key),
							};
							base.unset(linked_key);
						}
						if (production.id == 'Headquarters') {
							previous_headquarters = relocate_headquarters(e.game, base);
							economic_victory_relocation = relocate_economic_victory(
								e.game,
								previous_headquarters,
								base
							);
						}
						if (production.production_kind == 'project') {
							cancelled_project_queues = cancel_project_queues(
								e.game,
								production.id,
								base
							);
							const apply_completion_effects = e.game.get(
								'f_project_apply_completion_effects'
							);
							if (#is_defined(apply_completion_effects)) {
								project_completion_effects = apply_completion_effects(
									base,
									production.id
								);
							}
						}
					} else {
						throw Error('Unknown production kind: ' + production.production_kind);
					}
				}
			}
			base.set_accumulated_minerals(updated_minerals);
		}
		if (#sizeof(consumed_pops) > 0 || #is_defined(completed_facility)) {
			for (pop of base.get_pops()) {
				pop_type_snapshots :+{
					pop: pop,
					type: pop.get_type(),
				};
			}
			const psych = e.game.get('f_economy_get_base_psych')(e.game, base);
			e.game.get('f_base_process_psych')(e.game, base, psych);
		}
		if (completed_facility == 'ThePlanetaryDatalinks') {
			const queue_datalinks = e.game.get('f_project_queue_planetary_datalinks');
			if (#is_defined(queue_datalinks)) {
				queue_datalinks();
			}
		}

		return {
			old_minerals: old_minerals,
			old_queue: old_queue,
			produced_unit: produced_unit,
			completed_facility: completed_facility,
			previous_headquarters: previous_headquarters,
			economic_victory_relocation: economic_victory_relocation,
			cancelled_project_queues: cancelled_project_queues,
			project_completion_effects: project_completion_effects,
			consumed_pops: consumed_pops,
			pop_type_snapshots: pop_type_snapshots,
			network_node_link_state: network_node_link_state,
			prototype_state: prototype_state,
			orbital_launch: orbital_launch,
		};
	},

	rollback: (e) => {
		if (#is_defined(e.applied.project_completion_effects)) {
			const rollback_completion_effects = e.game.get(
				'f_project_rollback_completion_effects'
			);
			if (#is_defined(rollback_completion_effects)) {
				rollback_completion_effects(e.applied.project_completion_effects);
			}
		}
		if (#is_defined(e.applied.produced_unit)) {
			e.game.um.despawn_unit(e.applied.produced_unit);
		}
		if (#is_defined(e.applied.prototype_state)) {
			prototype_rules.rollback(e.applied.prototype_state);
		}
		if (#is_defined(e.applied.orbital_launch)) {
			e.game.get('f_orbital_rollback_launch')(e.applied.orbital_launch);
		}
		if (#is_defined(e.applied.completed_facility)) {
			e.data.base.remove_facility(e.applied.completed_facility);
		}
		if (#is_defined(e.applied.network_node_link_state)) {
			const linked_key = 'network_node_artifact_linked';
			if (e.applied.network_node_link_state.defined) {
				e.data.base.set(linked_key, e.applied.network_node_link_state.value);
			} else {
				e.data.base.unset(linked_key);
			}
		}
		if (#is_defined(e.applied.previous_headquarters)) {
			for (previous_headquarters of e.applied.previous_headquarters) {
				previous_headquarters.add_facility('Headquarters');
			}
		}
		if (#is_defined(e.applied.economic_victory_relocation)) {
			economic_victory.clear_base_state(e.data.base);
			economic_victory.set_base_state(
				e.applied.economic_victory_relocation.base,
				e.applied.economic_victory_relocation.turn,
				e.applied.economic_victory_relocation.cost
			);
			e.game.trigger('economic_victory_updated', {player: e.data.base.get_owner()});
		}
		for (snapshot of e.applied.cancelled_project_queues) {
			snapshot.base.set_production_queue(snapshot.queue);
		}
		for (snapshot of e.applied.pop_type_snapshots) {
			snapshot.pop.set_type(snapshot.type);
		}
		for (snapshot of e.applied.consumed_pops) {
			const pop = e.data.base.create_pop({type: snapshot.type});
			if (#is_defined(snapshot.worked_tile)) {
				e.data.base.work_pop_tile(pop, snapshot.worked_tile);
			}
		}
		e.data.base.set_production_queue(e.applied.old_queue);
		e.data.base.set_accumulated_minerals(e.applied.old_minerals);
	},

};
