return {

	available_parts: [
		'production',
		'build_queue',
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
			body: that.frame,
		};

		for (s of this.available_parts) {
			this.parts[s] = #include('bottom_bar/' + s);
			this.parts[s].init(pp);
		}

	},

};
