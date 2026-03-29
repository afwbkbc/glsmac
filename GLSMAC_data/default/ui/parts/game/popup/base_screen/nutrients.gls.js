return {

	init: (p) => {

		this.p = p;

		this.frame = p.body.panel({
			class: 'base-screen-side-frame',
			align: 'top left',
			top: 32,
			left: 3,
			height: 154,
		});

		const header = this.frame.panel({
			class: 'base-screen-header',
		});
		header.text({
			class: 'base-screen-frame-title',
			text: 'NUTRIENTS',
		});

		const body = this.frame.panel({
			class: 'base-screen-body',
		});

		p.ui.class('base-screen-growth-cell');
		p.ui.class('base-screen-growth-cell-empty').extend('base-screen-growth-cell').set({
			background: 'interface.pcx:crop(397,22,416,41)',
		});
		p.ui.class('base-screen-growth-cell-pending').extend('base-screen-growth-cell-empty').set({
			border: 'rgb(212,208,116),0',
		});
		p.ui.class('base-screen-growth-cell-full').extend('base-screen-growth-cell').set({
			background: 'interface.pcx:crop(418,22,437,41)',
		});

		this.growth_cells = body.area({
			left: 10,
			top: 4,
			right: 10,
			bottom: 34,
		});

		this.growth_label = body.text({
			class: 'base-screen-frame-text',
			align: 'bottom center',
			bottom: 12,
		});
	},

	set_growth_cells: (total, filled, pending) => {
		this.growth_cells.clear();

		const total_width = 112;
		const total_height = 90;

		const rows = 2; // is it always 2?
		const columns = #floor(#to_float(total) / #to_float(rows));
		const width = #floor(#to_float(total_width) / #to_float(columns));
		const height = #floor(#to_float(total_height) / #to_float(rows));

		this.p.ui.class('base-screen-growth-cell').set({
			width: width - 2,
			height: height - 2,
		});

		const offset_left = (total_width - (columns * width)) / 2;
		let left = offset_left;
		let top = (total_height - (rows * height)) / 2;

		let i = 0;
		let cls = '';

		for (let y = 0; y < rows; y++) {
			for (let x = 0; x < columns; x++) {
				if (i < filled) {
					cls = 'full';
				} else {
					if (i < filled + pending) {
						cls = 'pending';
					} else {
						cls = 'empty';
					}
				}
				i++;
				this.growth_cells.panel({
					class: 'base-screen-growth-cell-' + cls,
					left: left + 1,
					top: top + 1,
				});
				left += width;
			}
			top += height;
			left = offset_left;
		}

		let growth_in = (total - filled) / pending;
		let growth_text = 'Growth: ' + #to_string(growth_in);
		if (growth_in == 1) {
			growth_text += ' turn';
		} else {
			growth_text += ' turns';
		}
		this.growth_label.text = growth_text;
	},

};
