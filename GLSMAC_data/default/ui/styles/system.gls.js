return (ui) => {

	const colors = {
		sysfg: 'rgb(10, 80, 60)',
		sysbg: 'rgb(12, 12, 16)',
		systext: 'silver',
	};

	ui.class('system-popup').set({
		background: colors.sysbg,
		border: colors.sysfg,
	});

	ui.class('system-popup-header').set({
		align: 'top',
		left: 0,
		right: 0,
		height: 24,
		background: 'rgb(10, 80, 60)',
	});

	ui.class('system-popup-header-text').set({
		align: 'center',
		color: 'white',
		font: ':24',
	});

	ui.class('system-popup-body').set({
		align: 'top',
		left: 1,
		right: 1,
		top: 25,
		bottom: 1,
		background: colors.sysbg,
	});

	ui.class('system-popup-text').set({
		color: colors.systext,
		font: ':16',
	});

	ui.class('system-popup-input').set({
		color: colors.systext,
		border: colors.systext,
		background: 'black',
		font: ':16',
		height: 20,
	});

	ui.class('system-popup-button').set({
		color: colors.systext,
		font: ':22',
		width: 200,
		height: 24,
		border: 'gray',
		_hover: {
			border: 'white',
			color: 'white',
		},
	});
	ui.class('system-popup-button-ok').extend('system-popup-button').set({
		background: '#002200',
		_hover: {
			background: '#004400',
		},
	});
	ui.class('system-popup-button-cancel').extend('system-popup-button').set({
		background: '#220000',
		_hover: {
			background: '#440000',
		},
	});

};
