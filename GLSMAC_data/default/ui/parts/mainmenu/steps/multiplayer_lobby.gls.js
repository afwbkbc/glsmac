return (i) => {

	let parts = {};
	
	i.popup.show({
		title: 'Multiplayer Setup',
		width: 800,
		height: 600,
		generator: (body) => {

			const ii = i + {
				make_section: (title, properties, generator) => {
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
				},
			};

			for (section of [
				'game_settings',
				'players',
				'custom_game_options',
			]) {
				parts[section] = #include('multiplayer_lobby/' + section)(ii);
			}

		}
	});

};
