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

		body.text({
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

		this.production_label = body.text({
			class: 'base-screen-frame-text',
			align: 'bottom center',
			bottom: 20,
		});

	},

	set: (data) => {

		this.p.utils.set_cells(
			108,
			60,
			data.columns,
			data.rows,
			data.filled,
			data.pending,
			this.production_cells,
			'base-screen-bottombar-production-cell',
			this.production_label,
			(progress_in) => {
				return #to_string(progress_in) + ' TURNS';
			},
		);

	},

};
