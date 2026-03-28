return {

	init: (p) => {

		this.frame = p.body.panel({
			class: 'base-screen-side-frame',
			align: 'top right',
			top: 32,
			right: 3,
			height: 224,
		});

		const header = this.frame.panel({
			class: 'base-screen-header',
		});
		header.text({
			class: 'base-screen-frame-title',
			text: 'BASE FACILITIES',
		});

		this.frame.panel({
			class: 'base-screen-body',
		});

	},

};
