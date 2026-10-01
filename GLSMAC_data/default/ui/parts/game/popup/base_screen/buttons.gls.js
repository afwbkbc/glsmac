return {

	hide_buttons: () => {
		for (button of this.buttons) {
			button.remove();
		}
		this.buttons = [];
	},

	show_default: () => {
		this.hide_buttons();

		const btn_rename = this.frame.button({
			class: 'base-screen-popup-bottom-button',
			align: 'left',
			left: 3,
			width: 210,
			text: 'RENAME',
		});
		const btn_ok = this.frame.button({
			class: 'base-screen-popup-bottom-button',
			align: 'right',
			right: 3,
			width: 210,
			text: 'OK',
			is_ok: true,
			is_cancel: true,
		});

		btn_ok.on('click', (e) => {
			this.p.hide();
			return false;
		});

		this.buttons = [btn_rename, btn_ok];
	},

	show_production_selection: () => {
		this.hide_buttons();

		const btn_ok = this.frame.button({
			class: 'base-screen-popup-bottom-button',
			align: 'left',
			left: 215,
			width: 104,
			text: 'OK',
			is_ok: true,
		});

		const btn_cancel = this.frame.button({
			class: 'base-screen-popup-bottom-button',
			align: 'left',
			left: 321,
			width: 104,
			text: 'CANCEL',
			is_cancel: true,
		});

		this.buttons = [
			this.frame.button({
				class: 'base-screen-popup-bottom-button',
				align: 'left',
				left: 3,
				width: 104,
				text: 'HELP',
			}),
			this.frame.button({
				class: 'base-screen-popup-bottom-button',
				align: 'left',
				left: 109,
				width: 104,
				text: 'WORKSHOP',
			}),
			btn_ok,
			btn_cancel,
		];

		btn_cancel.on('click', (e) => {
			this.p.utils.hide_production_selection();
			return true;
		});

		btn_ok.on('click', (e) => {
			this.p.utils.update_production_selection();
			return true;
		});

	},

	init: (p) => {

		this.p = p;
		this.buttons = [];

		this.frame = p.body.panel({
			class: 'base-screen-frame',
			align: 'bottom center',
			width: 428,
			height: 28,
			bottom: -32,
		});

		p.ui.class('base-screen-popup-bottom-button').extend('game-popup-button').set({
			top: 3,
			bottom: 3,
			width: 210, // TODO: why doesn't this work?
		});

		this.show_default();

		return this.frame;
	},

};
