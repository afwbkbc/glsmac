return {

	add_object: (object, left) => {
		const cls = #classof(object);
		let type = null;
		let data = {};
		let top = 0;
		switch (cls) {
			case 'Unit': {
				type = 'unit-preview';
				data.unit = object;
				top = -2;
				break;
			}
			case 'Base': {
				type = 'base-preview';
				data.base = object;
				top = 2;
				break;
			}
			default: {
				throw Error('Unknown object type: ' + #classof(object));
			}
		}

		let item = this.list.panel({
			class: 'bottombar-objects-list-item',
			left: left,
		});
		item.widget({
			type: type,
			data: data,
			class: 'bottombar-objects-list-item-preview',
			top: top,
		});
		if (cls == 'Unit') {
			const p = this.p; // TODO: make this work within object
			item.text({
				class: 'bottombar-objects-list-item-text',
				text: p.get_stats_str(object),
			});
		}
	},

	update_units: (tile) => {
		if (#is_defined(this.list)) {
			this.list.remove();
		}
		this.list = this.frame.scrollview({
			left: 2,
			right: 2,
			top: -8,
			bottom: -6,
			padding: 10,
		});

		let left = 0;

		const base = tile.get_base();

		if (base != null) {
			this.add_object(base, left);
			left += this.object_width;
		}

		for (unit of tile.get_units()) {
			this.add_object(unit, left);
			left += this.object_width;
		}
	},

	init: (p) => {

		this.p = p;

		this.object_width = 57;

		const width = this.object_width; // TODO: fix so that this works inside properties

		p.ui.class('bottombar-objects-list-item').set({
			width: width,
			height: 48,
		});

		p.ui.class('bottombar-objects-list-item-preview').set({
			width: width,
			height: 43,
		});

		p.ui.class('bottombar-objects-list-item-text').set({
			font: 'arialnb.ttf:12',
			color: 'rgb(235,235,235)',
			align: 'bottom center',
		});

		this.frame = p.el.panel({
			class: 'bottombar-frame',
			align: 'bottom',
			bottom: 7,
			left: 245,
			right: 258,
			height: 58,
		});

		p.map.on('tile_preview', (e) => {
			this.update_units(e.tile);
		});

	},

};
