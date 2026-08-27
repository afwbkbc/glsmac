return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to process economic sanctions';
		}
		if (e.data.player.get_sanction_turns() <= 0) {
			return 'Player has no economic sanctions to process';
		}
	},

	apply: (e) => {
		const previous = e.data.player.get_sanction_turns();
		const updated = previous - 1;
		e.data.player.set_sanction_turns(updated);
		e.game.trigger('diplomatic_sanctions_updated', {
			player: e.data.player,
			turns: updated,
		});
		if (updated == 0) {
			e.game.message('Economic sanctions against ' + e.data.player.name + ' have expired.');
		}
		return {turns: previous};
	},

	rollback: (e) => {
		e.data.player.set_sanction_turns(e.applied.turns);
		e.game.trigger('diplomatic_sanctions_updated', {
			player: e.data.player,
			turns: e.applied.turns,
		});
	},

};
