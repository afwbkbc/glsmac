return {

	validate: (e) => {

	},

	apply: (e) => {
		e.data.player.add_production_choice(e.data.id, e.data.data);
		return {
			player: e.data.player,
			id: e.data.id,
		};
	},

	rollback: (e) => {
		e.applied.player.remove_production_choice(e.applied.id);
	},

};
