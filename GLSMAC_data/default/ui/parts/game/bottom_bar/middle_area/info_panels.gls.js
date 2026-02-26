return {

	add_panel: () => {

		// TODO: fix `this` in object properties
		const w = this.panel_width;
		const l = this.panel_left;

		this.page.panel({
			class: 'bottombar-info-panel',
			width: w,
			left: l,
		});

		// this.panel_left += this.panel_width; // TODO: fix this
		this.panel_left = this.panel_left + this.panel_width + 4;
	},

	init: (p) => {

		this.panel_left = 1;
		this.panel_width = 230;

		const w = this.panel_width; // TODO: fix `this` in object properties
		p.ui.class('bottombar-info-panel').set({
			border: 'rgb(49, 78, 44),2',
			width: w,
			top: 0,
			height: 95,
		});

		this.page = p.frame.scrollview({
			class: 'bottombar-frame-page-noborder',
			has_hscroll: true,
		});

		this.button = p.frame.button({
			class: 'bottombar-frame-page-button',
			align: 'bottom right',
			bottom: 5,
			right: 5,
			text: 'I',
			group: 'pages',
		});

		this.add_panel();
		this.add_panel();

	},

};
