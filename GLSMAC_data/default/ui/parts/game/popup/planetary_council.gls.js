return {
	init: (p) => {
		this.p = p;
		this.player = null;
		this.status_text = null;
		this.first_text = null;
		this.second_text = null;
		this.detail_text = null;
		this.vote_first_button = null;
		this.vote_second_button = null;
		this.abstain_button = null;
		this.governor_button = null;
		this.supreme_button = null;
		this.trade_button = null;
		this.unity_button = null;
		this.charter_button = null;
		this.solar_button = null;
		this.polar_button = null;

		const result = p.create('PLANETARY COUNCIL', 560, 332, (body, cb) => {
			this.status_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 12,
			});
			this.first_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 40,
			});
			this.second_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 66,
			});
			this.detail_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 94,
			});

			this.vote_first_button = body.button({
				class: 'game-popup-button', text: '', top: 122,
			});
			this.vote_first_button.on('click', (e) => {
				const session = p.game.get('f_council_get_session')();
				if (this.player != null && session != null) {
					p.game.event('cast_council_vote', {
						player: this.player, vote_id: session.candidate_a_id,
					});
				}
				return true;
			});

			this.vote_second_button = body.button({
				class: 'game-popup-button', text: '', top: 148,
			});
			this.vote_second_button.on('click', (e) => {
				const session = p.game.get('f_council_get_session')();
				if (this.player != null && session != null) {
					p.game.event('cast_council_vote', {
						player: this.player, vote_id: session.candidate_b_id,
					});
				}
				return true;
			});

			this.abstain_button = body.button({
				class: 'game-popup-button', text: 'Abstain', top: 174,
			});
			this.abstain_button.on('click', (e) => {
				if (this.player != null) {
					p.game.event('cast_council_vote', {player: this.player, vote_id: -1});
				}
				return true;
			});

			this.governor_button = body.button({
				class: 'game-popup-button', text: 'Convene Governor Election', top: 122,
			});
			this.governor_button.on('click', (e) => {
				if (this.player != null) {
					p.game.event('call_planetary_council', {
						player: this.player, proposal: 'governor',
					});
				}
				return true;
			});

			this.supreme_button = body.button({
				class: 'game-popup-button', text: 'Propose Supreme Leader', top: 148,
			});
			this.supreme_button.on('click', (e) => {
				if (this.player != null) {
					p.game.event('call_planetary_council', {
						player: this.player, proposal: 'supreme',
					});
				}
				return true;
			});

			this.trade_button = body.button({
				class: 'game-popup-button', text: '', top: 174,
			});
			this.trade_button.on('click', (e) => {
				if (this.player != null) {
					const has_trade_pact = p.game.get('f_council_has_global_trade_pact')();
					p.game.event('call_planetary_council', {
						player: this.player,
						proposal: has_trade_pact ? 'repeal_trade_pact' : 'trade_pact',
					});
				}
				return true;
			});

			this.unity_button = body.button({
				class: 'game-popup-button', text: 'Propose Salvage of Unity Fusion Core', top: 200,
			});
			this.unity_button.on('click', (e) => {
				if (this.player != null) {
					p.game.event('call_planetary_council', {
						player: this.player, proposal: 'salvage_unity_core',
					});
				}
				return true;
			});

			this.charter_button = body.button({
				class: 'game-popup-button', text: '', top: 226,
			});
			this.charter_button.on('click', (e) => {
				if (this.player != null) {
					const repealed = p.game.get('f_council_is_un_charter_repealed')();
					p.game.event('call_planetary_council', {
						player: this.player,
						proposal: repealed ? 'reinstate_un_charter' : 'repeal_un_charter',
					});
				}
				return true;
			});

			this.solar_button = body.button({
				class: 'game-popup-button', text: 'Propose Launch of Solar Shade', top: 252,
			});
			this.solar_button.on('click', (e) => {
				if (this.player != null) {
					p.game.event('call_planetary_council', {
						player: this.player, proposal: 'launch_solar_shade',
					});
				}
				return true;
			});

			this.polar_button = body.button({
				class: 'game-popup-button', text: 'Propose Melting of Polar Caps', top: 278,
			});
			this.polar_button.on('click', (e) => {
				if (this.player != null) {
					p.game.event('call_planetary_council', {
						player: this.player, proposal: 'melt_polar_caps',
					});
				}
				return true;
			});

			body.button({
				class: 'game-popup-button', text: 'Close', top: 304, is_cancel: true,
			}).on('click', (e) => {
				cb(false);
				return true;
			});
		});

		p.game.on('council_updated', (e) => {
			if (this.player == null) { this.player = p.game.get_player(); }
			this.refresh();
			const state = this.player.get_council_state();
			if (state.proposal != '' && state.vote_id == -2 && !p.modules.popup.is_shown()) {
				p.modules.popup.show('planetary_council');
			}
		});
		return result;
	},

	refresh: () => {
		this.vote_first_button.hide();
		this.vote_second_button.hide();
		this.abstain_button.hide();
		this.governor_button.hide();
		this.supreme_button.hide();
		this.trade_button.hide();
		this.unity_button.hide();
		this.charter_button.hide();
		this.solar_button.hide();
		this.polar_button.hide();
		if (this.player == null) { return; }

		const get_session = this.p.game.get('f_council_get_session');
		const get_tally = this.p.game.get('f_council_get_tally');
		const get_rankings = this.p.game.get('f_council_get_rankings');
		const get_governor = this.p.game.get('f_council_get_governor');
		const validate_call = this.p.game.get('f_council_validate_call');
		const session = get_session();
		if (session != null) {
			const tally = get_tally();
			const is_policy = this.p.game.get('f_council_is_policy_proposal')(
				session.proposal
			);
			if (is_policy) {
				this.status_text.text = 'Resolution: ' +
					this.p.game.get('f_council_get_proposal_name')(session.proposal);
				this.first_text.text = 'Yes: ' + #to_string(tally.candidate_a_votes) +
					' committed votes';
				this.second_text.text = 'No: ' + #to_string(tally.candidate_b_votes) +
					' committed votes';
			} else {
				const first = this.p.game.get_player(session.candidate_a_id);
				const second = this.p.game.get_player(session.candidate_b_id);
				this.status_text.text = session.proposal == 'supreme'
					? 'Election: Supreme Leader of Planet'
					: 'Election: Planetary Governor';
				this.first_text.text = first.get_faction().name + ': ' +
					#to_string(tally.candidate_a_votes) + ' committed votes';
				this.second_text.text = second.get_faction().name + ': ' +
					#to_string(tally.candidate_b_votes) + ' committed votes';
			}
			this.detail_text.text = #to_string(tally.required_votes) + ' of ' +
				#to_string(tally.total_votes) + ' votes required.';
			if (this.player.get_council_state().vote_id == -2) {
				if (is_policy) {
					this.vote_first_button.text = 'Vote Yes';
					this.vote_second_button.text = 'Vote No';
				} else {
					const first = this.p.game.get_player(session.candidate_a_id);
					const second = this.p.game.get_player(session.candidate_b_id);
					this.vote_first_button.text = 'Vote for ' + first.get_faction().name;
					this.vote_second_button.text = 'Vote for ' + second.get_faction().name;
				}
				this.vote_first_button.show();
				this.vote_second_button.show();
				this.abstain_button.show();
			} else {
				this.detail_text.text = this.detail_text.text + ' Your vote has been recorded.';
			}
			return;
		}

		const governor = get_governor();
		const has_trade_pact = this.p.game.get('f_council_has_global_trade_pact')();
		const unity_salvaged = this.p.game.get('f_council_has_salvaged_unity_core')();
		const charter_repealed = this.p.game.get('f_council_is_un_charter_repealed')();
		this.status_text.text = (governor == null
			? 'Planetary Governor: none elected'
			: 'Planetary Governor: ' + governor.get_faction().name) +
			(has_trade_pact ? ' | Trade: active' : ' | Trade: inactive') +
			(charter_repealed ? ' | Charter: repealed' : ' | Charter: active');
		const rankings = get_rankings();
		this.first_text.text = #sizeof(rankings) > 0
			? rankings[0].player.get_faction().name + ': ' +
				#to_string(rankings[0].votes) + ' votes'
			: 'No eligible candidates';
		this.second_text.text = #sizeof(rankings) > 1
			? rankings[1].player.get_faction().name + ': ' +
				#to_string(rankings[1].votes) + ' votes'
			: '';
		const governor_error = validate_call(this.player, 'governor');
		const supreme_error = validate_call(this.player, 'supreme');
		const trade_proposal = has_trade_pact ? 'repeal_trade_pact' : 'trade_pact';
		const trade_error = validate_call(this.player, trade_proposal);
		const unity_error = validate_call(this.player, 'salvage_unity_core');
		const charter_proposal = charter_repealed
			? 'reinstate_un_charter'
			: 'repeal_un_charter';
		const charter_error = validate_call(this.player, charter_proposal);
		const solar_error = validate_call(this.player, 'launch_solar_shade');
		const polar_error = validate_call(this.player, 'melt_polar_caps');
		let can_convene = false;
		if (!#is_defined(governor_error)) {
			this.governor_button.show();
			can_convene = true;
		}
		if (!#is_defined(supreme_error)) {
			this.supreme_button.show();
			can_convene = true;
		}
		if (!#is_defined(trade_error)) {
			this.trade_button.text = has_trade_pact
				? 'Propose Repeal of Global Trade Pact'
				: 'Propose Global Trade Pact';
			this.trade_button.show();
			can_convene = true;
		}
		if (!unity_salvaged && !#is_defined(unity_error)) {
			this.unity_button.show();
			can_convene = true;
		}
		if (!#is_defined(charter_error)) {
			this.charter_button.text = charter_repealed
				? 'Propose Reinstatement of U.N. Charter'
				: 'Propose Repeal of U.N. Charter';
			this.charter_button.show();
			can_convene = true;
		}
		if (!#is_defined(solar_error)) {
			this.solar_button.show();
			can_convene = true;
		}
		if (!#is_defined(polar_error)) {
			this.polar_button.show();
			can_convene = true;
		}
		if (can_convene) {
			this.detail_text.text = 'The Council is ready to convene.';
		} else if (#is_defined(governor_error)) {
			this.detail_text.text = governor_error;
		} else if (#is_defined(supreme_error)) {
			this.detail_text.text = supreme_error;
		} else if (#is_defined(trade_error)) {
			this.detail_text.text = trade_error;
		} else {
			this.detail_text.text = charter_error;
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
