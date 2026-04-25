return {

	init: (p) => {

		this.buttons = p.body.panel({
			align: 'top',
			top: 3,
			left: 3,
			right: 3,
			height: 26,
			border: 'rgb(232,136,8),2',
		});

		p.ui.class('base-screen-top-button').set({
			width: 112,
			top: 3,
			bottom: 3,
			font: 'arialnb.ttf:16',
			color: 'rgb(228,104,24)',
			sound: 'ok.wav',
			_hover: {
				color: 'black',
			},
			_active: {
				color: 'black',
			},
		});

		p.ui.class('base-screen-top-button-1').extend('base-screen-top-button').set({
			border: 'rgb(136,69,28),2',
			background: 'interface.pcx:crop(68,65,134,82)',
			_hover: {
				background: 'interface.pcx:crop(68,86,134,103)',
			},
			_active: {
				background: 'interface.pcx:crop(68,86,134,103)',
				border: 'rgb(220,172,52),2',
			},
		});

		p.ui.class('base-screen-top-button-2').extend('base-screen-top-button').set({
			border: 'rgb(176,20,20),2',
			background: 'interface.pcx:crop(68,2,134,19)',
			_hover: {
				background: 'interface.pcx:crop(68,23,134,40)',
				border: 'rgb(136,12,12),2',
			},
			_active: {
				background: 'interface.pcx:crop(68,23,134,40)',
				border: 'rgb(228,120,24),2',
			},
		});

		p.ui.class('base-screen-top-button-dropdown').extend('base-screen-top-button-2').set({
			width: 20,
			background: 'interface.pcx:crop(248,2,265,19)',
			_hover: {
				background: 'interface.pcx:crop(248,23,265,40)',
			},
			_active: {
				background: 'interface.pcx:crop(248,44,265,61)',
			},
		});

		this.buttons.button({
			class: 'base-screen-top-button-1',
			align: 'left',
			left: 3,
			text: 'EXPLORE',
		});
		this.buttons.button({
			class: 'base-screen-top-button-1',
			align: 'left',
			left: 116,
			text: 'DISCOVER',
		});

		this.buttons.button({
			class: 'base-screen-top-button-dropdown',
			align: 'left',
			left: 230,
		});
		this.buttons.button({
			class: 'base-screen-top-button-2',
			align: 'center',
			width: 170,
			text: 'GOVERNOR',
		});
		this.buttons.button({
			class: 'base-screen-top-button-dropdown',
			align: 'right',
			right: 230,
		});

		this.buttons.button({
			class: 'base-screen-top-button-1',
			align: 'right',
			right: 116,
			text: 'BUILD',
		});
		this.buttons.button({
			class: 'base-screen-top-button-1',
			align: 'right',
			right: 3,
			text: 'CONQUER',
		});

	},

};
