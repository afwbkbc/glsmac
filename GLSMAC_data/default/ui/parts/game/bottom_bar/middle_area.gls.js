return {

	process_message: (text) => {
		this.pp.pages.messages.process_message(text);
	},

	init: (p) => {

		this.pp = {};

		p.ui.class('bottombar-panel-page-button').extend('bottombar-panel-button').set({
			width: 21,
			height: 21,
			font: 'arialnb.ttf:17',
			color: 'black',
		});

		p.ui.class('bottombar-panel-page').set({
			left: 3,
			top: 3,
			bottom: 3,
			right: 30,
		});

		this.pp = {
			ui: p.ui,
			game: p.game,
			frame: p.el.panel({
				class: 'bottombar-panel',
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
				if (#is_defined(s.on_show)) {
					s.on_show();
				}
				return true;
			});
			s.button.on('off', (e) => {
				page.hide();
				return true;
			});
		}

	},

};
