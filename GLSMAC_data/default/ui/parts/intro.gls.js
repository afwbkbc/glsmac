return (m) => {

	m.glsmac.on('intro', (e) => {

		const logo = m.root.surface({
			align: 'center',
			width: 800,
			height: 600,
			background: 'logo.pcx',
		});

		let escape_handler_id = 0;
		let timer = null;
		const next = () => {
			m.root.off('keydown', escape_handler_id);
			logo.remove();
			e.mainmenu();
		};
		escape_handler_id = m.root.on('keydown', (e) => {
			if (e.code == 'ESCAPE') {
				timer.stop();
				next();
				return true;
			}
			return false;
		});
		timer = #async(1000, () => {
			next();
			return false;
		});

	});

};
