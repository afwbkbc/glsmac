return {

	selected_unit: null,
	active_item: null,

	set_active_item: (object) => {
		if (this.active_item != null) {
			this.active_item.class = 'bottombar-objects-list-item';
			this.active_item.border = #undefined; // TODO: fix class switching logic to also apply modifier styles correctly
		}
		this.active_item = object;
		if (this.active_item != null) {
			this.active_item.class = 'bottombar-objects-list-item-active';
		}
	},

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
		switch (cls) {
			case 'Unit': {
				const p = this.p; // TODO: make this work within object
				item.text({
					class: 'bottombar-objects-list-item-text',
					text: p.get_stats_str(object),
				});
				if (object == this.selected_unit) {
					this.set_active_item(item);
				}
				item.on('mousedown', (e) => {
					this.p.game.select_unit(object);
					return true;
				});
				item.on('mouseover', (e) => {
					this.p.sections.object_preview.show(object);
					return true;
				});
				item.on('mouseout', (e) => {
					this.p.sections.object_preview.show(this.selected_unit);
					return true;
				});
				break;
			}
			case 'Base': {
				// TODO:
				break;
			}
		}
	},

	update_tile: (tile) => {

		if (#is_defined(this.list)) {
			this.list.remove();
			this.set_active_item(null);
		}
		this.list = this.frame.scrollview({
			left: 2,
			right: 2,
			top: -8,
			bottom: -8,
			padding: 10,
			has_hscroll: false,
			has_vscroll: false,
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
			height: 52,
			_hover: {
				border: 'rgb(14,40,49),1',
			},
		});
		p.ui.class('bottombar-objects-list-item-active').extend('bottombar-objects-list-item').set({
			border: 'rgb(28,80,99),1',
			_hover: {
				border: 'rgb(28,80,99),1',
			},
		});

		p.ui.class('bottombar-objects-list-item-preview').set({
			width: width,
			height: 43,
		});

		p.ui.class('bottombar-objects-list-item-text').set({
			font: 'arialnb.ttf:12',
			color: 'rgb(235,235,235)',
			align: 'bottom center',
			bottom: 3,
		});

		this.frame = p.el.panel({
			class: 'bottombar-frame',
			align: 'bottom',
			bottom: 7,
			left: 245,
			right: 258,
			height: 58,
		});

		p.map.on('unit_preview', (e) => {
			if (e.unit != this.selected_unit) {
				this.selected_unit = e.unit;
				if (this.selected_unit != null) {
					this.update_tile(this.selected_unit.get_tile());
				}
			}
		});

		p.map.on('tile_preview', (e) => {
			this.update_tile(e.tile);
		});

	},

};
