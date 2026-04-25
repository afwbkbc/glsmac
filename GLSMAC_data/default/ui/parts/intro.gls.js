return (m) => {

	m.glsmac.on('intro', (e) => {

		const logo = m.root.surface({
			align: 'center',
			width: 800,
			height: 600,
			background: 'logo.pcx',
		});

		const next = () => {
			logo.remove();
			m.glsmac.mainmenu();
		};
		logo.listen(m.root, 'keydown', (e) => {
			if (e.code == 'ESCAPE') {
				next();
				return true;
			}
			return false;
		});
		logo.timer(1000, () => {
			next();
			return false;
		});

	});

};
