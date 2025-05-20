return (i) => {
	const next = () => {
		if (i.settings.global.map.type == 'random') {
			i.steps.select_difficulty_level(i);
		} else {
			i.steps.customize_ocean_coverage(i);
		}
	};
	i.sliding.show({
		title: 'Select size of planet',
		entries: [
			['Tiny Planet', () => {
				i.settings.global.map.size_x = 68;
				i.settings.global.map.size_y = 34;
				next();
			}],
			['Small Planet', () => {
				i.settings.global.map.size_x = 88;
				i.settings.global.map.size_y = 44;
				next();
			}],
			['Standard Planet', () => {
				i.settings.global.map.size_x = 112;
				i.settings.global.map.size_y = 56;
				next();
			}],
			['Large Planet', () => {
				i.settings.global.map.size_x = 140;
				i.settings.global.map.size_y = 70;
				next();
			}],
			['Huge Planet', () => {
				i.settings.global.map.size_x = 180;
				i.settings.global.map.size_y = 90;
				next();
			}],
		]
	});
};
