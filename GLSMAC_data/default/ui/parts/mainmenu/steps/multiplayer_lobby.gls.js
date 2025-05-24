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

				let top = 6;

				const make_line = (label, choices) => {
					body.text({
						class: 'popup-panel-text',
						top: top + 3,
						text: label + ':',
						left: 10,
					});
					const select = body.select({
						top: top,
						width: 100,
						align: 'right',
						right: 10,
						itemclass: 'popup-list-button',
						itemheight: 19,
						itempadding: 1,
						items: choices,
					});
					select.on('select', (e) => {
						#print('SELECTED', e);
						return true;
					});
					top += 21;
				};

				make_line('Global Difficulty Level', [
					['0', 'Citizen'],
					['1', 'Specialist'],
				]);

				make_line('Time Controls', [
					['none', 'None'],
					['5', '5 min'],
					['15', '15 min'],
				]);

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
