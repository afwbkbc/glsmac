return (i) => {
	i.sliding.show({
		title: 'Adjust erosive forces',
		entries: [
			['Strong', () => {
				i.settings.global.map.erosive_forces = 0.5;
				i.steps.customize_native_lifeforms(i);
			}],
			['Average', () => {
				i.settings.global.map.erosive_forces = 0.75;
				i.steps.customize_native_lifeforms(i);
			}],
			['Weak', () => {
				i.settings.global.map.erosive_forces = 1.0;
				i.steps.customize_native_lifeforms(i);
			}],
		]
	});
};
