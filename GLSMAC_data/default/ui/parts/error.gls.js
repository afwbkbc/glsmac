return (m) => {

	let ui = m.glsmac.ui;

	ui.on('error', (e) => {

		const popup = m.root.window({
			class: 'window',
			title: 'ERROR',
			width: 440,
			height: 120,
			blocker: 'rgba(0, 0, 0, 120)',
		});

		popup.text({
			class: 'popup-text',
			align: 'top center',
			top: 30,
			text: e.error,
		});

		const footer = popup.panel({
			class: 'popup-footer',
		});
		const btn = footer.button({
			class: 'popup-button',
			text: 'OK',
			align: 'center',
		});
		const f_on_close = () => {
			popup.remove();
			return e.on_close();
		};
		btn.on('click', f_on_close);
		popup.on('keydown', (e) => {
			if (#is_empty(e.modifiers) && (e.code == 'ESCAPE') || (e.code == 'ENTER')) {
				return f_on_close();
			} else {
				return true;
			}
		});

	});

};
