return {
	init: (p) => {
		this.p = p;
		this.unit = null;
		this.targets = {};
		this.target = null;
		this.target_select = null;
		this.status_text = null;
		this.upgrade_button = null;

		return p.create('UNIT UPGRADE', 500, 148, (body, cb) => {
			body.text({class: 'game-popup-text', text: 'Design:', left: 10, top: 12});
			this.target_select = body.select({
				class: 'popup-list-select', align: 'top right', right: 10, top: 8,
				width: 350, items: [['', 'No compatible prototypes']], value: '',
			});
			this.target_select.on('select', (e) => {
				this.select_target(e.value);
				return true;
			});

			this.status_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 50,
			});

			body.button({
				class: 'game-popup-button', text: 'Cancel', top: 100, is_cancel: true,
			}).on('click', (e) => {
				cb(false);
				return true;
			});

			this.upgrade_button = body.button({
				class: 'game-popup-button', text: 'Upgrade Unit', top: 124, is_ok: true,
			});
			this.upgrade_button.on('click', (e) => {
				if (this.unit != null && this.target != null) {
					p.game.event('upgrade_unit', {
						unit: this.unit,
						target_def_id: this.target.id,
					});
					cb(true);
				}
				return true;
			});
		});
	},

	set: (data) => {
		this.unit = data.unit;
	},

	select_target: (id) => {
		this.target = id == '' || !#is_defined(this.targets[id])
			? null
			: this.targets[id];
		this.refresh_status();
	},

	refresh_status: () => {
		this.upgrade_button.hide();
		if (this.unit == null || this.target == null) {
			this.status_text.text = 'No compatible prototyped design is available.';
			return;
		}
		const player = this.p.game.get_player();
		const error = this.p.game.get('f_unit_upgrade_get_error')(
			this.unit,
			player.id,
			this.target.id
		);
		if (#is_defined(error)) {
			this.status_text.text = error;
			return;
		}
		const cost = this.p.game.get('f_unit_upgrade_get_cost')(
			player,
			this.unit.get_def(),
			this.target
		);
		this.status_text.text = 'Cost: ' + #to_string(cost) + ' energy credits. ' +
			#to_string(player.get_energy_credits()) + ' available.';
		this.upgrade_button.show();
	},

	on_show: () => {
		this.targets = {};
		let items = [];
		if (this.unit != null) {
			const player = this.p.game.get_player();
			for (target of this.p.game.get('f_unit_upgrade_get_targets')(
				player,
				this.unit.get_def()
			)) {
				const cost = this.p.game.get('f_unit_upgrade_get_cost')(
					player,
					this.unit.get_def(),
					target
				);
				this.targets[target.id] = target;
				items :+[target.id, target.name + ' (' + #to_string(cost) + ' EC)'];
			}
		}
		this.target_select.items = #sizeof(items) > 0
			? items
			: [['', 'No compatible prototypes']];
		this.target_select.readonly = #sizeof(items) == 0;
		this.target_select.value = #sizeof(items) > 0 ? items[0][0] : '';
		this.select_target(this.target_select.value);
	},

	on_hide: () => {
		this.unit = null;
		this.target = null;
		this.targets = {};
	},
};
