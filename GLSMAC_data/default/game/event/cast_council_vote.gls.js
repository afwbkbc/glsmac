const rules = #include('../council_rules');

return {
	validate: (e) => {
		if (
			#typeof(e.data.player) != 'Object' ||
			#typeof(e.data.player.get_council_state) != 'Callable' ||
			#typeof(e.data.player.set_council_state) != 'Callable'
		) {
			return 'Planetary Council vote requires a player';
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only cast their own Planetary Council vote';
		}
		if (#typeof(e.data.vote_id) != 'Int') {
			return 'Planetary Council vote ID is invalid';
		}
		return rules.validate_vote(e.game, e.data.player, e.data.vote_id);
	},

	apply: (e) => {
		const previous = e.data.player.get_council_state();
		const updated = #clone(previous);
		updated.vote_id = e.data.vote_id;
		e.data.player.set_council_state(updated);
		e.game.trigger('council_updated', {player: e.data.player});
		return previous;
	},

	rollback: (e) => {
		e.data.player.set_council_state(e.applied);
		e.game.trigger('council_updated', {player: e.data.player});
	},
};
