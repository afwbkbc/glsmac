const psi_gate_rules = #include('../psi_gate_rules');

return {
	validate: (e) => {
		return psi_gate_rules.get_teleport_error(
			e.game,
			e.data.unit,
			e.caller,
			e.data.destination
		);
	},

	resolve: (e) => {
		return {};
	},

	apply: (e) => {
		const unit = e.data.unit;
		const source = unit.get_tile().get_base();
		const destination = e.data.destination;
		const result = {
			source: source,
			destination: destination,
			source_used_turn: psi_gate_rules.snapshot_used_turn(source),
			destination_used_turn: psi_gate_rules.snapshot_used_turn(destination),
		};
		source.set(psi_gate_rules.used_turn_key, e.game.get_turn());
		destination.set(psi_gate_rules.used_turn_key, e.game.get_turn());
		unit.teleport_to_tile(destination.get_tile());
		return result;
	},

	rollback: (e) => {
		e.data.unit.teleport_to_tile(e.applied.source.get_tile());
		psi_gate_rules.restore_used_turn(
			e.applied.source,
			e.applied.source_used_turn
		);
		psi_gate_rules.restore_used_turn(
			e.applied.destination,
			e.applied.destination_used_turn
		);
	},
};
