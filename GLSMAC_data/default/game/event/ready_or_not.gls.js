return {

	validate: (e) => {
		if (e.game.is_started()) {
			return 'Game has already started';
		}
	},

	apply: (e) => {
		const player = e.game.get_player(e.caller);
		const was_ready = player.is_ready();
		if (was_ready != e.data.ready) {
			player.set_ready(e.data.ready);
			return {
				was_ready: was_ready,
			};
		}
	},

	rollback: (e) => {
		const player = e.game.get_player(e.caller);
		if (player.is_ready() != e.applied.was_ready) {
			player.set_ready(e.applied.was_ready);
		}
	},

};
