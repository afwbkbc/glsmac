return {

	init: (p) => {
		this.p = p;
		this.player = null;
		this.status_text = null;
		this.detail_text = null;
		this.action_button = null;

		return p.create('GLOBAL ENERGY MARKET', 520, 144, (body, cb) => {
			this.status_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 14,
			});
			this.detail_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 42,
			});

			body.button({
				class: 'game-popup-button', text: 'Close', top: 96, is_cancel: true,
			}).on('click', (e) => {
				cb(false);
				return true;
			});

			this.action_button = body.button({
				class: 'game-popup-button',
				text: 'Corner Global Energy Market',
				top: 120,
				is_ok: true,
			});
			this.action_button.on('click', (e) => {
				if (this.player != null) {
					p.game.event('corner_global_energy_market', {player: this.player});
					cb(true);
				}
				return true;
			});
		});
	},

	refresh: () => {
		this.action_button.hide();
		if (this.player == null) {
			return;
		}
		const get_state = this.p.game.get('f_economic_victory_get_state');
		const get_cost = this.p.game.get('f_economic_victory_get_cost');
		const get_headquarters = this.p.game.get('f_economic_victory_get_headquarters');
		const state = get_state(this.player);
		if (state != null) {
			this.status_text.text = 'Market bid active: ' + #to_string(state.cost) + ' EC committed.';
			this.detail_text.text = 'Completion date: M.Y. ' + #to_string(state.turn + 2100) + '.';
			return;
		}
		if (!this.player.has_technology('PlanetaryEconomics')) {
			this.status_text.text = 'Planetary Economics has not been discovered.';
			this.detail_text.text = '';
			return;
		}
		if (get_headquarters(this.player) == null) {
			this.status_text.text = 'A Headquarters is required to initiate a market bid.';
			this.detail_text.text = '';
			return;
		}
		const cost = get_cost(this.player);
		this.status_text.text = 'Required capital: ' + #to_string(cost) + ' EC.';
		this.detail_text.text = 'Energy reserves: ' +
			#to_string(this.player.get_energy_credits()) + ' EC.';
		if (
			!this.p.game.is_game_over() &&
			!this.p.game.is_turn_complete(this.player.id) &&
			this.player.get_energy_credits() >= cost
		) {
			this.action_button.show();
		}
	},

	on_show: () => {
		this.player = this.p.game.get_player();
		this.refresh();
	},

	on_hide: () => {
		this.player = null;
	},

};
