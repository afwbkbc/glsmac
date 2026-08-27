const rules = #include('../unit_upgrade_rules');
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
		return rules.get_error(e.game, e.data.unit, e.caller, e.data.target_def_id);
	},

	resolve: (e) => {
		const player = e.game.get_player(e.caller);
		const source = e.data.unit.get_def();
		const target = rules.find_definition(e.game, e.data.target_def_id);
		return {
			target_def_id: target.id,
			cost: rules.get_cost(e.game, player, source, target),
		};
	},

	apply: (e) => {
		const player = e.game.get_player(e.caller);
		const source = e.data.unit;
		const target = rules.find_definition(e.game, e.resolved.target_def_id);
		const snapshot = snapshot_unit(source);
		const source_name = source.get_def().name;
		const old_energy = rules.get_energy_credits(player);
		e.game.um.despawn_unit(source);
		const upgraded = e.game.um.spawn_unit({
			id: snapshot.id,
			def: target.id,
			owner: player,
			tile: e.game.tm.get_tile(snapshot.tile_x, snapshot.tile_y),
			morale: snapshot.morale,
			health: snapshot.health,
			home_base_id: snapshot.home_base_id,
			fuel: #min(
				snapshot.fuel,
				#is_defined(target.operational_range) ? target.operational_range : 0
			),
		});
		upgraded.movement = 0.0;
		upgraded.moved_this_turn = true;
		upgraded.native_capture_attempted = false;
		player.set_energy_credits(old_energy - e.resolved.cost);
		e.game.trigger('economy_updated', {player: player});
		e.game.trigger('unit_upgraded', {
			player: player,
			unit: upgraded,
			previous_def_id: snapshot.def,
			cost: e.resolved.cost,
		});
		e.game.message(
			player.name + ' upgraded ' + source_name + ' to ' +
			target.name + ' for ' + #to_string(e.resolved.cost) + ' energy credits.'
		);
		return {
			unit: snapshot,
			energy_credits: old_energy,
		};
	},

	rollback: (e) => {
		if (e.game.um.has_unit(e.applied.unit.id)) {
			e.game.um.despawn_unit(e.game.um.get_unit(e.applied.unit.id));
		}
		const restored = restore_unit(e.game, e.applied.unit);
		const player = e.game.get_player(e.applied.unit.owner);
		player.set_energy_credits(e.applied.energy_credits);
		e.game.trigger('economy_updated', {player: player});
		e.game.trigger('unit_upgraded', {
			player: player,
			unit: restored,
			previous_def_id: e.resolved.target_def_id,
			cost: 0 - e.resolved.cost,
		});
	},
};
