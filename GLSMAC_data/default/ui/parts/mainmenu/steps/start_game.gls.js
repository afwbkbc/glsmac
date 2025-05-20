return (i) => {
	i.sliding.show({
		entries: [
			['Make Random Map', () => {
				i.settings.global.map.type = 'random';
				i.randomize_map();
				i.steps.select_mapsize(i);
			}],
			['Customize Random Map', () => {
				i.settings.global.map.type = 'custom';
				i.steps.select_mapsize(i);
			}],
			['The Map of Planet', () => {
				i.steps.notimpl(i);
			}],
			['Huge Map of Planet', () => {
				i.steps.notimpl(i);
			}],
			['Load Map File', () => {
				i.steps.notimpl(i);
			}],
		]
	});
};
