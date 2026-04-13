return {

	init: (p) => {

		this.frame = p.body.panel({
			class: 'default-panel',
			left: 263,
			top: 69,
			right: 263,
			bottom: 86,
		});

		const header = this.frame.panel({
			class: 'default-panel',
			align: 'top',
			top: 3,
			left: 5,
			right: 5,
			height: 41,
		});

		const header_title = header.panel({
			class: 'default-panel-inner',
			top: 3,
			bottom: 3,
			left: 25,
			right: 25,
		});

		this.basename = header_title.text({
			align: 'top center', // TODO: fix vertical centering
			top: 3,
			font: 'arialnb.ttf:28',
			color: 'rgb(137,166,166)',
			transform: 'uppercase',
		});

		p.ui.class('base-screen-bottombar-arrow').set({
			width: 20,
			height: 35,
			top: 3,
			sound: 'ok.wav',
		});
		// TODO: make _hover and _active work without class
		p.ui.class('base-screen-bottombar-arrow-left').extend('base-screen-bottombar-arrow').set({
			align: 'top left',
			left: 3,
			background: 'interface.pcx:crop(290,64,309,98)',
			_hover: {
				background: 'interface.pcx:crop(290,100,309,134)',
			},
			_active: {
				background: 'interface.pcx:crop(290,136,309,170)',
			},
		});
		header.button({
			class: 'base-screen-bottombar-arrow-left',
		});
		p.ui.class('base-screen-bottombar-arrow-right').extend('base-screen-bottombar-arrow').set({
			align: 'top right',
			right: 3,
			background: 'interface.pcx:crop(311,64,330,98)',
			_hover: {
				background: 'interface.pcx:crop(311,100,330,134)',
			},
			_active: {
				background: 'interface.pcx:crop(311,136,330,170)',
			},
		});
		header.button({
			class: 'base-screen-bottombar-arrow-right',
		});

	},

	set: (data) => {
		this.basename.text = data.basename;
	},

};
