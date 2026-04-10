return {

	available_pages: ['resource', 'support', 'psych'],

	init: (p) => {

		const button_width = 128;
		const button_padding = 2;

		p.ui.class('base-screen-middle-area').set({
			left: 2,
			top: 2,
			right: 2,
			bottom: 2,
		});

		p.ui.class('base-screen-middle-button').extend('popup-button').set({
			width: button_width,
			align: 'bottom left',
			bottom: button_padding,
			height: 20,
			group: 'base-screen-middle-area-buttons',
		});

		this.area = p.body.panel({
			class: 'base-screen-frame',
			align: 'top center',
			top: 32,
			width: 392,
			height: 223,
		});
		const inner = this.area.area({
			bottom: 22,
		});

		const pp = {
			area: inner,
		};

		let left = button_padding;
		for (p of this.available_pages) {
			const page = #include('middle_area/' + p);
			page.init(pp);
			if (left > button_padding) {
				page.el.hide();
			}
			const btn = this.area.button({
				class: 'base-screen-middle-button',
				left: left,
				text: #uppercase(p),
			});
			btn.on('on', (e) => {
				page.el.show();
				return true;
			});
			btn.on('off', (e) => {
				page.el.hide();
				return true;
			});
			left += button_width + button_padding;
		}

	},

};
