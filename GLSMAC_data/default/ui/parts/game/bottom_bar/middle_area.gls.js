return {

	pp: {},

	process_message: (text) => {
		this.pp.pages.messages.process_message(text);
	},

	init: (p) => {

		p.ui.class('bottombar-frame-page-button').set({
			width: 21,
			height: 21,
			font: 'arialnb.ttf:17',
			color: 'black',
			background: 'interface.pcx:crop(47, 587, 70, 610)',
			border: 'black,2',
			_hover: {
				border: 'rgb(120,164,212),2',
			},
			_active: {
				border: 'rgb(120,164,212),2',
				background: 'interface.pcx:crop(86, 587, 109, 610)',
			},
		});

		p.ui.class('bottombar-frame-page').set({
			border: 'rgb(49, 78, 44),2',
			left: 3,
			top: 3,
			bottom: 3,
			right: 30,
		});

		this.pp = {
			ui: p.ui,
			game: p.game,
			frame: p.el.panel({
				class: 'bottombar-frame',
				left: 263,
				top: 69,
				right: 263,
				bottom: 86,
			}),
			pages: {},
		};

		for (s of ['messages']) {
			this.pp.pages[s] = #include('middle_area/' + s);
		}
		for (s of this.pp.pages) {
			s.init(this.pp);
		}

	},

};
