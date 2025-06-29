return (ui) => {

	ui.class('lobby-player-row').set({
		left: 2,
		right: 2,
		height: 24,
	});

	ui.class('lobby-player-cell').set({
		height: 20,
		top: 2,
	});

	ui.class('lobby-player-block').extend('lobby-player-cell').set({
		border: 'rgb(60, 82, 106),2',
	});

	ui.class('lobby-player-ready').extend('lobby-player-block').set({
		left: 2,
		width: 16,
		background: 'interface.pcx:crop(8, 860, 31, 883)',
	});

	ui.class('lobby-player-button').extend('lobby-player-block').set({
		textalign: 'left',
		textleft: 2,
		texttop: 1,
		color: 'white',
		font: 'arialnb.ttf:18',
	});

	ui.class('lobby-player-choicelist').extend('lobby-player-cell').set({
		itemclass: 'lobby-player-choice',
		itemheight: 20,
		itempadding: 1,
		top: 0, // TODO: why not 2?
	});
	ui.class('lobby-player-choice').extend('lobby-player-block').set({
		background: 'interface.pcx:crop(86, 353, 109, 376)',
		color: 'white',
		font: 'arialnb.ttf:18',
		textalign: 'left',
		textleft: 2,
		_hover: {
			color: 'rgb(254,254,254)', // TODO: why it doesn't work without color?
			border: 'rgb(120, 164, 212),2',
		},
	});

	ui.class('lobby-player-name').extend('lobby-player-button').set({
		left: 20,
		width: 200,
	});

	ui.class('lobby-player-faction').extend('lobby-player-choicelist').set({
		left: 222,
		width: 140,
	});

	ui.class('lobby-player-difficulty').extend('lobby-player-choicelist').set({
		left: 364,
		width: 120,
	});

	ui.class('lobby-player-triangle').set({
		align: 'right center',
		height: 20,
		width: 20,
		background: 'red',
	});


};
