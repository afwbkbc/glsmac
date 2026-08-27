const MAX_ENERGY_CREDITS = 1000000000;

return {
	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to process player economy';
		}
		if (
			#typeof(e.data.energy_credits) != 'Int' ||
			e.data.energy_credits < 0 ||
			e.data.energy_credits > MAX_ENERGY_CREDITS
		) {
			return 'Energy credits must be a non-negative whole number within the supported range';
		}
	},

	apply: (e) => {
		const previous = e.data.player.energy_credits;
		e.data.player.set_energy_credits(e.data.energy_credits);
		e.game.trigger('economy_updated', {player: e.data.player});
		return {energy_credits: previous};
	},

	rollback: (e) => {
		e.data.player.set_energy_credits(e.applied.energy_credits);
		e.game.trigger('economy_updated', {player: e.data.player});
	},
};
