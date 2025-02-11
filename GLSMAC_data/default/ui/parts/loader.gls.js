return (m) => {

	let loader = null;
	let loadertext = null;

	m.glsmac.ui.class('smac-loader').set({
		width: 300,
		height: 120,
		background: 'interface.pcx:crop(86, 353, 109, 376)',
		border: 'rgb(35, 59, 34)',
		header_height: 22,
		header_font: 'arialnb.ttf:18',
		header_background: 'interface.pcx:crop(86, 314, 109, 337)',
		header_border: 'rgb(35, 59, 34),2',
		header_color: 'rgb(109, 126, 178)',
		header_padding: 3,
		header_transform: 'uppercase',
		blocker: 'transparent',
		title: 'Loading',
	});

	m.glsmac.ui.class('smac-loader-text').set({
		align: 'center',
		color: 'rgb(109, 126, 178)',
		font: 'arialnb.ttf:21',
	});

	m.glsmac.on('loader_show', (e) => {
		if (loader == null) {
			loader = m.root.window({
				class: 'smac-loader',
			});
			loadertext = loader.text({
				class: 'smac-loader-text',
			});
		}

	});

	m.glsmac.on('loader_text', (e) => {
		if (loader != null) {
			loadertext.text = e.text;
		}
	});

	m.glsmac.on('loader_hide', (e) => {
		if (loader != null) {
			loader.remove();
			loader = null;
			loadertext = null;
		}

	});

};
