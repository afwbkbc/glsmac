return {

	available_parts: [
		'production',
		'queue',
		'middle_area',
		'support',
	],

	init: (p) => {

		this.parts = {};

		this.frame = p.ui.root.area({
			zindex: 0.85,
			align: 'bottom',
			height: p.modules.bottom_bar.height,
			left: 0,
			right: 0,
		});
		this.frame.hide();

		// hide menus
		p.ui.class('base-screen-bottombar-menu-hidebutton').set({
			width: 106,
			height: 14,
			background: 'black',
		});
		this.frame.surface({
			class: 'base-screen-bottombar-menu-hidebutton',
			align: 'top left',
			left: 11,
			top: 20,
		}).on('mousedown', (e) => {
			return true;
		});
		this.frame.surface({
			class: 'base-screen-bottombar-menu-hidebutton',
			align: 'top right',
			right: 11,
			top: 22,
		}).on('mousedown', (e) => {
			return true;
		});

		const that = this;

		const pp = {
			ui: p.ui,
			body: that.frame,
			utils: p.utils,
		};

		for (s of this.available_parts) {
			this.parts[s] = #include('bottom_bar/' + s);
			this.parts[s].init(pp);
		}

	},

	set: (data) => {

		this.parts.production.set({
			rows: 3,
			columns: 10,
			filled: 8,
			pending: 4,
		});

		this.parts.queue.set([
			'Mind Worms',
			'Recreation Commons',
		]);

		this.parts.middle_area.set({
			basename: data.base.name,
		});

	},

};
