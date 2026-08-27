const project_acquisition = #include('./project_acquisition');
const economic_victory = #include('./economic_victory_rules');
const MAX_ENERGY_CREDITS = 1000000000;

const get_rehome_base = (game, unit, owner_id, lost_base) => {
	let best = null;
	let best_distance = 0;
	for (candidate of game.bm.get_bases()) {
		if (candidate.id == lost_base.id || candidate.get_owner().id != owner_id) {
			continue;
		}
		const distance = game.tm.get_distance(unit.get_tile(), candidate.get_tile());
		if (
			best == null ||
			distance < best_distance ||
			(distance == best_distance && candidate.id < best.id)
		) {
			best = candidate;
			best_distance = distance;
		}
	}
	return best;
};

const rehome_units = (game, lost_base, owner_id) => {
	let snapshots = [];
	for (unit of game.um.get_units()) {
		if (unit.owner != owner_id || unit.home_base_id != lost_base.id) {
			continue;
		}
		snapshots :+{
			unit: unit,
			home_base_id: unit.home_base_id,
		};
		const destination = get_rehome_base(game, unit, owner_id, lost_base);
		unit.set_home_base_id(destination == null ? 0 : destination.id);
	}
	return snapshots;
};

const restore_units = (snapshots) => {
	for (snapshot of snapshots) {
		snapshot.unit.set_home_base_id(snapshot.home_base_id);
	}
};

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

const capture_base = (game, base, new_owner) => {
	const old_owner = base.get_owner();
	const old_queue = get_queue_specs(base);
	const rehomed_units = rehome_units(game, base, old_owner.id);
	const captured_headquarters = base.has_facility('Headquarters');
	const economic_victory_state = captured_headquarters
		? economic_victory.get_base_state(base)
		: null;
	let economic_victory_capture = #undefined;
	if (captured_headquarters) {
		base.remove_facility('Headquarters');
	}
	if (economic_victory_state != null) {
		economic_victory_capture = {
			state: economic_victory_state,
			old_owner_energy: old_owner.get_energy_credits(),
			new_owner_energy: new_owner.get_energy_credits(),
			new_owner: new_owner,
		};
		economic_victory.clear_base_state(base);
		const captured_energy = #floor(#to_float(economic_victory_state.cost) / 2.0);
		old_owner.set_energy_credits(#min(
			MAX_ENERGY_CREDITS,
			old_owner.get_energy_credits() + economic_victory_state.cost - captured_energy
		));
		new_owner.set_energy_credits(#min(
			MAX_ENERGY_CREDITS,
			new_owner.get_energy_credits() + captured_energy
		));
	}

	base.set_owner(new_owner);
	if (#is_defined(economic_victory_capture)) {
		game.message(
			new_owner.get_faction().name + ' has captured ' + old_owner.get_faction().name +
			'\'s Headquarters and foiled its Global Energy Market bid.'
		);
		game.trigger('economic_victory_updated', {player: old_owner});
		game.trigger('economy_updated', {player: old_owner});
		game.trigger('economy_updated', {player: new_owner});
	}
	const empath_guild_infiltration = base.has_facility('TheEmpathGuild')
		? project_acquisition.apply_empath_guild(game, base)
		: #undefined;
	if (#is_defined(game.get) && base.has_facility('ThePlanetaryDatalinks')) {
		const queue_datalinks = game.get('f_project_queue_planetary_datalinks');
		if (#is_defined(queue_datalinks)) {
			queue_datalinks();
		}
	}
	let valid_queue = [];
	for (production of old_queue) {
		if (base.can_produce(production.kind, production.id)) {
			valid_queue :+production;
		}
	}
	base.set_production_queue(valid_queue);

	return {
		old_owner: old_owner,
		old_queue: old_queue,
		rehomed_units: rehomed_units,
		captured_headquarters: captured_headquarters,
		empath_guild_infiltration: empath_guild_infiltration,
		economic_victory_capture: economic_victory_capture,
	};
};

const restore_base = (base, snapshot) => {
	if (#is_defined(snapshot.empath_guild_infiltration)) {
		project_acquisition.rollback_empath_guild(snapshot.empath_guild_infiltration);
	}
	if (base.get_owner().id != snapshot.old_owner.id) {
		base.set_owner(snapshot.old_owner);
	}
	if (snapshot.captured_headquarters && !base.has_facility('Headquarters')) {
		base.add_facility('Headquarters');
	}
	if (#is_defined(snapshot.economic_victory_capture)) {
		const capture = snapshot.economic_victory_capture;
		snapshot.old_owner.set_energy_credits(capture.old_owner_energy);
		capture.new_owner.set_energy_credits(capture.new_owner_energy);
		economic_victory.set_base_state(
			base,
			capture.state.turn,
			capture.state.cost
		);
	}
	base.set_production_queue(snapshot.old_queue);
	restore_units(snapshot.rehomed_units);
};

return {
	rehome_units: rehome_units,
	restore_units: restore_units,
	capture_base: capture_base,
	restore_base: restore_base,
};
