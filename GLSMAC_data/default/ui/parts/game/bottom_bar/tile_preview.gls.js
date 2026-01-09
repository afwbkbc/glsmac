return {

	init: (p) => {

		this.frame = p.el.panel({
			class: 'bottombar-frame',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 137,
			width: 106,
		});

		this.preview = this.frame.widget({
			type: 'tile-preview',
			align: 'top center',
			top: 2,
			width: 84,
			height: 52,
		});

	},

};
