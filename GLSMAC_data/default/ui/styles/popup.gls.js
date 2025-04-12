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
		bottom: 3,
	});

};
