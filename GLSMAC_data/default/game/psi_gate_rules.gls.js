const USED_TURN_KEY = 'psi_gate_used_turn';

const get_base_used_turn = (base) => {
	return base.has(USED_TURN_KEY) ? base.get(USED_TURN_KEY) : 0;
};

const is_available = (game, base) => {
	return base.has_facility('PsiGate') && get_base_used_turn(base) != game.get_turn();
};

const get_teleport_error = (game, unit, caller, destination) => {
	if (unit.owner != caller) {
		return 'Unit can only be teleported by its owner';
	}
	if (game.is_turn_complete(caller)) {
		return 'Player has already completed this turn';
	}
	if (unit.health <= 0.0) {
		return 'Destroyed unit cannot use a Psi Gate';
	}
	if (#is_defined(unit.transport_id) && unit.transport_id > 0) {
		return 'Embarked unit must travel with its transport';
	}
	if (unit.terraforming != 'none') {
		return 'Cancel the unit\'s terraforming order before teleporting';
	}
	if (unit.movement <= 0.0) {
		return 'Unit is out of moves';
	}
	const source_tile = unit.get_tile();
	const source = source_tile.get_base();
	if (source == null || source.get_owner().id != caller) {
		return 'Unit must be inside an owned base';
	}
	if (!is_available(game, source)) {
		return source.has_facility('PsiGate')
			? 'Source Psi Gate has already been used this turn'
			: 'Source base needs a Psi Gate';
	}
	if (destination == null || destination == source) {
		return 'Select another owned base with an available Psi Gate';
	}
	if (destination.get_owner().id != caller) {
		return 'Destination Psi Gate must be owned by the unit owner';
	}
	if (!is_available(game, destination)) {
		return destination.has_facility('PsiGate')
			? 'Destination Psi Gate has already been used this turn'
			: 'Destination base needs a Psi Gate';
	}
	if (source_tile.is_locked() || destination.get_tile().is_locked()) {
		return 'Psi Gate endpoint is currently busy';
	}
	for (candidate of destination.get_tile().get_units(true)) {
		if (candidate.owner != caller) {
			return 'Destination base contains foreign units';
		}
	}
};

const get_available_destinations = (game, unit, caller) => {
	let result = [];
	for (base of game.get_bm().get_bases()) {
		if (!#is_defined(get_teleport_error(game, unit, caller, base))) {
			result :+base;
		}
	}
	return result;
};

const snapshot_used_turn = (base) => {
	return {
		defined: base.has(USED_TURN_KEY),
		value: base.get(USED_TURN_KEY),
	};
};

const restore_used_turn = (base, snapshot) => {
	if (snapshot.defined) {
		base.set(USED_TURN_KEY, snapshot.value);
	} else {
		base.unset(USED_TURN_KEY);
	}
};

return {
	used_turn_key: USED_TURN_KEY,
	get_base_used_turn: get_base_used_turn,
	is_available: is_available,
	get_teleport_error: get_teleport_error,
	get_available_destinations: get_available_destinations,
	snapshot_used_turn: snapshot_used_turn,
	restore_used_turn: restore_used_turn,
};
