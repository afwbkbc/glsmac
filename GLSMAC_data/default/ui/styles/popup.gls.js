return (ui) => {

	ui.class('popup').set({
		background: 'interface.pcx:crop(86, 353, 109, 376)',
		border: 'rgb(73, 108, 61),2',
		align: 'center',
	});

	ui.class('popup-footer').set({
		align: 'bottom',
		bottom: 3,
		left: 3,
		right: 3,
		height: 23,
	});

	ui.class('popup-header').set({
		background: 'interface.pcx:crop(86,  314, 109, 337)',
		border: 'rgb(73, 108, 61),1',
		align: 'top',
		top: 3,
		left: 3,
		right: 3,
		height: 23,
	});

	ui.class('popup-panel-header').extend('popup-header').set({
		top: 0,
		left: 0,
		right: 0,
		height: 20,
	});

	ui.class('popup-header-text').set({
		font: 'arialnb.ttf:19',
		color: 'rgb(109, 126, 178)',
		align: 'top center',
		top: 2,
		transform: 'uppercase',
	});
	ui.class('popup-panel-header-text').extend('popup-header-text').set({
		font: 'arialnb.ttf:17',
		top: 3,
	});

	ui.class('popup-panel').set({
		border: 'rgb(73, 108, 61),1',
		align: 'top left',
		top: 3,
		left: 3,
		right: 3,
		bottom: 3,
	});
	ui.class('popup-body').extend('popup-panel').set({
		top: 28,
		bottom: 28,
	});
	ui.class('popup-panel-body').extend('popup-panel').set({
		top: 20,
		left: 0,
		right: 0,
		bottom: 0,
	});
	ui.class('popup-panel-body-noheader').extend('popup-panel-body').set({
		top: 0,
	});
	ui.class('popup-body-noheader').set({
		top: 3,
	});
	ui.class('popup-body-nofooter').set({
		bottom: 3,
	});

	ui.class('popup-text').set({
		color: 'rgb(109, 126, 178)',
		font: 'arialnb.ttf:18',
	});

	ui.class('popup-panel-text').extend('popup-text').set({
		font: 'arialnb.ttf:17',
	});

	ui.class('popup-button-base').set({
		background: 'interface.pcx:crop(68, 149, 134, 166)',
		color: 'rgb(13, 23, 36)',
		border: 'transparent,2',
		_hover: {
			color: 'rgb(164, 176, 232)',
			border: 'rgb(120, 164, 212),2',
		},
		_active: {
			color: 'rgb(13, 23, 36)',
			background: 'interface.pcx:crop(68,  170, 134, 187)',
			border: 'rgb(120, 164, 212),2',
		},
	});

	ui.class('popup-button').extend('popup-button-base').set({
		width: 230,
		height: 21,
		font: 'arialnb.ttf:18',
		transform: 'uppercase',
		sound: 'ok.wav',
		volume: 0.5,
	});
	ui.class('popup-list-button').extend('popup-button-base').set({
		height: 19,
		font: 'arialnb.ttf:17',
		_selected: {
			color: 'rgb(13, 23, 36)',
			background: 'interface.pcx:crop(68,  170, 134, 187)',
			border: 'rgb(120, 164, 212),2',
		},
	});

	ui.class('popup-list-select').set({
		height: 21,
		itemheight: 21,
		itempadding: 0,
		itemclass: 'popup-list-select-item',
	});
	ui.class('popup-list-select-item').set({
		height: 21,
		font: 'arialnb.ttf:18',
		color: 'rgb(144, 184, 228)',
		border: 'rgb(60, 82, 106),2',
		background: 'interface.pcx:crop(86, 353, 109, 376)',
		_hover: {
			color: 'rgb(164, 214, 248)',
			border: 'rgb(120, 164, 212),2',
		},
		_selected: {
			color: 'rgb(184, 238, 255)',
			border: 'rgb(130, 178, 232),2',
		},
		_highlight: {
			color: 'white',
			border: 'white',
		},
	});

	ui.class('popup-input').set({
		background: 'interface.pcx:crop(86, 353, 109, 376)',
		color: 'rgb(159, 196, 198)',
		border: 'rgb(60, 82, 106),2',
		height: 23,
		font: 'arialnb.ttf:18',
	});

	ui.class('popup-list-scroll-from').set({
		background: 'icons.pcx:crop(107, 4, 124, 21)',
		_hover: {
			background: 'icons.pcx:crop(132, 4, 149, 21)',
		},
		_active: {
			background: 'icons.pcx:crop(157, 4, 174, 21)',
		},
	});
	ui.class('popup-list-scroll-to').set({
		background: 'icons.pcx:crop(107, 29, 124, 46)',
		_hover: {
			background: 'icons.pcx:crop(132, 29, 149, 46)',
		},
		_active: {
			background: 'icons.pcx:crop(157, 29, 174, 46)',
		},
	});
	ui.class('popup-list-scroll-slider').set({
		background: 'icons.pcx:crop(260, 4, 276, 17)',
		_hover: {
			background: 'icons.pcx:crop(104, 104, 121, 115)',
		},
	});
	ui.class('popup-list-scrollbar').set({
		scroll_type: 'vertical',
		fromto_size: 18,
		slider_size: 18,
		class_from: 'popup-list-scroll-from',
		class_to: 'popup-list-scroll-to',
		class_slider: 'popup-list-scroll-slider',
		min: 0.0,
		max: 1.0,
		value: 0.0,
		speed: 0.5,
		width: 18,
		top: 3,
		bottom: 3,
		right: 3,
	});
	ui.class('popup-list').set({
		background: 'interface.pcx:crop(86, 353, 109, 376)',
		border: 'rgb(60, 82, 106),2',
		padding: 3,
		itemsize: 20,
		scroll_type: 'vertical',
		vscroll_class: 'popup-list-scrollbar',
		autoscroll: true,
		has_hscroll: false,
	});
	ui.class('popup-list-line').set({
		color: 'rgb(159, 196, 198)',
		font: 'arialnb.ttf:18',
	});
};
