return {

	available_sections: [
		'top_buttons',
		'nutrients',
		'energy_grid',
		'game_state',
		'middle_area',
		'facilities',
		'resources',
		'energy_allocation',
		'buttons',
		'bottom_bar',
	],

	init: (p) => {

		this.sections = {};

		p.ui.class('base-screen-frame').set({
			border: 'rgb(35,59,34),2',
		});
		p.ui.class('base-screen-block').extend('default-panel').set({
			top: 2,
			left: 2,
			bottom: 2,
			right: 2,
		});

		p.ui.class('base-screen-header').extend('base-screen-block').set({
			align: 'top',
			height: 20,
		});
		p.ui.class('base-screen-body').extend('base-screen-block').set({
			align: 'top',
			top: 23,
		});

		p.ui.class('base-screen-side-frame').extend('base-screen-frame').set({
			width: 138,
		});
		p.ui.class('base-screen-side-middle-frame').extend('base-screen-frame').set({
			width: 534,
		});

		p.ui.class('base-screen-frame-title').set({
			align: 'center',
			font: 'arialnb.ttf:16',
			color: 'rgb(53,61,115)',
		});
		p.ui.class('base-screen-frame-text').set({
			font: 'arialn.ttf:14',
			color: 'rgb(68,100,160)',
		});

		return p.create('', 680, 442, (body, cb) => {

			const pp = {
				body: body,
				ui: p.ui,
				hide: p.hide,
				modules: p.modules,
			};

			for (s of this.available_sections) {
				this.sections[s] = #include('base_screen/' + s);
				this.sections[s].init(pp);
			}

		});

	},

	set: (data) => {

		#print('TODO: set base', data.base);

		// dummy data for now
		this.sections.nutrients.set_growth_cells(24, 4, 2);

	},

	on_hide: () => {
		this.sections.bottom_bar.frame.hide();
	},

	on_show: () => {
		this.sections.bottom_bar.frame.show();
	},

};
