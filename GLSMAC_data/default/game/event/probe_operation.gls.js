const base_capture = #include('../base_capture');
const snapshot_unit = #include('../entity_snapshots').snapshot_unit;

const is_un_charter_active = (game) => {
	const is_repealed = game.get('f_council_is_un_charter_repealed');
	return !#is_defined(is_repealed) || !is_repealed();
};

const get_target_player = (game, operation, target) => {
	return operation == 'subvert_unit'
		? game.get_player(target.owner)
		: target.get_owner();
};

const get_sabotage_facilities = (base) => {
	let result = [];
	for (facility of base.get_facilities()) {
		if (!facility.is_project && facility.id != 'Headquarters') {
			result :+facility.id;
		}
	}
	return result;
};

const spawn_snapshot = (game, snapshot, owner_id, transferred) => {
	const unit = game.um.spawn_unit({
		id: snapshot.id,
		def: snapshot.def,
		owner: game.get_player(owner_id),
		tile: game.tm.get_tile(snapshot.tile_x, snapshot.tile_y),
		morale: snapshot.morale,
		health: snapshot.health,
		terraforming: transferred ? 'none' : snapshot.terraforming,
		terraforming_turns_remaining: transferred ? 0 : snapshot.terraforming_turns_remaining,
		home_base_id: transferred ? 0 : snapshot.home_base_id,
		fuel: snapshot.fuel,
		transport_id: snapshot.transport_id,
	});
	unit.movement = transferred ? 0.0 : snapshot.movement;
	unit.moved_this_turn = transferred ? true : snapshot.moved_this_turn;
	unit.native_capture_attempted = transferred ? false : snapshot.native_capture_attempted;
	return unit;
};

const despawn_snapshots = (game, snapshots) => {
	for (let i = #sizeof(snapshots) - 1; i >= 0; i--) {
		if (game.um.has_unit(snapshots[i].id)) {
			game.um.despawn_unit(game.um.get_unit(snapshots[i].id));
		}
	}
};

const spawn_snapshots = (game, snapshots, owner_id, transferred) => {
	for (snapshot of snapshots) {
		if (snapshot.transport_id == 0) {
			spawn_snapshot(game, snapshot, owner_id, transferred);
		}
	}
	for (snapshot of snapshots) {
		if (snapshot.transport_id != 0) {
			spawn_snapshot(game, snapshot, owner_id, transferred);
		}
	}
};

const restore_unit = (game, snapshot) => {
	if (game.um.has_unit(snapshot.id)) {
		game.um.despawn_unit(game.um.get_unit(snapshot.id));
	}
	return spawn_snapshot(game, snapshot, snapshot.owner, false);
};

const snapshot_pop_types = (base) => {
	let result = [];
	for (pop of base.get_pops()) {
		result :+{pop: pop, type: pop.get_type()};
	}
	return result;
};

const restore_pop_types = (snapshots) => {
	for (snapshot of snapshots) {
		snapshot.pop.set_type(snapshot.type);
	}
};

const get_riot_pop = (base) => {
	for (pop of base.get_pops()) {
		if (pop.get_type() != 'DRONE') {
			return pop;
		}
	}
	return null;
};

const snapshot_surviving_pop_types = (base, removed_count) => {
	let result = [];
	const pops = base.get_pops();
	for (let i = 0; i < #sizeof(pops) - removed_count; i++) {
		result :+{pop: pops[i], type: pops[i].get_type()};
	}
	return result;
};

