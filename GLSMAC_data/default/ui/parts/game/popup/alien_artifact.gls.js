const artifact_rules = #include('../../../../game/artifact_rules');

return {
	init: (p) => {
		this.p = p;
		this.unit = null;
		this.study_button = null;
		this.contribute_button = null;
		this.status_text = null;

		return p.create('ALIEN ARTIFACT', 500, 146, (body, cb) => {
			this.status_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 12,
			});

			this.study_button = body.button({
				class: 'game-popup-button', text: 'Discover Technology', top: 74,
			});
			this.study_button.on('click', (e) => {
				if (this.unit != null) {
					p.game.event('study_alien_artifact', {unit: this.unit});
					cb(true);
				}
				return true;
			});

			this.contribute_button = body.button({
				class: 'game-popup-button', text: 'Contribute 50 Minerals', top: 98,
			});
			this.contribute_button.on('click', (e) => {
				if (this.unit != null) {
					p.game.event('contribute_alien_artifact', {unit: this.unit});
					cb(true);
				}
				return true;
			});

			body.button({
				class: 'game-popup-button', text: 'Cancel', top: 122, is_cancel: true,
			}).on('click', (e) => {
				cb(false);
				return true;
			});
		});
	},

	set: (data) => {
		this.unit = data.unit;
	},

	on_show: () => {
		this.study_button.hide();
		this.contribute_button.hide();
		this.status_text.text = 'No valid use is currently available.';
		if (this.unit == null) {
			return;
		}
		const game = this.p.game;
		const player = game.get_player();
		if (!#is_defined(artifact_rules.get_study_error(game, this.unit, player.id))) {
			this.study_button.show();
			this.status_text.text = 'Alien Artifact at ' +
				this.unit.get_tile().get_base().name;
		}
		if (!#is_defined(artifact_rules.get_contribution_error(
			game,
			this.unit,
			player.id
		))) {
			const target = artifact_rules.get_contribution_target(
				this.unit.get_tile().get_base()
			);
			this.contribute_button.text = 'Contribute 50 Minerals to ' +
				target.production.name;
			this.contribute_button.show();
			this.status_text.text = 'Alien Artifact at ' +
				this.unit.get_tile().get_base().name;
		}
	},

	on_hide: () => {
		this.unit = null;
	},
};
