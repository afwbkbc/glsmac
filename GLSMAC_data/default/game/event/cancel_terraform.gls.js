return {

	validate: (e) => {
		const unit = e.data.unit;
		if (unit.owner != e.caller) {
			return 'A Former can only be ordered by its owner';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (unit.health <= 0.0) {
			return 'Dead unit cannot cancel terraforming';
		}
		if (unit.terraforming == 'none') {
			return 'Former has no terraforming order to cancel';
		}
	},

	resolve: (e) => {
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
		unit.set_terraforming_order('none', 0);
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
