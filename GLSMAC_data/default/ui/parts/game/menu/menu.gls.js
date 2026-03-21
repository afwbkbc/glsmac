return {

	menu_width: 139,
	item_height: 18,

	available_menus: [
		'left_menu',
		'left_menu_game',
		'right_menu',
	],

	menu_defs: {},
	menus: {},

	on_close_handlers: [],

	init: (p) => {

		const that = this;

		p.ui.class('game-menu').set({
			width: that.menu_width,
			background: 'interface.pcx:crop(439, 64, 458, 83)',
		});
		p.ui.class('game-menu-top-border').set({
			align: 'top',
			left: 0,
			right: 0,
			top: -4,
			height: 5,
			background: 'console2_a.pcx:crop(0, 0, 138, 4)',
		});
		p.ui.class('game-menu-bottom-border').set({
			align: 'bottom',
			left: 0,
			right: 0,
			bottom: -4,
			height: 5,
			background: 'console2_a.pcx:crop(0, 0, 138, 4):flipv:fliph',
		});
		p.ui.class('game-menu-item').set({
			font: 'arialnb.ttf:17',
			color: 'rgb(176, 212, 235)',
			height: 17,
			_hover: {
				background: 'console_x2_a.pcx:crop(1, 219, 106, 228)',
				color: 'rgb(228, 104, 24)',
			},
			_active: {
				background: 'console_x2_a.pcx:crop(1, 249, 106, 258)',
				color: 'black',
			},
		});

		const pp = {
			menu: that,
			menu_width: that.menu_width,
			item_height: that.item_height,
			maybe_quit: p.maybe_quit,
			create: (entries) => {

				let top = 0;

				const menu = p.ui.root.panel({
					class: 'game-menu',
					height: #sizeof(entries) * that.item_height,
				});
				menu.hide();
				menu.surface({
					class: 'game-menu-top-border',
				});
				menu.surface({
					class: 'game-menu-bottom-border',
				});

				let active_entry = null;
				let active_item = null;

				for (e of entries) {
					const entry = e; // TODO: fix
					const item = menu.button({
						class: 'game-menu-item',
						text: entry.label,
						top: top,
					});
					item.on('click', (e) => {
						if (active_entry != null) {
							if (#is_defined(active_entry.close)) {
								active_entry.close();
							}
							active_item.active = false;
						}
						active_entry = entry;
						active_item = item;
						active_item.active = true;
						if (#is_defined(active_entry.open)) {
							active_entry.open();
						}
						return true;
					});

					top += that.item_height;
				}

				return {
					el: menu,
					is_open: false,
					on_close: () => {
						if (active_entry != null) {
							if (#is_defined(active_entry.close)) {
								active_entry.close();
							}
							active_item.active = false;
							active_entry = null;
							active_item = null;
						}
					},
				};
			},
		};

		for (m of this.available_menus) {
			this.menu_defs[m] = #include(m);
			this.menus[m] = this.menu_defs[m].init(pp);
			const m = this.menus[m];
		}

	},

	open: (id, style) => {
		const m = this.menus[id];
		if (!#is_defined(m)) {
			#print('WARNING: menu not found: ' + id);
			return;
		}
		if (!m.is_open) {
			m.is_open = true;
			if (#is_defined(m.on_open)) {
				m.on_open();
			}
			if (#is_defined(style)) {
				for (k in style) {
					m.el[k] = style[k];
				}
			}
			m.el.show();
		}
	},

	close: (id) => {
		const m = this.menus[id];
		if (!#is_defined(m)) {
			#print('WARNING: menu not found: ' + id);
			return;
		}
		if (m.is_open) {
			m.el.hide();
			if (#is_defined(m.on_close)) {
				m.on_close();
			}
			m.is_open = false;
			for ( cb of this.on_close_handlers ) {
				cb( id );
			}
		}
	},

	close_all: () => {
		for (id in this.menus) {
			const m = this.menus[id];
			if (m.is_open) {
				this.close(id);
			}
		}
	},

	on_close: (cb) => {
		this.on_close_handlers :+ cb;
	},

};
