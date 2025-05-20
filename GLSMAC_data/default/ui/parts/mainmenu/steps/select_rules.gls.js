return (i) => {
	i.sliding.show({
		title: 'Game rules',
		entries: [
			['Play with Standard Rules', () => {
				// TODO: rules
				// TODO: faction selection
				i.glsmac.start_game();
			}],
			['Play with Current Rules', () => {
				// TODO: rules
				// TODO: faction selection
				i.glsmac.start_game();
			}],
			['Customize Rules', () => {
				// TODO: custom rules
				i.steps.notimpl(i);
			}],
		]
	});
};
