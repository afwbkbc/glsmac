return {

	init: (p) => {

		this.frame = p.body.panel({
			class: 'base-screen-frame',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 6,
			width: 129,
		});

		this.frame.panel({
			class: 'default-panel-inner',
		});

	},

};
