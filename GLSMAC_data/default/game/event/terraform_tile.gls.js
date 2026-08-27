const terraforming = #include('../../units/terraforming');
const unit_abilities = #include('../unit_abilities');

return {

	validate: (e) => {
		const unit = e.data.unit;
		if (#typeof(e.data.type) != 'String') {
			return 'Terraforming order must be identified by name';
		}
		const order = terraforming.get_order(e.data.type);
		if (order == null) {
			return 'Unknown terraforming order';
		}
		if (unit.owner != e.caller) {
			return 'A Former can only be ordered by its owner';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (unit.health <= 0.0) {
			return 'Dead unit cannot terraform';
		}
		if (!unit.get_def().can_terraform) {
			return 'Only Formers can terraform terrain';
		}
		if (unit.terraforming != 'none') {
			return 'Former already has a terraforming order';
		}
		if (unit.movement <= 0.0) {
			return 'Former is out of moves';
		}

		const tile = unit.get_tile();
		if (tile.is_locked()) {
			return 'Terraforming site is locked';
		}
		const unavailable = terraforming.get_unavailable_reason(
			tile,
			unit.get_owner(),
			e.data.type
		);
		if (unavailable != null) {
			return unavailable;
		}
		for (other of tile.get_units()) {
			if (other.id != unit.id && other.terraforming != 'none') {
				return 'Another Former is already working this tile';
			}
		}
	},

	resolve: (e) => {
		// Apply after server acceptance so rejected client orders cannot advance local state.
		return {};
	},

	apply: (e) => {
		const unit = e.data.unit;
		const previous = {
			type: '' + unit.terraforming,
			turns: unit.terraforming_turns_remaining + 0,
			movement: unit.movement + 0.0,
			moved_this_turn: unit.moved_this_turn == true,
		};
		const order = terraforming.get_order(e.data.type);
		const get_effects = #is_defined(e.game.get)
			? e.game.get('f_project_get_player_effects')
			: #undefined;
		const effects = #is_defined(get_effects)
			? get_effects(unit.get_owner())
			: {terraforming_rate_multiplier: 1.0};
		const fungus_rate_multiplier = (
			e.data.type == 'remove_fungus' || e.data.type == 'plant_fungus'
		) && #is_defined(effects.fungus_terraforming_rate_multiplier)
			? effects.fungus_terraforming_rate_multiplier
			: 1.0;
		const rate_multiplier = effects.terraforming_rate_multiplier *
			fungus_rate_multiplier *
			unit_abilities.get_terraforming_rate_multiplier(unit, e.data.type);
		const turns = #max(
			#ceil(#to_float(order.turns) / rate_multiplier),
			1
		);
		unit.set_terraforming_order(e.data.type, turns);
		unit.movement = 0.0;
		unit.moved_this_turn = true;
		return previous;
	},

	rollback: (e) => {
		const unit = e.data.unit;
		unit.set_terraforming_order(e.applied.type, e.applied.turns);
		unit.movement = e.applied.movement;
		unit.moved_this_turn = e.applied.moved_this_turn;
	},

};
