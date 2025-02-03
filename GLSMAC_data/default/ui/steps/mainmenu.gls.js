return (m) => {

	m.glsmac.on('mainmenu', (e) => {

		#print('TODO: mainmenu');

		// tmp
		m.root.on('keydown', (e) => {
			if (e.code == 'ESCAPE') {
				m.glsmac.exit();
				return true;
			}
			return false;
		});

	});

};
