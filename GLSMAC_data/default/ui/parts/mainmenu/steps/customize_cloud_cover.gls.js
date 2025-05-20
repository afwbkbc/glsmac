return (i) => {
	i.sliding.show({
		title: 'Select cloud cover',
		entries: [
			['Sparse', () => {
				i.settings.global.map.cloud_cover = 0.25;
				i.steps.select_difficulty_level(i);
			}],
			['Average', () => {
				i.settings.global.map.cloud_cover = 0.5;
				i.steps.select_difficulty_level(i);
			}],
			['Dense', () => {
				i.settings.global.map.cloud_cover = 0.75;
				i.steps.select_difficulty_level(i);
			}],
		]
	});
};
