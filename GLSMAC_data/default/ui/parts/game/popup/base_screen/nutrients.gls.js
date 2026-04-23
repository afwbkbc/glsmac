return {

	init: (p) => {

		this.p = p;

		this.frame = p.body.panel({
			class: 'base-screen-side-frame',
			align: 'top left',
			top: 32,
			left: 3,
			height: 154,
		});

		const header = this.frame.panel({
			class: 'base-screen-header',
		});
		header.text({
			class: 'base-screen-frame-title',
			text: 'NUTRIENTS',
		});

		const body = this.frame.panel({
			class: 'base-screen-body',
		});

		p.ui.class('base-screen-nutrients-cell');
		p.ui.class('base-screen-nutrients-cell-empty').extend('base-screen-nutrients-cell').set({
			background: 'interface.pcx:crop(397,22,416,41)',
		});
		p.ui.class('base-screen-nutrients-cell-pending').extend('base-screen-nutrients-cell-empty').set({
			border: 'rgb(212,208,116),0',
		});
		p.ui.class('base-screen-nutrients-cell-full').extend('base-screen-nutrients-cell').set({
			background: 'interface.pcx:crop(418,22,437,41)',
		});

		this.growth_cells = body.area({
			left: 6,
			top: 10,
			right: 6,
			bottom: 34,
		});

		this.growth_label = body.text({
			class: 'base-screen-frame-text',
			align: 'bottom center',
			bottom: 12,
		});
	},

	set: (data) => {
		this.p.utils.set_cells(
			112,
			90,
			data.columns,
			data.rows,
			data.filled,
			data.pending,
			this.growth_cells,
			'base-screen-nutrients-cell',
			this.growth_label,
			(progress_in) => {
				let progress_text = '';
				if (data.pending > 0) {
					progress_text = 'Growth: ' + #to_string(progress_in);
					if (progress_in == 1) {
						progress_text += ' turn';
					} else {
						progress_text += ' turns';
					}
				} else {
					if (data.pending < 0) {
						progress_text = 'Hunger!';
					} else {
						progress_text = 'Growth: -stagnant-';
					}
				}
				return progress_text;
			},
		);
	},

};
