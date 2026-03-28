return {

	init: (p) => {

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
			p.hide();
			return false;
		});

	},

};
