return {

	init: (p) => {
		this.p = p;
		this.base = null;

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

		this.btn_hurry = this.frame.button({
			class: 'base-screen-popup-bottom-button',
			align: 'left',
			left: 3,
			width: 210,
			text: 'HURRY',
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
		this.btn_hurry.on('click', (e) => {
			if (this.base != null && this.p.game.get('f_economy_get_hurry_cost')(this.base) > 0) {
				this.p.game.event('hurry_base_production', {base: this.base});
			}
			return false;
		});

	},

	set: (data) => {
		this.base = data.base;
		const cost = this.p.game.get('f_economy_get_hurry_cost')(this.base);
		this.btn_hurry.text = cost > 0 ? 'HURRY (' + #to_string(cost) + ')' : 'HURRY';
	},

};
