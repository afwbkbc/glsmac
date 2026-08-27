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

return {

	validate: (e) => {
		if (e.caller != e.data.base.get_owner().id) {
			return 'Only base owner can remove queued production';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (#typeof(e.data.index) != 'Int') {
			return 'Production queue index must be an integer';
		}
		if (e.data.index < 0 || e.data.index >= #sizeof(e.data.base.get_production_queue())) {
			return 'Production queue index is out of bounds';
		}
	},

	apply: (e) => {
		const old_queue = get_queue_specs(e.data.base);
		e.data.base.remove_production(e.data.index);
		return {
			old_queue: old_queue,
		};
	},

	rollback: (e) => {
		e.data.base.set_production_queue(e.applied.old_queue);
	},

};
