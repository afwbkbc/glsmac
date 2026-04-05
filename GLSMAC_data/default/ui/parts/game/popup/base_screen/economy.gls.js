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
		this.header = header.text({
			class: 'base-screen-frame-title',
		});

		const body = this.frame.panel({
			class: 'base-screen-body',
		});

		body.text({
			class: 'base-screen-frame-title',
			text: 'NONE',
		});

	},

	set_commerce: (data) => {
		this.header.text = 'COMMERCE';
	},

	set_energy_grid: (data) => {
		this.header.text = 'ENERGY GRID';
	},

};
