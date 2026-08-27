return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to initialize research';
		}
	},

	apply: (e) => {
		const previous = e.data.player.get_research_state();
		e.data.player.set_research_state(e.data.state);
		e.game.trigger('research_updated', {
			player: e.data.player,
		});
		return previous;
	},

	rollback: (e) => {
		e.data.player.set_research_state(e.applied);
		e.game.trigger('research_updated', {
			player: e.data.player,
		});
	},

};
