return {

	init: (p) => {

		p.ui.class('bottombar-minimap-info-text').set({
			font: 'arialn.ttf:14',
			color: 'rgb(60, 148, 124)',
		});

		p.ui.class('bottombar-minimap-button').set({
			align: 'left',
			width: 54,
			height: 32,
			top: 3,
		});

		this.frame = p.el.panel({
			align: 'bottom right',
			top: 59,
			bottom: 6,
			right: 7,
			width: 236,
		});

		const minimap = this.frame.widget({
			type: 'minimap',
			align: 'top left',
			top: 36,
			height: 112,
			left: 0,
			width: 236,
		});

		// the block under minimap
		const control_panel = this.frame.panel({
			class: 'bottombar-panel',
			align: 'bottom',
			bottom: 1,
			left: 1,
			right: 1,
			height: 38,
		});

		// info block
		const info_block = control_panel.panel({
			class: 'bottombar-panel-inner',
			align: 'left',
			left: 3,
			top: 3,
			bottom: 3,
			width: 114,
		});
		const info_year = info_block.text({
			class: 'bottombar-minimap-info-text',
			align: 'top left',
			top: 3,
			left: 3,
			text: 'Mission Year ' + #to_string(p.game.get_year()),
		});
		minimap.listen(p.game, 'turn', (e) => {
			info_year.text = 'Mission Year ' + #to_string(e.year);
		});
		const info_energy = info_block.text({
			class: 'bottombar-minimap-info-text',
			align: 'bottom left',
			bottom: 4,
			left: 3,
			text: 'Energy: 0',
		});

		// minimap-related buttons (no functionality yet)

		// TODO: make _hover and _active work without class
		p.ui.class('bottombar-minimap-button-left').extend('bottombar-minimap-button').set({
			background: 'interface.pcx:crop(532, 143, 585, 174)',
			_hover: {
				background: 'interface.pcx:crop(532, 176, 585, 207)',
			},
			_active: {
				background: 'interface.pcx:crop(532, 209, 585, 240)',
			},
		});
		control_panel.button({
			class: 'bottombar-minimap-button-left',
			text: 'button1',
			group: 'minimap-mode',
			left: 120,
		}).on('click', (e) => {
			minimap.show();
			return true;
		});

		p.ui.class('bottombar-minimap-button-right').extend('bottombar-minimap-button').set({
			background: 'interface.pcx:crop(587, 143, 640, 174)',
			_hover: {
				background: 'interface.pcx:crop(587, 176, 640, 207)',
			},
			_active: {
				background: 'interface.pcx:crop(587, 209, 640, 240)',
			},
		});
		control_panel.button({
			class: 'bottombar-minimap-button-right',
			text: 'button2',
			group: 'minimap-mode',
			left: 175,
		}).on('click', (e) => {
			minimap.hide();
			return true;
		});

		// 'turn complete' button

		const btn_turn_complete = this.frame.button({
			left: 2,
			top: 2,
			width: 233,
			height: 23,
			border: 'rgb(228, 104, 24),2',
			color: 'rgb(228, 104, 24),2',
			font: 'arialnb.ttf:17',
			text: 'TURN COMPLETE',
		});

		const blink_rate = 25;
		const blink_speed = 0.025;

		const blinker = btn_turn_complete.surface({
			left: 4,
			top: 4,
			right: 4,
			bottom: 4,
			background: 'console_x2_a.pcx:crop(198, 113, 375, 129)',
			opacity: 0.0,
		});
		let blink_timer = null;
		let blink_forward = true;
		let blink_opacity = 0.0;
		let is_mouseover = false;
		let allow_turn_complete = true;
		// TODO: use _hover and _active
		btn_turn_complete.on('mouseover', (e) => {
			if (allow_turn_complete && !is_mouseover) {
				is_mouseover = true;
				blinker.opacity = 0.5;
			}
			return false;
		});
		btn_turn_complete.on('mouseout', (e) => {
			if (is_mouseover) {
				is_mouseover = false;
				blinker.opacity = blink_opacity;
			}
			return false;
		});
		btn_turn_complete.on('mousedown', (e) => {
			if (allow_turn_complete && is_mouseover) {
				blinker.opacity = 0.8;
			}
			return false;
		});
		btn_turn_complete.on('mouseup', (e) => {
			if (allow_turn_complete && is_mouseover) {
				blinker.opacity = 0.5;
			} else {
				blinker.opacity = blink_opacity;
			}
			return false;
		});

		const f_blink_stop = () => {
			if (blink_timer != null) {
				blink_timer.stop();
				blink_timer = null;
				blink_opacity = 0.0;
				blinker.opacity = 0.0;
			}
		};
		const f_blink_start = () => {
			f_blink_stop();
			blink_timer = #async(blink_rate, () => {
				if (blink_forward) {
					blink_opacity += blink_speed;
					if (blink_opacity >= 1.0) {
						blink_opacity = 1.0;
						blink_forward = false;
					}
				} else {
					blink_opacity -= blink_speed;
					if (blink_opacity <= 0.25) {
						blink_opacity = 0.25;
						blink_forward = true;
					}
				}
				if (!is_mouseover) {
					blinker.opacity = blink_opacity;
				}
				return true;
			});
		};

		const turn_complete_sound = btn_turn_complete.sound({
			sound: 'cpu turn complete.wav',
			volume: 0.5,
		});

		let turn_status = 'active';
		const f_set_status = (status) => {
			if (turn_status != status) {
				turn_status = status;
				switch (turn_status) {
					case 'please_wait': {
						f_blink_stop();
						btn_turn_complete.text = 'PLEASE WAIT';
						allow_turn_complete = false;
						break;
					}
					case 'active': {
						f_blink_stop();
						btn_turn_complete.text = 'TURN COMPLETE';
						allow_turn_complete = true;
						break;
					}
					case 'turn_complete': {
						btn_turn_complete.text = 'TURN COMPLETE';
						allow_turn_complete = true;
						f_blink_start();
						turn_complete_sound.play();
						break;
					}
					case 'waiting_for_players': {
						f_blink_stop();
						btn_turn_complete.text = 'WAITING FOR PLAYERS';
						allow_turn_complete = true;
						break;
					}
					default: {
						#print('warning: unknown turn status: ' + status);
					}
				}
				if (!allow_turn_complete && is_mouseover) {
					is_mouseover = false;
					blinker.opacity = blink_opacity;
				}
			}
		};
		minimap.listen(p.game, 'turn_status', (e) => {
			f_set_status(e.status);
		});

		btn_turn_complete.on('click', (e) => {
			switch (turn_status) {
				case 'active': {
					p.modules.popup.show('turn_confirmation', (result) => {
						if (result) {
							f_set_status('please_wait');
							p.game.event('complete_turn', {});
						}
					});
					break;
				}
				case 'turn_complete': {
					f_set_status('please_wait');
					p.game.event('complete_turn', {});
					break;
				}
				case 'waiting_for_players': {
					f_set_status('please_wait');
					p.game.event('uncomplete_turn', {});
					break;
				}
			}
			return true;
		});

		this.frame.on('remove', (e) => { // TODO: dom async method
			f_blink_stop();
		});
	},

};
