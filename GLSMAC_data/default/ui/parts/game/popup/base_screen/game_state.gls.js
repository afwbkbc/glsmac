return {

	init: (p) => {

		this.frame = p.body.panel({
			class: 'base-screen-side-frame',
			align: 'top left',
			top: 384,
			left: 3,
			height: 55,
		});

		this.frame.panel({
			class: 'base-screen-block',
		});

	},

};
