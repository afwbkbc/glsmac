return {

	ITEM_WIDTH: 132,
	ITEM_HEIGHT: 105,
	ITEM_PADDING: 3,

	selected_production: null,
	pending_production: null,

	selected_item: null,
	pending_item: null,

	init: (p) => {

		this.p = p;

		this.area = p.body.panel({
			class: 'default-panel',
			top: 0,
			left: 0,
			right: 0,
			bottom: 0,
		});
		this.area.hide();

		return this.area;
	},

	show: () => {
		this.area.show();
	},

	hide: () => {
		this.area.hide();
	},

	reset: () => {
		if (
			this.selected_production != null &&
			this.selected_item != null &&
			this.selected_production != this.pending_production
		) {
			this.pending_item.class = 'base-screen-production-item';
			this.pending_item = this.selected_item;
			this.pending_item.class = 'base-screen-production-item-active';
			this.pending_production = this.selected_production;
		}
	},

	set: (data) => {
		this.area.clear();
		const production_choices = data.base.get_production_choices();
		let x = this.ITEM_PADDING;
		let y = this.ITEM_PADDING;

		this.selected_production = data.base.get_production().id;
		this.pending_production = this.selected_production;

		for (choice of production_choices) {

			if (choice.type == 'facility') {
				if (data.base.has_facility(choice.facility_id)) {
					// skip existing buildings
					continue;
				}
			}

			const id = choice.id;
			const is_selected = id == this.selected_production;

			const panel = this.area.panel({
				class: is_selected ? 'base-screen-production-item-active' : 'base-screen-production-item',
				left: x,
				top: y,
			});
			if (is_selected) {
				this.selected_item = panel;
				this.pending_item = panel;
			}

			const c = this.p.INTENT_COLORS[choice.intent];

			panel.text({
				class: 'base-screen-production-title',
				text: choice.name,
				color: c,
			});

			panel.surface({
				class: 'base-screen-production-preview',
				background: choice.texture + ':tint(' + c + ',1.0):stretch',
			});

			let eta = '';
			if (#is_defined(choice.cost)) {
				let minerals_left = choice.cost - data.base.get('accumulated_minerals');
				if (minerals_left < 0) {
					minerals_left = 0;
				}
				const turns = #ceil(#to_float(minerals_left) / #to_float(data.minerals_per_turn));
				if (turns != 1) {
					eta = #to_string(turns) + ' turns';
				} else {
					eta = #to_string(turns) + ' turn';
				}
			} else if (id == 'STOCKPILE_ENERGY') {
				eta = #to_string(#floor(#to_float(data.minerals_per_turn) / 2.0)) + ' Energy per turn';
			}

			panel.text({
				class: 'base-screen-production-eta',
				text: eta,
			});

			panel.text({
				class: 'base-screen-production-hint',
				text: choice.hint,
			});

			panel.on('mousedown', (e) => {
				if (id != this.pending_production) {
					if (this.pending_item != null) {
						this.pending_item.class = 'base-screen-production-item';
					}
					this.pending_item = panel;
					this.pending_item.class = 'base-screen-production-item-active';
					this.pending_production = id;
				}
				return true;
			});

			x += this.ITEM_WIDTH + this.ITEM_PADDING;
		}
	},

};
