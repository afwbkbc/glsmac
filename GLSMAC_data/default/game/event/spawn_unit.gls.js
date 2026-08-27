return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to spawn units';
		}
	},

	apply: (e) => {
		let data = {
			def: e.data.type,
			owner: e.data.owner,
			tile: e.data.tile,
			morale: e.data.morale,
			health: e.data.health,
		};
		if (#is_defined(e.data.home_base_id)) {
			data.home_base_id = e.data.home_base_id;
		}
		else if (#is_defined(e.data.home_base_at_tile) && e.data.home_base_at_tile) {
			for (base of e.game.bm.get_bases()) {
				const base_tile = base.get_tile();
				if (
					base.get_owner().id == e.data.owner.id &&
					base_tile.x == e.data.tile.x &&
					base_tile.y == e.data.tile.y
				) {
					data.home_base_id = base.id;
					break;
				}
			}
		}
		if (#is_defined(e.data.fuel)) {
			data.fuel = e.data.fuel;
		}
		if (#is_defined(e.data.transport_id)) {
			data.transport_id = e.data.transport_id;
		}
		const unit = e.game.um.spawn_unit(data);
		if (#is_defined(e.data.movement)) {
			unit.movement = e.data.movement;
		}
		if (#is_defined(e.data.moved_this_turn)) {
			unit.moved_this_turn = e.data.moved_this_turn;
		}

		return {
			unit: unit,
		};
	},

	rollback: (e) => {
		e.game.um.despawn_unit(e.applied.unit);
	},

};
