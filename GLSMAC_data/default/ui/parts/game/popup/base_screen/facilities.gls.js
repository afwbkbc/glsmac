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

		this.list = this.frame.listview({
			class: 'base-screen-body',
			itemsize: 17,
			padding: 4,
		});

	},

	set: (data) => {
		this.list.clear();
		for (f of data) {
			this.list.text({
				class: 'base-screen-frame-text',
				text: f,
			});
		}
	},

};
