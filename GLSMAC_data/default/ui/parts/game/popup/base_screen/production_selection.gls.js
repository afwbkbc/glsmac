return {

	ITEM_WIDTH: 132,
	ITEM_HEIGHT: 105,

	ITEM_PADDING: 3,

	ITEMS_IN_ROW: 5,

	base: null,

	visible: false,

	selected_production: null,
	pending_production: null,

	selected_item: null,
	pending_item: null,

	grid: [],
	current_grid_x: 0,
	current_grid_y: 0,
	initial_grid_x: 0,
	initial_grid_y: 0,

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

		p.body.on('keydown', (e) => {
			if (!this.visible) {
				return false;
			}
			let newx = null;
			let newy = null;
			switch (e.code) {
				case 'LEFT': {
					if ( this.current_grid_x > 0 ) {
						newx = this.current_grid_x - 1;
					}
					newy = this.current_grid_y;
					break;
				}
				case 'RIGHT': {
					if ( this.current_grid_x < this.ITEMS_IN_ROW - 1 ) {
						newx = this.current_grid_x + 1;
					}
					newy = this.current_grid_y;
					break;
				}
				case 'UP': {
					newx = this.current_grid_x;
					if ( this.current_grid_y > 0 ) {
						newy = this.current_grid_y - 1;
					}
					break;
				}
				case 'DOWN': {
					newx = this.current_grid_x;
					newy = this.current_grid_y + 1;
					break;
				}
				default: {
					return false;
				}
			}
			if ( newx != null && newy != null ) {
				const idx = newy * this.ITEMS_IN_ROW + newx;
				if ( idx < #sizeof( this.grid) ) {
					this.grid[ idx ].trigger( 'mousedown' );
				}
			}
			return true;
		});

		return this.area;
	},

	show: () => {
		this.visible = true;
		this.area.show();
	},

	hide: () => {
		this.area.hide();
		this.visible = false;
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
			this.current_grid_x = this.initial_grid_x;
			this.current_grid_y = this.initial_grid_y;
		}
	},

	submit: () => {
		if (this.selected_production != this.pending_production) {
			this.p.game.event('set_base_production', {
				base: parent.base,
				id: parent.pending_production,
			});
			this.reset();
		}
		this.p.utils.hide_production_selection();
	},

	set: (data) => {

		this.base = data.base;

		this.grid = [];

		this.area.clear();

		const production_choices = this.base.get_production_choices();
		let x = this.ITEM_PADDING;
		let y = this.ITEM_PADDING;

		let grid_x = 0;
		let grid_y = 0;

		this.selected_production = this.base.get_production().id;
		this.pending_production = this.selected_production;

		for (choice of production_choices) {

			if (choice.type == 'facility') {
				if (this.base.has_facility(choice.facility_id)) {
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

			if ( choice.type == 'unit' ) {
				// unit textures have different aspect ratio, also no tint
				panel.surface({
					class: 'base-screen-production-preview-unit',
					background: choice.texture + ':stretch',
				});
			}
			else {
				panel.surface({
					class: 'base-screen-production-preview',
					background: choice.texture + ':tint(' + c + ',1.0):stretch',
				});
			}

			let eta = '';
			if (#is_defined(choice.cost)) {
				let minerals_left = choice.cost - this.base.get('accumulated_minerals');
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

			const panel_grid_x = grid_x;
			const panel_grid_y = grid_y;
			if (is_selected) {
				this.current_grid_x = panel_grid_x;
				this.current_grid_y = panel_grid_y;
				this.initial_grid_x = panel_grid_x;
				this.initial_grid_y = panel_grid_y;
			}

			panel.on('mousedown', (e) => {
				const is_synthetic = !#is_defined( e.button );
				if ((e.button == 'left' || is_synthetic) && id != this.pending_production) {
					if (this.pending_item != null) {
						this.pending_item.class = 'base-screen-production-item';
					}
					this.pending_item = panel;
					this.pending_item.class = 'base-screen-production-item-active';
					this.pending_production = id;
					this.current_grid_x = panel_grid_x;
					this.current_grid_y = panel_grid_y;
				} else if ( e.button == 'left' ) {
					// treat as double click
					this.submit();
				}
				return true;
			});

			this.grid :+ panel;

			x += this.ITEM_WIDTH + this.ITEM_PADDING;
			grid_x++;
			if (grid_x >= this.ITEMS_IN_ROW) {
				x = this.ITEM_PADDING;
				y += this.ITEM_HEIGHT + this.ITEM_PADDING;
				grid_x = 0;
				grid_y++;
			}
		}
	},

};
