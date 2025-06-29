return {

	validate: (e) => {
		if (e.game.is_started()) {
			return 'Game has already started';
		}
	},

	apply: (e) => {
		const player = e.game.get_player(e.caller);
		const old_faction = player.get_faction();
		if (e.data.faction != 'RANDOM') {
			player.set_faction_by_id(e.data.faction);
		} else {
			player.unset_faction();
		}
		e.game.trigger('player_update', {
			player: player,
		});
		if (#is_defined(old_faction)) {
			return {
				faction: old_faction.id,
			}
		} else {
			return {
				faction: 'RANDOM',
			}
		}
	},

	rollback: (e) => {
		const player = e.game.get_player(e.caller);
		if (e.applied.faction != 'RANDOM') {
			player.set_faction_by_id(e.applied.faction);
		} else {
			player.unset_faction();
		}
		e.game.trigger('player_update', {
			player: player,
		});
	},

};
