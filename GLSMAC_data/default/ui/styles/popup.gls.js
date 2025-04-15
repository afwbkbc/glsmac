return (ui) => {

	ui.class('popup').set({
		background: 'interface.pcx:crop(86,  353, 109, 376)',
		border: 'rgb(73, 108, 61),2',
		align: 'center',
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

	ui.class('popup-footer').set({
		align: 'bottom',
		bottom: 3,
		left: 3,
		right: 3,
		height: 23,
	});

	ui.class('popup-header-text').set({
		font: 'arialnb.ttf:19',
		color: 'rgb(109, 126, 178)',
		align: 'top center',
		top: 2,
		transform: 'uppercase',
	});

	ui.class('popup-body').set({
		border: 'rgb(73, 108, 61),1',
		align: 'top',
		top: 28,
		left: 3,
		right: 3,
		bottom: 28,
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

	ui.class('popup-button').set({
		background: 'interface.pcx:crop(68, 149, 134, 166)',
		width: 230,
		height: 21,
		font: 'arialnb.ttf:18',
		transform: 'uppercase',
		sound: 'ok.wav',
		volume: 0.5,
		color: 'rgb(13, 23, 36)',
		border: 'transparent,2',
		_hover: {
			color: 'rgb(164, 176, 232)',
			border: 'rgb(120, 164, 212),2',
		},
		_active: {
			background: 'interface.pcx:crop(68,  170, 134, 187)',
			border: 'rgb(120, 164, 212),2',
		},
	});

};
