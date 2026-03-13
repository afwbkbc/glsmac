return {

	panels: [],

	refresh: (width) => {

		const optimal_panel_size = 210;
		const panel_padding = 3;

		let panels_count = ( width + panel_padding ) / optimal_panel_size;
		if ( panels_count < 1 ) {
			panels_count = 1; // prevent division by zero
		}
		const panel_width = ( width + panel_padding ) / panels_count - panel_padding;

		let current_panels_count = #sizeof(this.panels);

		let left = 0;
		for (let i = 0; i < panels_count; i++) {
			if (i < current_panels_count) {
				// resize existing panel
				this.panels[i].width = panel_width;
				this.panels[i].left = left;
			}
			else {
				// add new panel
				this.panels :+ this.add_panel(left, panel_width);
			}
			left += panel_width + panel_padding;
		}
		while ( current_panels_count > panels_count ) {
			// remove excessive panels
			current_panels_count--;
			this.panels[ current_panels_count ].remove();
			this.panels :~;
		}

	},

	add_panel: (left, width) => {

		const panel = this.page.panel({
			class: 'bottombar-info-panel',
			width: width,
			left: left,
		});

		// TODO: texts etc

		return panel;

	},

	on_show: () => {
		this.refresh(this.page.width); // TODO: make resize event trigger while hidden
	},

	init: (p) => {

		p.ui.class('bottombar-info-panel').extend('bottombar-frame-inner').set({
			top: 0,
			bottom: 0,
			//height: 97,
		});

		this.page = p.frame.panel({
			class: 'bottombar-frame-page',
			overflow: 'hidden',
		});

		this.button = p.frame.button({
			class: 'bottombar-frame-page-button',
			align: 'bottom right',
			bottom: 5,
			right: 5,
			text: 'I',
			group: 'pages',
		});

		this.page.on('resize', (e) => {
			this.refresh(e.width);
		});
		this.refresh(this.page.width);

	},

};
