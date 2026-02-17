return {

	selected_tile: null,

	selected_object: null,

	unit_items: {},
	base_items: {},
	active_item: null,

	set_active_item: (item) => {
		if (this.active_item != null) {
			this.active_item.class = 'bottombar-objects-list-item';
			this.active_item.border = #undefined; // TODO: fix class switching logic to also apply modifier styles correctly
		}
		this.active_item = item;
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
		const key = #to_string(object.id);
		switch (cls) {
			case 'Unit': {
				this.unit_items[key] = item;
				const p = this.p; // TODO: make this work within object
				item.text({
					class: 'bottombar-objects-list-item-text',
					text: p.get_stats_str(object),
				});
				item.on('mousedown', (e) => {
					this.p.game.select_unit(object);
					return true;
				});
				break;
			}
			case 'Base': {
				this.base_items[key] = item;
				item.on('mousedown', (e) => {
					// TODO
					#print('TODO: select_base', object);
					return true;
				});
				break;
			}
		}
		item.on('mouseover', (e) => {
			this.p.sections.object_preview.show(object);
			return true;
		});
		item.on('mouseout', (e) => {
			this.p.sections.object_preview.show(this.selected_object);
			return true;
		});
		if (cls == 'Unit' && object == this.selected_object) {
			this.set_active_item(item);
		}
	},

	update_tile: (tile) => {

		if (#is_defined(this.list)) {
			this.list.remove();
			this.set_active_item(null);
			this.unit_items = {};
			this.base_items = {};
		}
		this.list = this.frame.scrollview({
			left: 2,
			right: 2,
			top: 1,
			bottom: -8,
			padding: 2,
			has_hscroll: true,
			has_vscroll: false,
		});

		this.list_width = 0;

		const base = tile.get_base();

		if (base != null) {
			this.add_object(base, this.list_width);
			this.list_width = this.list_width + this.object_width;
		}

		for (unit of tile.get_units()) {
			this.add_object(unit, this.list_width);
			this.list_width = this.list_width + this.object_width;
		}

		this.selected_tile = tile;
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
			if (e.unit != this.selected_object || (this.selected_object != null && e.unit.movement != this.selected_object.movement)) { // TODO: dynamic updates of wrapped objects
				this.selected_object = e.unit;
				if (this.selected_object != null) {
					const tile = this.selected_object.get_tile();
					if (tile != this.selected_tile) {
						this.update_tile(tile);
					} else {
						{
							const key = #to_string(e.unit.id);
							if (!#is_defined(this.unit_items[key])) {
								this.add_object(e.unit, this.list_width);
								this.list_width = this.list_width + this.object_width;
							}
						}
						{
							const key = #to_string(this.selected_object.id);
							if (#is_defined(this.unit_items[key])) {
								this.set_active_item(this.unit_items[key]);
							}
						}
					}
				}
			}
		});

		p.map.on('base_preview', (e) => {
			if (e.base != this.selected_object) {
				this.selected_object = e.base;
				this.set_active_item(null);
				const tile = e.base.get_tile();
				if (tile != this.selected_tile) {
					this.update_tile(tile);
				}
			}
		});

		p.map.on('tile_preview', (e) => {
			if (e.tile != this.selected_tile) {
				this.update_tile(e.tile);
			}
		});

	},

};
