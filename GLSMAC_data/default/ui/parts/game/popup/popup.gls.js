return {

	popups: {},

	popup_cb: null,

	init: (p) => {

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
			_hover: {
				border: 'rgb(120,164,212),2',
			},
			_active: {
				border: 'rgb(120,164,212),2',
				background: 'interface.pcx:crop(86, 587, 109, 610)',
			},
		});


		let viewport_size = {
			// we don't need to resize width because viewport width = root object width so align center works correctly
			height: p.ui.get_height() - p.modules.bottom_bar.height,
		};

		const f_resize = (data) => {
			data.el.top = (viewport_size.height - data.height) / 2;
			data.el.bottom = 0;
		};

		p.ui.on('resize', (e) => {
			viewport_size = {
				height: e.height - p.modules.bottom_bar.height,
			};
			for (data of this.popups) {
				f_resize(data);
			}
		});

		const f_hide = (data) => {
			data.el.hide();
			this.popup_cb = null;
		};

		const that = this; // TODO

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
					if (that.popup_cb != null) {
						that.popup_cb(result);
					}
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

		for (popup of [
			'turn_confirmation',
		]) {
			this.popups[popup] = #include(popup).init(pp);
		}

	},

	show: (popup, cb) => {
		const p = this.popups[popup];
		if (!#is_defined(p)) {
			throw Error('Unknown popup: ' + popup);
		}
		this.popup_cb = cb;
		p.el.show();
	},

};
