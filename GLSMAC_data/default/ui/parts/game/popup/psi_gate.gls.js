const psi_gate_rules = #include('../../../../game/psi_gate_rules');

return {
	init: (p) => {
		this.p = p;
		this.unit = null;
		this.destinations = {};
		this.destination = null;
		this.destination_select = null;
		this.teleport_button = null;
		this.status_text = null;

		return p.create('PSI GATE', 500, 148, (body, cb) => {
			body.text({class: 'game-popup-text', text: 'Destination:', left: 10, top: 12});
			this.destination_select = body.select({
				class: 'popup-list-select', align: 'top right', right: 10, top: 8,
				width: 350, items: [['', 'No available Psi Gates']], value: '',
			});
			this.destination_select.on('select', (e) => {
				this.select_destination(e.value);
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

			this.teleport_button = body.button({
				class: 'game-popup-button', text: 'Teleport Unit', top: 124, is_ok: true,
			});
			this.teleport_button.on('click', (e) => {
				if (this.unit != null && this.destination != null) {
					p.game.event('teleport_unit', {
						unit: this.unit,
						destination: this.destination,
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

	select_destination: (id) => {
		this.destination = id == '' || !#is_defined(this.destinations[id])
			? null
			: this.destinations[id];
		this.teleport_button.hide();
		this.status_text.text = this.destination == null
			? 'No available Psi Gate destination.'
			: 'Destination: ' + this.destination.name;
		if (this.destination != null) {
			this.teleport_button.show();
		}
	},

	on_show: () => {
		this.destinations = {};
		let items = [];
		if (this.unit != null) {
			const player = this.p.game.get_player();
			for (base of psi_gate_rules.get_available_destinations(
				this.p.game,
				this.unit,
				player.id
			)) {
				const key = #to_string(base.id);
				this.destinations[key] = base;
				items :+[key, base.name];
			}
		}
		this.destination_select.items = #sizeof(items) > 0
			? items
			: [['', 'No available Psi Gates']];
		this.destination_select.readonly = #sizeof(items) == 0;
		this.destination_select.value = #sizeof(items) > 0 ? items[0][0] : '';
		this.select_destination(this.destination_select.value);
	},

	on_hide: () => {
		this.unit = null;
		this.destination = null;
		this.destinations = {};
	},
};
