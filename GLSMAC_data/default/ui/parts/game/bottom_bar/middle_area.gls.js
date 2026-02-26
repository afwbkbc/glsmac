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

		p.ui.class('bottombar-frame-page-noborder').set({
			left: 3,
			top: 3,
			bottom: 3,
			right: 30,
		});
		p.ui.class('bottombar-frame-page').extend('bottombar-frame-page-noborder').set({
			border: 'rgb(49, 78, 44),2',
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

		this.pp.pages_order = ['messages', 'info_panels'];
		for (s of this.pp.pages_order) {
			this.pp.pages[s] = #include('middle_area/' + s);
		}
		let first = true;
		for (p of this.pp.pages_order) {
			const s = this.pp.pages[p];
			s.init(this.pp);
			const page = s.page;
			if (first) {
				first = false;
			} else {
				page.hide();
			}
			s.button.on('on', (e) => {
				page.show();
				return true;
			});
			s.button.on('off', (e) => {
				page.hide();
				return true;
			});
		}

	},

};
