return (i) => {

	let parts = {};

	const config = i.glsmac.config;

	i.popup.show({
		title: 'Multiplayer Setup',
		width: 800,
		height: 600,
		generator: (body) => {

			const make_section = (title, properties, generator) => {
				let section = body.panel({
					class: 'popup-panel',
				} + properties);
				section.panel({
					class: 'popup-panel-header',
				}).text({
					class: 'popup-panel-header-text',
					text: title,
				});
				let inner = section.panel({
					class: 'popup-panel-body',
				});
				generator(inner);
				return section;
			};

			parts.game_settings = make_section(config.gamename, {
				width: 296,
				height: 358,
			}, (body) => {

				let lines = [];
				const make_line = (label, choices, on_change) => {
					lines :+[label, choices, on_change];
				};
				const make_empty_line = () => {
					lines :+[];
				};

				make_line('Global Difficulty Level', [
					['0', 'Citizen'],
					['1', 'Specialist'],
					['2', 'Talent'],
					['3', 'Librarian'],
					['4', 'Thinker'],
					['5', 'Transcend'],
				], (value) => {
					#print('DIFFICULTY:', value);
				});

				make_line('Time Controls', [
					['none', 'None'],
					['5', '5 min'],
					['15', '15 min'],
				], (value) => {
					#print('TIME CONTROLS:', value);
				});

				make_empty_line();

				make_line('Type of Game', [
					['new', 'Random map'],
					['load', 'Load game'],
				], (value) => {
					#print('TYPE OF GAME', value);
				});

				// small hack until zindex is fully fixed: add lines in reverse order so that higher selects were drawn on top of lower selects
				let top = 21 * (#sizeof(lines) - 1) + 6;
				for (i = #sizeof(lines); i > 0; i--) {
					const line = lines[i - 1];
					if (#sizeof(line) > 0) {
						body.text({
							class: 'popup-panel-text',
							top: top + 3,
							text: line[0] + ':',
							left: 10,
						});
						const select = body.select({
							top: top,
							class: 'popup-list-select',
							items: line[1],
						});
						select.on('select', (e) => {
							line[2](e.value);
							return true;
						});
					}
					top -= 21;
				}
			});

			parts.players = make_section('Players', {
				left: 301,
				width: 490,
				height: 358,
			}, (body) => {

			});

			parts.custom_game_options = make_section('Custom Game Options', {
				top: 363,
				width: 788,
				height: 204,
			}, (body) => {

			});

		}
	});

};
