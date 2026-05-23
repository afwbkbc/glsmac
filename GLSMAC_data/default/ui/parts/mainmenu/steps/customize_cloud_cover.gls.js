return (i) => {
	i.sliding.show({
		title: 'Select cloud cover',
		entries: [
			['Sparse', () => {
				i.settings.global.map.cloud_cover = 0.25;
				i.steps.select_difficulty_level(i);
			}, () => {
				i.set_customize_preview({
					cloud_cover_type: 1,
				});
			}],
			['Average', () => {
				i.settings.global.map.cloud_cover = 0.5;
				i.steps.select_difficulty_level(i);
			}, () => {
				i.set_customize_preview({
					cloud_cover_type: 2,
				});
			}],
			['Dense', () => {
				i.settings.global.map.cloud_cover = 0.75;
				i.steps.select_difficulty_level(i);
			}, () => {
				i.set_customize_preview({
					cloud_cover_type: 3,
				});
			}],
		],
		destructor: () => {
			i.set_customize_preview({
				cloud_cover_type: i.customize_preview_defaults.cloud_cover_type,
			});
		},
	});
};
