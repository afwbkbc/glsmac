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

		this.map_area = this.frame.widget({
			type: 'minimap',
			align: 'top left',
			top: 36,
			height: 112,
			left: 6,
			width: 224,
		});

	},

};
