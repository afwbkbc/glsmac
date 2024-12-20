#main((glsmac) => {

	const root = glsmac.ui.root;

	for (align of [
		'top left',
		'top center',
		'top right',
		'center left',
		'center',
		'center right',
		'bottom left',
		'bottom center',
		'bottom right',
	]) {
		root.img({
			align: align,
			width: 50,
			height: 50,
		});
	}

});
