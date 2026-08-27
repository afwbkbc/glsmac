const refresh_player_psych = (game, player) => {
	const get_psych = game.get('f_economy_get_base_psych');
	const process_psych = game.get('f_base_process_psych');
	if (!#is_defined(get_psych) || !#is_defined(process_psych)) {
		return;
	}
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id == player.id) {
			process_psych(game, base, get_psych(game, base));
		}
	}
};

return {

	validate: (e) => {
		if (
			#typeof(e.data.player) != 'Object' ||
			#typeof(e.data.player.get_social_engineering) != 'Callable' ||
			#typeof(e.data.player.set_social_engineering) != 'Callable' ||
			#typeof(e.data.player.get_energy_credits) != 'Callable' ||
			#typeof(e.data.player.set_energy_credits) != 'Callable'
		) {
			return 'Social engineering requires a player';
		}
		if (e.caller != 0 && e.data.player.id != e.caller) {
			return 'Players may only change their own social engineering choices';
		}
		if (e.game.is_turn_complete(e.data.player.id)) {
			return 'Player has already completed this turn';
		}
		const choices_error = e.game.get('f_social_validate_choices')(
			e.data.player,
			e.data.choices
		);
		if (#is_defined(choices_error)) {
			return choices_error;
		}
		const cost = e.game.get('f_social_get_adoption_cost')(
			e.data.player,
			e.data.choices
		);
		const available = e.data.player.get_energy_credits();
		if (available < cost) {
			return 'Social engineering upheaval costs ' + #to_string(cost) +
				' energy credits; only ' + #to_string(available) + ' available';
		}
	},

	apply: (e) => {
		const previous = {
			choices: e.data.player.get_social_engineering(),
			energy_credits: e.data.player.get_energy_credits(),
		};
		const cost = e.game.get('f_social_get_adoption_cost')(
			e.data.player,
			e.data.choices
		);
		e.data.player.set_social_engineering(e.data.choices);
		e.data.player.set_energy_credits(previous.energy_credits - cost);
		refresh_player_psych(e.game, e.data.player);
		e.game.trigger('social_engineering_updated', {
			player: e.data.player,
		});
		if (cost > 0) {
			e.game.trigger('economy_updated', {player: e.data.player});
		}
		return previous;
	},

	rollback: (e) => {
		const changed_energy = e.data.player.get_energy_credits() != e.applied.energy_credits;
		e.data.player.set_social_engineering(e.applied.choices);
		e.data.player.set_energy_credits(e.applied.energy_credits);
		refresh_player_psych(e.game, e.data.player);
		e.game.trigger('social_engineering_updated', {
			player: e.data.player,
		});
		if (changed_energy) {
			e.game.trigger('economy_updated', {player: e.data.player});
		}
	},

};
