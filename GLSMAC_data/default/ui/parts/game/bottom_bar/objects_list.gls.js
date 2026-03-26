return {

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
					this.p.modules.popup.hide('base_screen');
					this.set_active_item(null);
					this.p.game.select_unit(object);
					return true;
				});
				break;
			}
			case 'Base': {
				this.base_items[key] = item;
				item.on('mousedown', (e) => {
					this.set_active_item(null);
					this.p.game.select_base(object);
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
		if (object == this.selected_object) {
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

		this.selected_tile = null;
		this.selected_object = null;
		this.unit_items = {};
		this.base_items = {};
		this.active_item = null;

		this.p = p;

		this.object_width = 57;

		const width = this.object_width; // TODO: fix so that this works inside properties

		p.ui.class('bottombar-objects-list-item').set({
			width: width,
			height: 46,
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
			height: 40,
		});

		p.ui.class('bottombar-objects-list-item-text').set({
			font: 'arialnb.ttf:12',
			color: 'rgb(235,235,235)',
			align: 'bottom center',
			bottom: 4,
		});

		const frame_outer = p.el.panel({
			class: 'bottombar-panel',
			align: 'bottom',
			bottom: 7,
			left: 245,
			right: 258,
			height: 58,
		});
		this.frame = frame_outer.panel({
			class: 'bottombar-panel-inner',
		});

		p.map.on('unit_preview', (e) => {
			if (e.unit != null) {
				const tile = e.unit.get_tile();
				if (tile != this.selected_tile) {
					this.update_tile(tile);
				}
				const key = #to_string(e.unit.id);
				if (!#is_defined(this.unit_items[key])) {
					this.add_object(e.unit, this.list_width);
					this.list_width = this.list_width + this.object_width;
				}
			}
		});

		p.map.on('base_preview', (e) => {
			const tile = e.base.get_tile();
			if (tile != this.selected_tile) {
				this.update_tile(tile);
			}
		});

		p.map.on('tile_preview', (e) => {
			if (e.tile != this.selected_tile) {
				this.update_tile(e.tile);
			}
		});

		p.game.on('unit_select', (e) => {
			this.p.modules.popup.hide('base_screen');
			const key = #to_string(e.unit.id);
			this.set_active_item(this.unit_items[key]);
			this.selected_object = e.unit;
		});

		p.game.on('base_select', (e) => {
			const key = #to_string(e.base.id);
			this.set_active_item(this.base_items[key]);
			const last_selected_object = this.selected_object;
			this.selected_object = e.base;
			this.p.modules.popup.set('base_screen', {
				base: e.base,
			});
			this.p.modules.popup.show('base_screen', (result) => {
				this.selected_object = null;
				if (last_selected_object != null) {
					switch (#classof(last_selected_object)) {
						case 'Unit': {
							this.p.game.select_unit(last_selected_object);
							break;
						}
					}
				} else {
					this.set_active_item(null);
					this.p.game.select_tile(this.selected_tile);
				}
			});
		});

		p.game.on('tile_select', (e) => {
			if (this.selected_object != null) {
				this.selected_object = null;
				this.set_active_item(null);
			}
		});

		p.game.on('turn', (e) => {
			if (this.selected_object != null) {
				this.selected_object = null;
				this.set_active_item(null);
			}
		});

		// TODO: make event dom object
		frame_outer.on('remove', (e) => {
			if (#is_defined(p.map)) {
				p.map.off('unit_preview');
				p.map.off('base_preview');
				p.map.off('tile_preview');
				p.game.off('unit_select');
				p.game.off('base_select');
				p.game.off('tile_select');
				p.game.off('turn');
			}
		});
	},

};