const remove_base_population = (game, base, count) => {
	let removed = [];
	const old_nutrients = base.get('accumulated_nutrients');
	game.get('f_base_reset_nutrients')(game, base);
	for (let i = 0; i < count; i++) {
		const pops = base.get_pops();
		const pop = pops[#sizeof(pops) - 1];
		const worked_tile = pop.get('worked_tile');
		removed :+{type: pop.get_type(), worked_tile: worked_tile};
		if (#is_defined(worked_tile)) {
			game.get('f_base_pop_unwork_tile')(base, pop);
		}
		base.destroy_pop(pop);
	}
	return {pops: removed, nutrients: old_nutrients};
};

const restore_base_population = (game, base, snapshot) => {
	for (let i = #sizeof(snapshot.pops) - 1; i >= 0; i--) {
		const pop = base.create_pop({type: snapshot.pops[i].type});
		if (#is_defined(snapshot.pops[i].worked_tile)) {
			game.get('f_base_pop_work_tile')(base, pop, snapshot.pops[i].worked_tile);
		}
	}
	base.set('accumulated_nutrients', snapshot.nutrients);
};

const refresh_base_psych = (game, base) => {
	const get_psych = game.get('f_economy_get_base_psych');
	const process_psych = game.get('f_base_process_psych');
	if (#is_defined(get_psych) && #is_defined(process_psych)) {
		process_psych(game, base, get_psych(game, base));
	}
};

const snapshot_base_units = (game, base, owner_id) => {
	let result = [];
	const base_tile = base.get_tile();
	for (unit of game.um.get_units()) {
		const unit_tile = unit.get_tile();
		if (
			unit.owner == owner_id &&
			(unit_tile == base_tile || unit_tile.is_adjactent_to(base_tile))
		) {
			result :+snapshot_unit(unit);
		}
	}
	return result;
};

const promote_probe = (game, probe) => {
	const morale_set = game.um.get_moraleset(probe.get_def().morale_set);
	probe.morale = #min(probe.morale + 1, #sizeof(morale_set) - 1);
};

const get_result_message = (game, operation, target, resolved) => {
	let message = '';
	if (!resolved.success) {
		message = 'Probe operation failed.';
	} else if (operation == 'infiltrate') {
		message = 'Datalinks infiltrated.';
	} else if (operation == 'steal_technology') {
		const resolver = game.get('f_technology_get_definition');
		const definition = #is_defined(resolver) ? resolver(resolved.technology_id) : null;
		message = 'Acquired ' + (
			definition == null ? resolved.technology_id : definition.name
		) + '.';
	} else if (operation == 'sabotage') {
		message = resolved.sabotage_facility_id == ''
			? 'Destroyed accumulated minerals at ' + target.name + '.'
			: 'Sabotaged ' + resolved.sabotage_facility_id + ' at ' + target.name + '.';
	} else if (operation == 'drain_energy') {
		message = 'Drained ' + #to_string(resolved.drain_amount) + ' energy credits.';
	} else if (operation == 'incite_drone_riots') {
		message = 'Drone riots incited at ' + target.name + '.';
	} else if (operation == 'assassinate_researchers') {
		message = 'Prominent researchers assassinated; ' +
			#to_string(resolved.research_loss) + ' research points lost.';
	} else if (operation == 'genetic_plague') {
		message = 'Genetic plague caused ' + #to_string(resolved.population_loss) +
			' population casualties at ' + target.name + '.';
	} else if (operation == 'subvert_unit') {
		const definition = target.get_def();
		message = 'Subverted ' + (
			#is_defined(definition.name) ? definition.name : definition.id
		) + ' for ' + #to_string(resolved.cost) + ' energy credits.';
	} else if (operation == 'mind_control_base') {
		message = 'Mind controlled ' + target.name + ' for ' +
			#to_string(resolved.cost) + ' energy credits.';
	}
	if (resolved.detected) {
		message += ' The operation was detected.';
	}
	if (!resolved.survives) {
		message += ' Probe Team lost.';
	}
	return message;
};

const validate_base_operation = (e, actor, target_player) => {
	const operation = e.data.operation;
	const base = e.data.target;
	if (
		#typeof(base) != 'Object' || #typeof(base.get_owner) != 'Callable' ||
		#typeof(base.get_size) != 'Callable' || #typeof(base.get_facilities) != 'Callable'
	) {
		return 'Probe operation target must be a base';
	}
	const has_intelligence = e.game.get('f_council_has_intelligence');
	if (
		operation == 'infiltrate' &&
		(#is_defined(has_intelligence)
			? has_intelligence(actor, target_player)
			: actor.has_infiltrated(target_player))
	) {
		return 'Target faction datalinks are already infiltrated';
	}
	if (
		operation == 'steal_technology' &&
		#sizeof(e.game.get('f_probe_get_unknown_technologies')(actor, target_player)) == 0
	) {
		return 'Target faction has no technology available to steal';
	}
	if (
		operation == 'sabotage' && !e.game.get('f_probe_can_sabotage')(base)
	) {
		return 'Target base has nothing available to sabotage';
	}
	if (
		operation == 'drain_energy' &&
		(target_player.energy_credits <= 0 || actor.energy_credits >= 1000000000)
	) {
		return 'No energy credits can be drained from the target';
	}
	if (
		operation == 'incite_drone_riots' &&
		!e.game.get('f_probe_can_incite_drone_riots')(base)
	) {
		return 'Target base has no population available to incite';
	}
	if (operation == 'assassinate_researchers') {
		if (e.data.unit.morale < 3) {
			return 'Probe Team lacks the experience to assassinate researchers';
		}
		if (e.game.get('f_probe_get_assassination_research_loss')(target_player) <= 0) {
			return 'Target faction has no active research to disrupt';
		}
	}
	if (operation == 'genetic_plague') {
		if (!actor.has_technology('RetroviralEngineering')) {
			return 'Retroviral Engineering is required for genetic warfare';
		}
		if (e.game.get('f_probe_get_plague_population_loss')(base) <= 0) {
			return 'Target base is too small for a genetic plague';
		}
		if (actor.get_major_atrocities() >= 1000000) {
			return 'Major atrocity limit has been reached';
		}
	}
	if (operation == 'mind_control_base') {
		const cost = e.game.get('f_probe_get_mind_control_cost')(actor, base);
		if (cost == null) {
			return base.has_facility('Headquarters')
				? 'A headquarters base cannot be mind controlled'
				: 'Target faction is immune to mind control';
		}
		if (actor.energy_credits < cost) {
			return 'Not enough energy credits to mind control this base';
		}
	}
};

return {
	validate: (e) => {
		const probe = e.data.unit;
		if (#typeof(probe) != 'Object' || #typeof(probe.get_def) != 'Callable') {
			return 'Probe operation requires a unit';
		}
		if (probe.owner != e.caller) {
			return 'Probe Team can only be ordered by its owner';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (!e.game.get('f_probe_is_unit')(probe)) {
			return 'Only a Probe Team can perform probe operations';
		}
		if (probe.health <= 0.0 || probe.movement <= 0.0) {
			return 'Probe Team is unable to act';
		}
		if (#is_defined(probe.transport_id) && probe.transport_id > 0) {
			return 'Embarked Probe Team must disembark before acting';
		}
		if (probe.terraforming != 'none') {
			return 'Probe Team cannot act while terraforming';
		}

		const operations = e.game.get('f_probe_get_operations')();
		const operation = e.data.operation;
		if (#typeof(operation) != 'String' || !#is_defined(operations[operation])) {
			return 'Unknown probe operation';
		}
		const target = e.data.target;
		if (#typeof(target) != 'Object' || #typeof(target.get_tile) != 'Callable') {
			return 'Probe operation requires a target';
		}
		if (!probe.get_tile().is_adjactent_to(target.get_tile())) {
			return 'Probe operation target must be adjacent';
		}
		const actor = e.game.get_player(e.caller);
		if (
			operations[operation].target == 'base' &&
			#typeof(target.get_owner) != 'Callable'
		) {
			return 'Probe operation target must be a base';
		}
		const target_player = get_target_player(e.game, operation, target);
		if (target_player.id == actor.id) {
			return 'Probe Team cannot target its own faction';
		}
		if (e.game.get('f_probe_has_project')(target_player, 'TheHunterSeekerAlgorithm')) {
			return 'The Hunter-Seeker Algorithm blocks this probe operation';
		}

		if (operations[operation].target == 'base') {
			return validate_base_operation(e, actor, target_player);
		}
		if (
			#typeof(target.get_def) != 'Callable' || target.health <= 0.0 ||
			(#is_defined(target.transport_id) && target.transport_id > 0)
		) {
			return 'Probe subversion target must be an active, unembarked unit';
		}
		if (#is_defined(target.get_cargo) && #sizeof(target.get_cargo()) > 0) {
			return 'A transport carrying units cannot be subverted';
		}
		const cost = e.game.get('f_probe_get_subversion_cost')(actor, target);
		if (cost == null) {
			return 'Target faction is immune to unit subversion';
		}
		if (actor.energy_credits < cost) {
			return 'Not enough energy credits to subvert this unit';
		}
	},

	resolve: (e) => {
		const operation = e.data.operation;
		const actor = e.game.get_player(e.caller);
		const target_player = get_target_player(e.game, operation, e.data.target);
		const operations = e.game.get('f_probe_get_operations')();
		const defender = operations[operation].target == 'base'
			? e.game.get('f_probe_get_defending_probe')(target_player, e.data.target)
			: null;
		const chance = e.game.get('f_probe_get_success_chance')(
			e.data.unit,
			target_player,
			operation,
			e.data.target
		);
		const paid = operation == 'subvert_unit' || operation == 'mind_control_base';
		const intercepted = defender != null;
		const success = (paid && !intercepted) || e.game.random.get_int(1, 100) <= chance;
		const detected = operation == 'genetic_plague' || intercepted || paid || !success ||
			e.game.random.get_int(1, 100) <= 35;
		const survives = intercepted && !success
			? false
			: paid || e.game.random.get_int(1, 100) <= (success ? 85 : 35);
		let result = {
			success: success,
			detected: detected,
			survives: survives,
			chance: chance,
			cost: 0,
			technology_id: '',
			sabotage_facility_id: '',
			drain_amount: 0,
			research_loss: 0,
			population_loss: 0,
			defender_id: defender == null ? 0 : defender.id,
		};
		if (operation == 'subvert_unit') {
			result.cost = e.game.get('f_probe_get_subversion_cost')(actor, e.data.target);
		} else if (operation == 'mind_control_base') {
			result.cost = e.game.get('f_probe_get_mind_control_cost')(actor, e.data.target);
		} else if (success && operation == 'steal_technology') {
			const unknown = e.game.get('f_probe_get_unknown_technologies')(actor, target_player);
			result.technology_id = unknown[e.game.random.get_int(0, #sizeof(unknown) - 1)];
		} else if (success && operation == 'sabotage') {
			const facilities = get_sabotage_facilities(e.data.target);
			if (
				#sizeof(facilities) > 0 &&
				(e.data.target.get_accumulated_minerals() <= 0 || e.game.random.get_int(0, 1) == 1)
			) {
				result.sabotage_facility_id = facilities[
					e.game.random.get_int(0, #sizeof(facilities) - 1)
				];
			}
		} else if (success && operation == 'drain_energy') {
			const room = 1000000000 - actor.energy_credits;
			result.drain_amount = #min(
				room,
				#min(
					target_player.energy_credits,
					#max(1, #floor(#to_float(target_player.energy_credits) / 4.0))
				)
			);
		} else if (success && operation == 'assassinate_researchers') {
			result.research_loss = e.game.get('f_probe_get_assassination_research_loss')(
				target_player
			);
		} else if (success && operation == 'genetic_plague') {
			result.population_loss = e.game.get('f_probe_get_plague_population_loss')(
				e.data.target
			);
		}
		return result;
	},

	apply: (e) => {
		const operation = e.data.operation;
		const probe = e.data.unit;
		const actor = e.game.get_player(e.caller);
		const target_player = get_target_player(e.game, operation, e.data.target);
		const result_message = get_result_message(
			e.game,
			operation,
			e.data.target,
			e.resolved
		);
		let applied = {
			probe: snapshot_unit(probe),
			target_player_id: target_player.id,
			actor_energy: actor.energy_credits,
			target_energy: target_player.energy_credits,
			infiltrated: actor.has_infiltrated(target_player),
			actor_atrocities: actor.get_major_atrocities(),
			actor_sanction_turns: actor.get_sanction_turns(),
		};

		if (e.resolved.cost > 0) {
			actor.set_energy_credits(actor.energy_credits - e.resolved.cost);
		}
		probe.movement = 0.0;
		probe.moved_this_turn = true;
		if (
			e.resolved.success && e.resolved.defender_id > 0 &&
			e.game.um.has_unit(e.resolved.defender_id)
		) {
			const defender = e.game.um.get_unit(e.resolved.defender_id);
			applied.defending_probe = snapshot_unit(defender);
			e.game.um.despawn_unit(defender);
		}

		if (e.resolved.success && operation == 'infiltrate') {
			actor.set_infiltrated(target_player, true);
		} else if (e.resolved.success && operation == 'steal_technology') {
			applied.research = actor.get_research_state();
			let technologies = [];
			for (id of applied.research.technologies) {
				technologies :+id;
			}
			technologies :+e.resolved.technology_id;
			let target = applied.research.target;
			let progress = applied.research.progress;
			if (target == e.resolved.technology_id) {
				target = e.game.get('f_technology_get_next_target')(technologies, actor);
				if (target == '') {
					progress = 0;
				}
			}
			actor.set_research_state({technologies: technologies, target: target, progress: progress});
			e.game.trigger('research_updated', {player: actor});
			const queue_datalinks = e.game.get('f_project_queue_planetary_datalinks');
			if (#is_defined(queue_datalinks)) {
				queue_datalinks();
			}
		} else if (e.resolved.success && operation == 'sabotage') {
			const base = e.data.target;
			applied.base_minerals = base.get_accumulated_minerals();
			applied.pop_types = snapshot_pop_types(base);
			if (e.resolved.sabotage_facility_id == '') {
				base.set_accumulated_minerals(0);
			} else {
				base.remove_facility(e.resolved.sabotage_facility_id);
				refresh_base_psych(e.game, base);
			}
			e.game.trigger('update_base', {base: base});
		} else if (e.resolved.success && operation == 'drain_energy') {
			actor.set_energy_credits(actor.energy_credits + e.resolved.drain_amount);
			target_player.set_energy_credits(target_player.energy_credits - e.resolved.drain_amount);
		} else if (e.resolved.success && operation == 'incite_drone_riots') {
			const base = e.data.target;
			applied.pop_types = snapshot_pop_types(base);
			get_riot_pop(base).set_type('DRONE');
			e.game.trigger('update_base', {base: base});
		} else if (e.resolved.success && operation == 'assassinate_researchers') {
			applied.target_research = target_player.get_research_state();
			target_player.set_research_state({
				technologies: applied.target_research.technologies,
				target: applied.target_research.target,
				progress: #max(0, applied.target_research.progress - e.resolved.research_loss),
			});
			e.game.trigger('research_updated', {player: target_player});
		} else if (e.resolved.success && operation == 'genetic_plague') {
			const base = e.data.target;
			applied.pop_types = snapshot_surviving_pop_types(base, e.resolved.population_loss);
			applied.population = remove_base_population(e.game, base, e.resolved.population_loss);
			actor.set_major_atrocities(applied.actor_atrocities + 1);
			if (is_un_charter_active(e.game)) {
				actor.set_sanction_turns(#min(1000000, applied.actor_sanction_turns + 10));
				e.game.trigger('diplomatic_sanctions_updated', {
					player: actor,
					turns: actor.get_sanction_turns(),
				});
				e.game.message(
					'Economic sanctions imposed against ' + actor.name + ' for 10 years.'
				);
			}
			refresh_base_psych(e.game, base);
			e.game.trigger('update_base', {base: base});
		} else if (e.resolved.success && operation == 'subvert_unit') {
			applied.transferred_units = [snapshot_unit(e.data.target)];
			despawn_snapshots(e.game, applied.transferred_units);
			spawn_snapshots(e.game, applied.transferred_units, actor.id, true);
		} else if (e.resolved.success && operation == 'mind_control_base') {
			const base = e.data.target;
			applied.pop_types = snapshot_pop_types(base);
			applied.transferred_units = snapshot_base_units(e.game, base, target_player.id);
			despawn_snapshots(e.game, applied.transferred_units);
			applied.base_capture = base_capture.capture_base(e.game, base, actor);
			spawn_snapshots(e.game, applied.transferred_units, actor.id, true);
			refresh_base_psych(e.game, base);
			e.game.trigger('update_base', {base: base});
		}

		if (e.resolved.success && e.resolved.survives && e.game.um.has_unit(probe.id)) {
			promote_probe(e.game, probe);
		}
		if (!e.resolved.survives && e.game.um.has_unit(probe.id)) {
			e.game.um.despawn_unit(probe);
		}

		if (e.resolved.detected) {
			applied.diplomacy = e.game.get('f_diplomacy_snapshot_pair')(actor, target_player);
			e.game.get('f_diplomacy_set_bilateral_relation')(actor, target_player, 'vendetta');
			e.game.get('f_diplomacy_clear_offers')(actor, target_player);
			e.game.trigger('diplomacy_updated', {
				player: actor,
				target: target_player,
				relation: 'vendetta',
			});
		}
		e.game.trigger('economy_updated', {player: actor});
		e.game.trigger('economy_updated', {player: target_player});
		e.game.trigger('probe_operation', {
			player: actor,
			target: target_player,
			operation: operation,
			success: e.resolved.success,
			detected: e.resolved.detected,
			atrocity: operation == 'genetic_plague' && e.resolved.success,
		});
		e.game.message(result_message);
		return applied;
	},

	rollback: (e) => {
		const operation = e.data.operation;
		const actor = e.game.get_player(e.caller);
		const target_player = e.game.get_player(e.applied.target_player_id);
		if (#is_defined(e.applied.transferred_units)) {
			despawn_snapshots(e.game, e.applied.transferred_units);
			spawn_snapshots(e.game, e.applied.transferred_units, target_player.id, false);
		}
		if (#is_defined(e.applied.base_capture)) {
			base_capture.restore_base(e.data.target, e.applied.base_capture);
		}
		if (#is_defined(e.applied.population)) {
			restore_base_population(e.game, e.data.target, e.applied.population);
		}
		if (#is_defined(e.applied.defending_probe)) {
			restore_unit(e.game, e.applied.defending_probe);
		}
		if (#is_defined(e.applied.base_minerals)) {
			e.data.target.set_accumulated_minerals(e.applied.base_minerals);
		}
		if (
			operation == 'sabotage' && e.resolved.sabotage_facility_id != '' &&
			!e.data.target.has_facility(e.resolved.sabotage_facility_id)
		) {
			e.data.target.add_facility(e.resolved.sabotage_facility_id);
		}
		if (#is_defined(e.applied.pop_types)) {
			restore_pop_types(e.applied.pop_types);
		}
		if (#is_defined(e.applied.research)) {
			actor.set_research_state(e.applied.research);
			e.game.trigger('research_updated', {player: actor});
		}
		if (#is_defined(e.applied.target_research)) {
			target_player.set_research_state(e.applied.target_research);
			e.game.trigger('research_updated', {player: target_player});
		}
		actor.set_infiltrated(target_player, e.applied.infiltrated);
		actor.set_energy_credits(e.applied.actor_energy);
		actor.set_major_atrocities(e.applied.actor_atrocities);
		actor.set_sanction_turns(e.applied.actor_sanction_turns);
		target_player.set_energy_credits(e.applied.target_energy);
		e.game.trigger('diplomatic_sanctions_updated', {
			player: actor,
			turns: e.applied.actor_sanction_turns,
		});
		restore_unit(e.game, e.applied.probe);
		if (#is_defined(e.applied.diplomacy)) {
			e.game.get('f_diplomacy_restore_pair')(actor, target_player, e.applied.diplomacy);
			e.game.trigger('diplomacy_updated', {
				player: actor,
				target: target_player,
				relation: e.applied.diplomacy.player_relation,
			});
		}
		e.game.trigger('economy_updated', {player: actor});
		e.game.trigger('economy_updated', {player: target_player});
		if (
			operation == 'sabotage' || operation == 'mind_control_base' ||
			operation == 'incite_drone_riots' || operation == 'genetic_plague'
		) {
			e.game.trigger('update_base', {base: e.data.target});
		}
	},
};
