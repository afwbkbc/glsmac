return {

	init: (p) => {

		this.frame = p.el.panel({
			class: 'bottombar-frame',
			align: 'bottom right',
			top: 59,
			bottom: 6,
			right: 7,
			width: 236,
		});

		this.map_area = this.frame.surface({
			align: 'top left',
			top: 36,
			height: 112,
			left: 6,
			width: 224,
		});

		p.ui.set_render_surface('minimap', this.map_area);

	},

};
