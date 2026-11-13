return {

	cell_columns: 11,

	init: (p) => {

		this.p = p;

		this.frame = p.body.panel({
			class: 'base-screen-frame',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 6,
			width: 129,
		});

		const body = this.frame.panel({
			class: 'default-panel-inner',
		});

		this.production_preview = body.surface({
			align: 'top center',
			top: 20,
			width: 40,
			height: 40,
		});

		this.production_title = body.text({
			class: 'base-screen-frame-title',
			text: 'NOTHING',
			align: 'top center',
			top: 64,
		});

		p.ui.class('base-screen-bottombar-production-cell');
		p.ui.class('base-screen-bottombar-production-cell-empty').extend('base-screen-bottombar-production-cell').set({
			background: 'interface.pcx:crop(397,43,416,62)',
		});
		p.ui.class('base-screen-bottombar-production-cell-pending').extend('base-screen-bottombar-production-cell-empty').set({
			border: 'rgb(231,235,231),0',
		});
		p.ui.class('base-screen-bottombar-production-cell-full').extend('base-screen-bottombar-production-cell').set({
			background: 'interface.pcx:crop(418,43,437,62)',
		});

		this.production_cells = body.area({
			align: 'top center',
			left: 6,
			right: 6,
			top: 85,
		});

		this.production_eta = body.text({
			class: 'base-screen-frame-text',
			align: 'bottom center',
			bottom: 20,
		});

		body.on('mousedown', (e) => {
			this.p.utils.show_production_selection();
			return true;
		});
	},

	set: (data) => {

		const production = data.base.get_production();

		const c = this.p.INTENT_COLORS[production.intent];

		this.production_preview.background = production.texture + ':tint(' + c + ',1.0):stretch';
		this.production_title.text = production.name;
		this.production_title.color = c;

		if (#is_defined(production.cost)) {
			this.production_cells.show();
			this.production_eta.show();
			this.p.utils.set_cells(
				108,
				60,
				8,
				production.cost / 8,
				data.base.get('accumulated_minerals'),
				data.minerals_per_turn,
				this.production_cells,
				'base-screen-bottombar-production-cell',
				this.production_eta,
				(progress_in) => {
					if (data.minerals_per_turn > 0) {
						return progress_in > 0
							? #to_string(progress_in) + ' TURNS'
							: 'COMPLETE'
							;
					} else {
						return 'No Minerals';
					}
				},
			);
		} else {
			this.production_cells.hide();
			this.production_eta.hide();
		}

	},

};
