return {

	init: (p) => {

		this.frame = p.body.panel({
			class: 'base-screen-side-frame',
			align: 'top left',
			top: 188,
			left: 3,
			height: 194,
		});

		const header = this.frame.panel({
			class: 'base-screen-header',
		});
		header.text({
			class: 'base-screen-frame-title',
			text: 'ENERGY GRID',
		});

		this.frame.panel({
			class: 'base-screen-body',
		});

	},

};
