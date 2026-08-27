const artifact_rules = #include('../artifact_rules');
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
		return artifact_rules.get_contribution_error(e.game, e.data.unit, e.caller);
	},

	resolve: (e) => {
		return {};
	},

	apply: (e) => {
		const unit = e.data.unit;
		const base = unit.get_tile().get_base();
		const target = artifact_rules.get_contribution_target(base);
		if (target == null) {
			throw Error('Alien Artifact has no valid production target');
		}
		const old_minerals = base.get_accumulated_minerals();
		const backup = snapshot_unit(unit);
		base.set_accumulated_minerals(
			old_minerals + artifact_rules.contribution_minerals
		);
		e.game.um.despawn_unit(unit);
		e.game.message(
			base.get_owner().name + ' has applied an Alien Artifact to ' +
			target.production.name + '.'
		);
		return {
			base: base,
			old_minerals: old_minerals,
			target_kind: target.kind,
			unit: backup,
		};
	},

	rollback: (e) => {
		e.applied.base.set_accumulated_minerals(e.applied.old_minerals);
		restore_unit(e.game, e.applied.unit);
	},
};
