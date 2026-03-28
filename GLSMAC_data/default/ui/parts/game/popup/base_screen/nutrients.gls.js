return {

	init: (p) => {

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

		this.frame.panel({
			class: 'base-screen-body',
		});

	},

};
