return (m) => {
	/*
		m.glsmac.ui.class('menublock-items').set({
			align: 'bottom right',
			width: 354,
			bottom: 16,
		});
		m.glsmac.ui.class('menublock-item').set({
			align: 'bottom left',
			width: 300,
			height: 46,
		});
		m.glsmac.ui.class('menublock-item-top').set({
			left: 0,
			top: -2,
			width: 354,
			height: 8,
			background: 'console_x.pcx:crop(708, 1, 715, 354):rotate():flipv():contrast(0.8)',
		});
		m.glsmac.ui.class('menublock-item-top2').set({
			left: 200,
			top: -8,
			width: 154,
			height: 8,
			background: 'console_x.pcx:crop(708, 1, 715, 354):rotate():flipv():contrast(1.5)',
		});
		m.glsmac.ui.class('menublock-item-body').set({
			font: 'arialnb.ttf:20',
			transform: 'uppercase',
			color: 'rgb(94, 162, 137)',
			background: 'rgba(26, 47, 27, 210)',
			width: 284,
			height: 36,
			top: 6,
			left: 11,
		});
		m.glsmac.ui.class('menublock-item-active').set({
			color: 'rgb(193, 240, 204)',
			background: 'rgba(34, 58, 35, 210)',
		});
		m.glsmac.ui.class('menublock-item-bottom').set({
			left: 6,
			top: 42,
			width: 295,
			height: 8,
			background: 'console_x.pcx:crop(699, 1, 706, 293):rotate():flipv():contrast(0.3)',
		});
		m.glsmac.ui.class('menublock-item-right').set({
			left: 295,
			top: 6,
			width: 5,
			height: 36,
			background: 'interface.pcx:crop(299, 756, 303, 790):contrast(0.3)',
		});
	*/
	let current_menublock = null;

	let active_entry_index = null;

	const remove_active_menublock = () => {
		if (current_menublock != null) {
			current_menublock.remove();
			current_menublock = null;
			active_entry_index = null;
		}
	};

	return {

		manager: #include('manager')('popup', this),

		back: () => {
			remove_active_menublock();
			this.manager.pop(m);
		},

		select: (index) => {
			if (index != active_entry_index) {
				if (active_entry_index != null) {
					((current_entries[active_entry_index]).obj).removeclass('menublock-item-active');
				}
				active_entry_index = index;
				((current_entries[active_entry_index]).obj).addclass('menublock-item-active');
			}
		},

		show: (data) => {

			this.manager.push('popup', data);

			remove_active_menublock();

			current_menublock = m.root.panel({
				class: 'popup',
				width: 400,
				height: 300,
			});

			if (#is_defined(data.title)) {
				const header = current_menublock.panel({
					class: 'popup-header',
				});
				header.text({
					class: 'popup-header-text',
					text: data.title,
				});
			}

			const body = current_menublock.panel({
				class: 'popup-body',
			});

			current_menublock.on('keydown', (e) => {
				#print(e.modifiers);
				if (!#is_empty(e.modifiers)) {
					return false;
				}
				if (e.code == 'ESCAPE') {
					this.back();
					return true;
				}
				return false;
			});

			return {};

		},

		hide: () => {
			remove_active_menublock();
		},

	};


};
