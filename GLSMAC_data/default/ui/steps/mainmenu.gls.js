return (m) => {

	m.glsmac.on('mainmenu', (e) => {

		m.root.surface({
			id: 'mainmenu-background',
			top: 0,
			left: 0,
			right: 0,
			bottom: 0,
			background: 'openinga.pcx:stretch()',
		});

		const menublock = #include('mainmenu/menublock')(m);

		menublock.show([
			['Start Game', () => {
				#print('START GAME');
			}],
			['Quick Start', () => {

			}],
			['Scenario', () => {

			}],
			['Load Game', () => {

			}],
			['Multiplayer', () => {

			}],
			['View Credits', () => {

			}],
			['Exit Game', () => {
				m.glsmac.exit();
			}],
		], () => {
			m.glsmac.exit();
		});

	});

};
