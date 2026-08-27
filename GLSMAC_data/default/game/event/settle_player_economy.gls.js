const MAX_LIQUIDATIONS_PER_TURN = 1024;
const MAX_ENERGY_CREDITS = 1000000000;

return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to settle player economy';
		}
		if (
			#typeof(e.data.liquidation_count) != 'Int' ||
			e.data.liquidation_count < 0 ||
			e.data.liquidation_count > MAX_LIQUIDATIONS_PER_TURN
		) {
			return 'Facility liquidation count is invalid';
		}
	},

	apply: (e) => {
		if (!e.game.is_master()) {
			return;
		}
		const player = e.data.player;
		const economy = e.game.get('f_economy_get_player')(e.game, player);
		if (player.energy_credits + economy < 0) {
			const candidate = e.game.get('f_economy_get_liquidation_candidate')(e.game, player);
			if (candidate != null) {
				if (e.data.liquidation_count >= MAX_LIQUIDATIONS_PER_TURN) {
					throw Error('Facility liquidation limit exceeded');
				}
				e.game.event('liquidate_base_facility', {
					base: candidate.base,
					facility_id: candidate.facility.id,
				});
				e.game.event('settle_player_economy', {
					player: player,
					liquidation_count: e.data.liquidation_count + 1,
				});
				return;
			}
		}
		const updated = #min(
			MAX_ENERGY_CREDITS,
			#max(0, player.energy_credits + economy)
		);
		e.game.event('process_player_economy', {
			player: player,
			energy_credits: updated,
		});
	},

	rollback: (e) => {
		// This host-only orchestration event does not mutate game state directly.
	},

};
