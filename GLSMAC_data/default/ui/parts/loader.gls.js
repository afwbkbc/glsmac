return (m) => {

	let loader = null;

	m.glsmac.on('loader_show', (e) => {
		if (loader == null) {
			loader = m.root.panel({
				class: 'smac-loader',
			});
		}

	});

	m.glsmac.on('loader_text', (e) => {
		if (loader != null) {
			
		}
	});

	m.glsmac.on('loader_hide', (e) => {
		if (loader != null) {
			loader.remove();
			loader = null;
		}

	});

};
