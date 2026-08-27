return {

	validate: (e) => {
		if (e.game.is_started()) {
			return 'Game has already started';
		}
		if (#typeof(e.data.faction) != 'String') {
			return 'Faction must be identified by name';
		}
		if (e.data.faction == 'RANDOM') {
			return;
		}
		let faction_exists = false;
		for (faction of e.game.get_fm().list()) {
			if (faction.id == e.data.faction) {
				if (#is_defined(faction.is_native) && faction.is_native) {
					return 'Planet cannot be selected as a playable faction';
				}
				faction_exists = true;
				break;
			}
		}
		if (!faction_exists) {
			return 'Unknown faction: ' + e.data.faction;
		}
		for (player of e.game.get_players()) {
			if (player.id != e.caller) {
				const faction = player.get_faction();
				if (#is_defined(faction) && faction.id == e.data.faction) {
					return 'Faction is already selected by another player';
				}
			}
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
