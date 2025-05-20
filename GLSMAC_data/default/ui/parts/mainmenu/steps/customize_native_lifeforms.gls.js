return (i) => {
	i.sliding.show({
		title: 'Native life forms',
		entries: [
			['Rare', () => {
				i.settings.global.map.native_lifeforms = 0.25;
				i.steps.customize_cloud_cover(i);
			}],
			['Average', () => {
				i.settings.global.map.native_lifeforms = 0.5;
				i.steps.customize_cloud_cover(i);
			}],
			['Abundant', () => {
				i.settings.global.map.native_lifeforms = 0.75;
				i.steps.customize_cloud_cover(i);
			}],
		]
	});
};
