let popups = [];

return {

	popup: (ui) => {
		const popup = ui.root.panel({
			class: 'system-popup',
			align: 'center',
			width: 500,
			height: 120,
		});

		const header = popup.panel({
			class: 'system-popup-header',
		});
		header.text({
			class: 'system-popup-header-text',
			text: 'GLSMAC',
		});

		const body = popup.panel({
			class: 'system-popup-body',
		});

		popups :+ popup;

		return [popup, body];
	},

	error: (ui, p) => {
		const popup = this.popup(ui);
		const body = popup[1];

		let top = 0;
		for (text of p.texts) {
			top += 16;
			body.text({
				class: 'system-popup-text',
				text: text,
				top: top,
				left: 10,
			});
		}

		body.button({
			class: 'system-popup-button-ok',
			is_ok: true,
			is_cancel: true,
			text: 'OK',
			align: 'top center',
			top: top + 20,
		})
			.on('click', (e) => {
				if (#is_defined(p.on_close)) {
					p.on_close();
				}
				return true;
			})
		;

		return popup[0];
	},

	prompt: (ui, p) => {
		const popup = this.popup(ui);
		const body = popup[1];

		body.text({
			class: 'system-popup-text',
			text: p.text,
			top: 10,
			left: 10,
		});

		const input = body.input({
			class: 'system-popup-input',
			top: 30,
			left: 20,
			right: 10,
		});

		if (#is_defined(p.oktext)) {
			body.button({
				class: 'system-popup-button-ok',
				is_ok: true,
				text: p.oktext,
				align: 'top right',
				top: 60,
				right: 40,
			})
				.on('click', (e) => {
					if (#is_defined(p.on_ok)) {
						p.on_ok(input.value);
					}
					return true;
				})
			;
		}

		if (#is_defined(p.canceltext)) {
			body.button({
				class: 'system-popup-button-cancel',
				is_cancel: true,
				text: p.canceltext,
				align: 'top left',
				top: 60,
				left: 40,
			})
				.on('click', (e) => {
					if (#is_defined(p.on_cancel)) {
						p.on_cancel();
					}
					return true;
				})
			;
		}

		return popup[0];
	},

};
