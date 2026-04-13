return {

	init: (p) => {

		this.frame = p.body.panel({
			class: 'default-panel',
			align: 'bottom right',
			top: 59,
			bottom: 6,
			right: 7,
			width: 236,
		});

		const header = this.frame.panel({
			class: 'base-screen-header',
		});
		header.text({
			class: 'base-screen-frame-title',
			text: 'FORCES SUPPORTED',
		});
		const body = this.frame.panel({
			class: 'base-screen-body',
		});

	},

};
