return (m) => {
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
				width: data.width,
				height: data.height,
			});

			let body_class = 'popup-body';

			if (#is_defined(data.title)) {
				const header = current_menublock.panel({
					class: 'popup-header',
				});
				header.text({
					class: 'popup-header-text',
					text: data.title,
				});
			} else {
				body_class += ' popup-body-noheader';
			}

			if (!#is_defined(data.buttons)) {
				body_class += ' popup-body-nofooter';
			}

			const body = current_menublock.panel({
				class: body_class,
			});

			if (#is_defined(data.generator)) {
				data.generator(body);
			}

			if (#is_defined(data.buttons)) {
				const footer = current_menublock.panel({
					class: 'popup-footer',
				});
				for (button of data.buttons) {
					const btn = footer.button(button.style + {class: 'popup-button'});
					btn.on('click', button.onclick);
				}
			}

			current_menublock.on('keydown', (e) => {
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

		error: (text) => {
			const that = this;
			#print('ERROR BEGIN');
			this.show({
				title: 'ERROR',
				width: 400,
				height: 120,
				generator: (body) => {
					#print('ERROR GENERATOR BEGIN');
					body.text({
						class: 'popup-text',
						align: 'center',
						text: text,
					});
					#print('ERROR GENERATOR END');
				},
				buttons: [
					{
						style: {
							text: 'OK',
							align: 'center',
							is_ok: true,
							is_cancel: true,
						},
						onclick: (e) => {
							that.back();
							return true;
						},
					},
				],
			});
			#print('ERROR END');
		},

		hide: () => {
			remove_active_menublock();
		},

	};


};
