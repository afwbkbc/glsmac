return (m) => {

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

	let current_menublock = null;
	let current_entries = [];
	let menublock_animation = null;

	let active_entry_index = null;
	const set_active_entry = (index) => {
		if (index != active_entry_index) {
			if (active_entry_index != null) {
				((current_entries[active_entry_index]).obj).removeclass('menublock-item-active');
			}
			active_entry_index = index;
			((current_entries[active_entry_index]).obj).addclass('menublock-item-active');
		}
	};
	const get_active_cb = () => {
		if (active_entry_index != null) {
			return (current_entries[active_entry_index]).cb;
		} else {
			return null;
		}
	};

	const slideout_and_cb = (cb) => {
		if (menublock_animation == null) {
			menublock_animation = #async(10, () => {
				current_menublock.right = current_menublock.right - 40;
				if (current_menublock.right > -354) {
					return true;
				} else {
					menublock_animation = null;
					remove_active_menublock();
					cb();
					return false;
				}
			});
		}
	};

	const remove_active_menublock = () => {
		if (current_menublock != null) {
			if (menublock_animation != null) {
				menublock_animation.stop();
				menublock_animation = null;
			}
			current_menublock.remove();
			current_menublock = null;
			active_entry_index = null;
		}
	};

	return {

		show: (entries, on_exit) => {

			remove_active_menublock();

			current_menublock = m.root.group({
				class: 'menublock-items',
			});
			current_menublock.on('keydown', (e) => {
				// TODO: switch
				if (e.code == 'ESCAPE') {
					slideout_and_cb(on_exit);
					return true;
				}
				// TODO: else if
				if (e.code == 'DOWN') {
					if (active_entry_index < #sizeof(current_entries) - 1) {
						set_active_entry(active_entry_index + 1);
					}
					return true;
				}
				if (e.code == 'UP') {
					if (active_entry_index > 0) {
						set_active_entry(active_entry_index - 1);
					}
					return true;
				}
				if (e.code == 'ENTER') {
					slideout_and_cb(get_active_cb());
					return true;
				}
				return false;
			});
			let bottom = (#sizeof(entries) - 1) * 70;
			current_entries = [];
			let index = 0;
			for (entry of entries) {
				const itemobj = current_menublock.panel({
					class: 'menublock-item',
					bottom: bottom,
				});
				itemobj.surface({class: 'menublock-item-top'});
				itemobj.surface({class: 'menublock-item-top2'});
				itemobj.surface({class: 'menublock-item-bottom'});
				itemobj.surface({class: 'menublock-item-right'});
				const itembody = itemobj.button({
					class: 'menublock-item-body',
					text: entry[0],
				});
				const cb = entry[1];
				const current_entry = {
					obj: itembody,
					cb: cb,
					index: index,
				};
				itembody.on('mousedown', (e) => {
					if (menublock_animation == null) {
						set_active_entry(current_entry.index);
						const cb = get_active_cb();
						slideout_and_cb(cb);
						return true;
					}
					return false;
				});
				itembody.on('mousemove', (e) => {
					if (menublock_animation == null) {
						set_active_entry(current_entry.index);
						return true;
					}
					return false;
				});
				bottom -= 70;
				current_entries:+current_entry;
				index++;
			}
			if (#sizeof(entries) > 0) {
				set_active_entry(0);
			}

			current_menublock.right = -354;
			menublock_animation = #async(10, () => {
				current_menublock.right = current_menublock.right + 40;

				if (current_menublock.right < 0) {
					return true;
				} else {
					current_menublock.right = 0;
					menublock_animation = null;
					return false;
				}
			});

			return {};

		},

	};
};
