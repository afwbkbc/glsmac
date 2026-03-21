return {

	available_popups: [
		'turn_confirmation',
		'please_dont_go',
	],

	sliding_interval: 5,
	sliding_time: 100,

	popup_defs: {},
	popups: {},

	viewport_size: null,

	popup: null,
	popup_def: null,
	popup_cb: null,
	popup_result: null,

	sliding_timer: null,
	sliding_speed: null,
	target_top: null,

	bottombar_height: null,

	sound_up: null,
	sound_down: null,

	menu: null,

	init: (p) => {

		this.bottombar_height = p.modules.bottom_bar.height - 40; // skip transparent area in the middle of bottombar

		this.menu = p.modules.menu;

		const header_height = 24;
		const padding = 3;

		p.ui.class('game-popup').set({
			align: 'center',
			background: 'interface.pcx:crop(86, 665, 109, 688)',
			border: 'rgb(73, 108, 61),2',
			blocker: 'rgba(0, 0, 0, 120)',
		});
		p.ui.class('game-popup-block').set({
			background: 'interface.pcx:crop(86, 548, 109, 571)',
			border: 'rgb(35, 59, 34),2',
			left: padding,
			right: padding,
			top: padding,
		});
		p.ui.class('game-popup-header').extend('game-popup-block').set({
			align: 'top center',
			left: padding,
			top: padding,
			right: padding,
			height: header_height,
		});
		p.ui.class('game-popup-text').set({
			font: 'arialnb.ttf:18',
			color: 'rgb(92, 124, 188)',
			left: padding,
		});
		p.ui.class('game-popup-body').set({
			border: 'rgb(35, 59, 34),2',
			left: padding,
			right: padding,
			top: header_height + padding + padding,
		});
		p.ui.class('game-popup-button').set({
			left: padding,
			right: padding,
			height: 21,
			font: 'arialnb.ttf:17',
			color: 'black',
			background: 'interface.pcx:crop(47, 587, 70, 610)',
			border: 'black,2',
			sound: 'ok.wav',
			_hover: {
				border: 'rgb(120,164,212),2',
			},
			_active: {
				border: 'rgb(120,164,212),2',
				background: 'interface.pcx:crop(86, 587, 109, 610)',
			},
		});

		this.sound_up = p.ui.root.sound({
			sound: 'menu up.wav',
			volume: 0.5,
		});
		this.sound_down = p.ui.root.sound({
			sound: 'menu down.wav',
			volume: 0.5,
		});

		const that = this; // TODO

		this.viewport_size = {
			// we don't need to resize width because viewport width = root object width so align center works correctly
			height: p.ui.get_height() - that.bottombar_height,
		};

		const f_resize = (data) => {
			data.el.top = (this.viewport_size.height - data.height) / 2;
			data.el.bottom = 0;
		};

		p.ui.on('resize', (e) => {
			that.viewport_size = {
				height: e.height - that.bottombar_height,
			};
			const is_hiding = this.popup != null && this.popup_cb == null;
			that.sliding_stop();
			if (is_hiding) {
				this.hide();
			}
			for (data of this.popups) {
				f_resize(data);
			}
			if (this.popup != null) {
				this.sliding_speed = (this.viewport_size.height - this.popup.el.top) / (this.sliding_time / this.sliding_interval);
			}
		});

		const f_hide = (data) => {
			if (data != this.popup) {
				#print('WARNING: popup mismatch');
			}

			if (this.sliding_timer != null) {
				// if canceled early - start from where it is
				this.target_top = data.el.top;
			}
			this.sliding_stop();

			if (#is_defined(this.popup_def.on_hide)) {
				this.popup_def.on_hide();
			}

			this.target_top = this.viewport_size.height;
			this.sound_down.play();
			this.sliding_timer = #async(this.sliding_interval, () => {
				const new_top = data.el.top + this.sliding_speed;
				if (new_top >= this.target_top) {
					data.el.top = this.target_top;
					this.sliding_timer = null;
					if (that.popup_cb != null && that.popup_result != null) {
						that.popup_cb(that.popup_result);
						that.popup_cb = null;
						that.popup_result = null;
					}
					this.hide();
					f_resize(data);
					return false;
				}
				data.el.top = new_top;
				return true;
			});
		};

		const pp = {
			create: (title, width, height, generator) => {

				const popup = p.ui.root.window({
					class: 'game-popup',
					width: width + padding + padding,
				});
				popup.hide();

				const header = popup.panel({
					class: 'game-popup-header',
				});
				header.text({
					class: 'game-popup-text',
					align: 'center',
					text: title,
				});

				// side borders
				const borderheight = 529;
				popup.surface({
					width: 15,
					height: borderheight,
					background: 'console_x2_a.pcx:crop(713, 1, 727, 529)',
					align: 'top left',
					left: -23,
				});
				popup.surface({
					width: 8,
					height: borderheight,
					background: 'console_x2_a.pcx:crop(729, 1, 736, 529)',
					align: 'top left',
					left: -8,
				});
				popup.surface({
					width: 8,
					height: borderheight,
					background: 'console_x2_a.pcx:crop(738, 1, 745, 529)',
					align: 'top right',
					right: -8,
				});
				popup.surface({
					width: 15,
					height: borderheight,
					background: 'console_x2_a.pcx:crop(747, 1, 761, 529)',
					align: 'top right',
					right: -23,
				});

				const body = popup.panel({
					class: 'game-popup-body',
					height: height,
				});

				const f_result = (result) => {
					that.popup_result = result;
					f_hide(data);
				};

				generator(body, f_result);

				const data = {
					el: popup,
					height: height + header_height + padding,
				};

				popup.on('keydown', (e) => {
					if (e.code == 'ESCAPE') {
						f_result(false);
					}
					return true;
				});

				f_resize(data);

				return data;

			},
		};

		for (popup of this.available_popups) {
			this.popup_defs[popup] = #include(popup);
			this.popups[popup] = this.popup_defs[popup].init(pp);
		}

	},

	sliding_stop: () => {
		if (this.sliding_timer != null) {
			this.sliding_timer.stop();
			this.sliding_timer = null;
			this.popup.el.top = this.target_top;
		}
	},

	hide: () => {
		if (this.popup != null) {
			this.sliding_stop();
			this.popup.el.hide();
			this.popup = null;
			this.popup_def = null;
			this.popup_cb = null;
		}
	},

	show: (popup, cb) => {
		this.menu.close_all();
		if (this.popup != null) {
			this.hide();
		}
		this.popup = this.popups[popup];
		this.popup_def = this.popup_defs[popup];
		if (!#is_defined(this.popup) || !#is_defined(this.popup_def)) {
			throw Error('Unknown popup: ' + popup);
		}
		this.popup_cb = cb;

		if (#is_defined(this.popup_def.on_show)) {
			this.popup_def.on_show();
		}

		this.target_top = this.popup.el.top;
		this.sliding_speed = (this.viewport_size.height - this.target_top) / (this.sliding_time / this.sliding_interval);
		this.popup.el.top = this.viewport_size.height;
		this.sound_up.play();
		this.sliding_timer = #async(this.sliding_interval, () => {
			const new_top = this.popup.el.top - this.sliding_speed;
			if (new_top <= this.target_top) {
				this.popup.el.top = this.target_top;
				this.sliding_timer = null;
				return false;
			}
			this.popup.el.top = new_top;
			return true;
		});

		this.popup.el.show();
	},

};
