const artifact_rules = #include('../artifact_rules');
const technology_acquisition = #include('../technology_acquisition');
const snapshot_unit = #include('../entity_snapshots').snapshot_unit;

const restore_unit = (game, snapshot) => {
	const unit = game.um.spawn_unit({
		id: snapshot.id,
		def: snapshot.def,
		owner: game.get_player(snapshot.owner),
		tile: game.tm.get_tile(snapshot.tile_x, snapshot.tile_y),
		morale: snapshot.morale,
		health: snapshot.health,
		terraforming: snapshot.terraforming,
		terraforming_turns_remaining: snapshot.terraforming_turns_remaining,
		home_base_id: snapshot.home_base_id,
		fuel: snapshot.fuel,
		transport_id: snapshot.transport_id,
	});
	unit.movement = snapshot.movement;
	unit.moved_this_turn = snapshot.moved_this_turn;
	unit.native_capture_attempted = snapshot.native_capture_attempted;
	return unit;
};

return {
	validate: (e) => {
		return artifact_rules.get_study_error(e.game, e.data.unit, e.caller);
	},

	resolve: (e) => {
		return {};
	},

	apply: (e) => {
		const unit = e.data.unit;
		const base = unit.get_tile().get_base();
		const method = artifact_rules.get_study_method(base);
		const linked_state = {
			defined: base.has(artifact_rules.linked_key),
			value: base.get(artifact_rules.linked_key),
		};
		if (method == 'network_node') {
			base.set(artifact_rules.linked_key, true);
		}
		const acquired = technology_acquisition.apply(
			e.game,
			base.get_owner(),
			1
		);
		if (!#is_defined(acquired)) {
			throw Error('Alien Artifact did not discover a technology');
		}
		const backup = snapshot_unit(unit);
		e.game.um.despawn_unit(unit);
		for (name of acquired.completed_names) {
			e.game.message(
				base.get_owner().name + ' has decoded ' + name +
					' from an Alien Artifact.'
			);
		}
		return {
			base: base,
			method: method,
			linked_state: linked_state,
			research: acquired,
			unit: backup,
		};
	},

	rollback: (e) => {
		technology_acquisition.rollback(e.game, e.applied.research);
		if (e.applied.linked_state.defined) {
			e.applied.base.set(
				artifact_rules.linked_key,
				e.applied.linked_state.value
			);
		} else {
			e.applied.base.unset(artifact_rules.linked_key);
		}
		restore_unit(e.game, e.applied.unit);
	},
};
