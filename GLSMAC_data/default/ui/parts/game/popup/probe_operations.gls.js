return {
	init: (p) => {
		this.p = p;
		this.unit = null;
		this.targets = {};
		this.target = null;
		this.operation = null;
		this.target_select = null;
		this.operation_select = null;
		this.status_text = null;
		this.execute_button = null;

		return p.create('PROBE OPERATIONS', 500, 196, (body, cb) => {
			body.text({class: 'game-popup-text', text: 'Target:', left: 10, top: 12});
			this.target_select = body.select({
				class: 'popup-list-select', align: 'top right', right: 10, top: 8,
				width: 350, items: [['', 'No adjacent targets']], value: '',
			});
			this.target_select.on('select', (e) => {
				this.select_target(e.value);
				return true;
			});

			body.text({class: 'game-popup-text', text: 'Operation:', left: 10, top: 50});
			this.operation_select = body.select({
				class: 'popup-list-select', align: 'top right', right: 10, top: 46,
				width: 350, items: [['', 'No available operations']], value: '',
			});
			this.operation_select.on('select', (e) => {
				this.operation = e.value;
				this.refresh_status();
				return true;
			});

			this.status_text = body.text({
				class: 'game-popup-text', text: '', left: 10, right: 10, top: 88,
			});

			body.button({
				class: 'game-popup-button', text: 'Cancel', top: 148, is_cancel: true,
			}).on('click', (e) => {
				cb(false);
				return true;
			});

			this.execute_button = body.button({
				class: 'game-popup-button', text: 'Execute Operation', top: 172, is_ok: true,
			});
			this.execute_button.on('click', (e) => {
				if (this.unit != null && this.target != null && this.operation != null) {
					p.game.event('probe_operation', {
						unit: this.unit,
						operation: this.operation,
						target: this.target,
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

	get_target_player: () => {
		return #classof(this.target) == 'Base'
			? this.target.get_owner()
			: this.p.game.get_player(this.target.owner);
	},

	is_valid_unit_target: (unit) => {
		return unit.health > 0.0 &&
			(!#is_defined(unit.transport_id) || unit.transport_id == 0) &&
			(!#is_defined(unit.get_cargo) || #sizeof(unit.get_cargo()) == 0);
	},

	get_operation_items: () => {
		if (this.target == null || this.unit == null) {
			return [];
		}
		const actor = this.p.game.get_player();
		const target_player = this.get_target_player();
		if (this.p.game.get('f_probe_has_project')(
			target_player,
			'TheHunterSeekerAlgorithm'
		)) {
			return [];
		}
		const definitions = this.p.game.get('f_probe_get_operations')();
		let items = [];
		if (#classof(this.target) == 'Unit') {
			if (!this.is_valid_unit_target(this.target)) {
				return [];
			}
			const cost = this.p.game.get('f_probe_get_subversion_cost')(actor, this.target);
			if (cost != null) {
				items :+['subvert_unit', definitions.subvert_unit.name +
					' (' + #to_string(cost) + ' EC)'];
			}
			return items;
		}
		const has_intelligence = this.p.game.get('f_council_has_intelligence');
		if (!(#is_defined(has_intelligence)
			? has_intelligence(actor, target_player)
			: actor.has_infiltrated(target_player))) {
			items :+['infiltrate', definitions.infiltrate.name];
		}
		if (#sizeof(this.p.game.get('f_probe_get_unknown_technologies')(
			actor,
			target_player
		)) > 0) {
			items :+['steal_technology', definitions.steal_technology.name];
		}
		if (this.p.game.get('f_probe_can_sabotage')(this.target)) {
			items :+['sabotage', definitions.sabotage.name];
		}
		if (target_player.energy_credits > 0 && actor.energy_credits < 1000000000) {
			items :+['drain_energy', definitions.drain_energy.name];
		}
		if (this.p.game.get('f_probe_can_incite_drone_riots')(this.target)) {
			items :+['incite_drone_riots', definitions.incite_drone_riots.name];
		}
		if (
			this.unit.morale >= 3 &&
			this.p.game.get('f_probe_get_assassination_research_loss')(target_player) > 0
		) {
			items :+['assassinate_researchers', definitions.assassinate_researchers.name];
		}
		if (
			actor.has_technology('RetroviralEngineering') &&
			this.p.game.get('f_probe_get_plague_population_loss')(this.target) > 0
		) {
			items :+['genetic_plague', definitions.genetic_plague.name];
		}
		const cost = this.p.game.get('f_probe_get_mind_control_cost')(actor, this.target);
		if (cost != null) {
			items :+['mind_control_base', definitions.mind_control_base.name +
				' (' + #to_string(cost) + ' EC)'];
		}
		return items;
	},

	select_target: (key) => {
		this.target = key == '' || !#is_defined(this.targets[key]) ? null : this.targets[key];
		const items = this.get_operation_items();
		this.operation_select.items = #sizeof(items) > 0
			? items
			: [['', 'No available operations']];
		this.operation_select.readonly = #sizeof(items) == 0;
		this.operation_select.value = #sizeof(items) > 0 ? items[0][0] : '';
		this.operation = this.operation_select.value;
		this.refresh_status();
	},

	refresh_status: () => {
		this.execute_button.hide();
		if (this.target == null) {
			this.status_text.text = 'Move next to a foreign base or unit.';
			return;
		}
		const target_player = this.get_target_player();
		if (this.p.game.get('f_probe_has_project')(
			target_player,
			'TheHunterSeekerAlgorithm'
		)) {
			this.status_text.text = 'Blocked by the Hunter-Seeker Algorithm.';
			return;
		}
		if (this.operation == '') {
			this.status_text.text = 'No legal operation is available.';
			return;
		}
		const actor = this.p.game.get_player();
		let cost = 0;
		if (this.operation == 'subvert_unit') {
			cost = this.p.game.get('f_probe_get_subversion_cost')(actor, this.target);
		} else if (this.operation == 'mind_control_base') {
			cost = this.p.game.get('f_probe_get_mind_control_cost')(actor, this.target);
		}
		if (cost > actor.energy_credits) {
			this.status_text.text = 'Requires ' + #to_string(cost) +
				' energy credits; ' + #to_string(actor.energy_credits) + ' available.';
			return;
		}
		const chance = this.p.game.get('f_probe_get_success_chance')(
			this.unit,
			target_player,
			this.operation,
			this.target
		);
		this.status_text.text = cost > 0
			? 'Cost: ' + #to_string(cost) + ' energy credits.'
			: 'Success chance: ' + #to_string(chance) + '%.';
		this.execute_button.show();
	},

	on_show: () => {
		this.targets = {};
		let items = [];
		if (this.unit != null) {
			const player = this.p.game.get_player();
			for (tile of this.unit.get_tile().get_surrounding_tiles()) {
				const base = tile.get_base();
				if (base != null && base.get_owner().id != player.id) {
					const key = 'b' + #to_string(base.id);
					this.targets[key] = base;
					items :+[key, base.name + ' (' + base.get_owner().name + ')'];
				}
				for (unit of tile.get_units()) {
					if (unit.owner != player.id && this.is_valid_unit_target(unit)) {
						const key = 'u' + #to_string(unit.id);
						this.targets[key] = unit;
						items :+[key, unit.get_def().name + ' (' +
							this.p.game.get_player(unit.owner).name + ')'];
					}
				}
			}
		}
		this.target_select.items = #sizeof(items) > 0
			? items
			: [['', 'No adjacent targets']];
		this.target_select.readonly = #sizeof(items) == 0;
		this.target_select.value = #sizeof(items) > 0 ? items[0][0] : '';
		this.select_target(this.target_select.value);
	},

	on_hide: () => {
		this.unit = null;
		this.target = null;
		this.operation = null;
		this.targets = {};
	},
};
