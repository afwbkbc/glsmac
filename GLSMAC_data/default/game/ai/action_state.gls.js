const MAX_ACTION_ATTEMPTS_PER_UNIT = 16;
const MAX_PENDING_ACTION_CHECKS = 10;

const has_state_changed = (unit, attempt) => {
	return (
		unit.get_tile() != attempt.tile ||
		unit.movement != attempt.movement ||
		unit.health != attempt.health ||
		unit.moved_this_turn != attempt.moved_this_turn ||
		unit.terraforming != attempt.terraforming
	);
};

const can_attempt_action = (unit, action_attempts) => {
	const unit_key = #to_string(unit.id);
	const attempt = action_attempts[unit_key];
	return (
		(!#is_defined(attempt) || (attempt.count < MAX_ACTION_ATTEMPTS_PER_UNIT && !attempt.pending)) &&
		unit.movement > 0.0 &&
		!unit.is_immovable &&
		unit.terraforming == 'none'
	);
};

const record_action_attempt = (unit, action_attempts) => {
	const unit_key = #to_string(unit.id);
	const previous = action_attempts[unit_key];
	action_attempts[unit_key] = {
		count: #is_defined(previous) ? previous.count + 1 : 1,
		pending: true,
		pending_checks: 0,
		tile: unit.get_tile(),
		movement: unit.movement + 0.0,
		health: unit.health + 0.0,
		moved_this_turn: unit.moved_this_turn == true,
		terraforming: '' + unit.terraforming,
	};
};

const refresh_pending_actions = (units, action_attempts) => {
	let waiting = false;
	for (unit of units) {
		const unit_key = #to_string(unit.id);
		const attempt = action_attempts[unit_key];
		if (!#is_defined(attempt) || !attempt.pending) {
			continue;
		}
		if (has_state_changed(unit, attempt)) {
			attempt.pending = false;
			attempt.pending_checks = 0;
		} else {
			attempt.pending_checks = attempt.pending_checks + 1;
			if (attempt.pending_checks >= MAX_PENDING_ACTION_CHECKS) {
				attempt.pending = false;
			} else {
				waiting = true;
			}
		}
	}
	return waiting;
};

const has_nearby_animation = (unit) => {
	const tile = unit.get_tile();
	if (tile.is_locked()) {
		return true;
	}
	for (candidate of tile.get_surrounding_tiles()) {
		if (candidate.is_locked()) {
			return true;
		}
	}
	return false;
};

return {
	can_attempt_action: can_attempt_action,
	record_action_attempt: record_action_attempt,
	refresh_pending_actions: refresh_pending_actions,
	has_nearby_animation: has_nearby_animation,
};
