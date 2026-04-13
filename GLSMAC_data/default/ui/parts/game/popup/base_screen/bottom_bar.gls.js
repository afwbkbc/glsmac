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

	},

};
