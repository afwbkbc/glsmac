return (i) => {
	i.sliding.show({
		title: 'Select ocean coverage',
		entries: [
			['30-50% of surface', () => {
				i.settings.global.map.ocean_coverage = 0.4;
				i.steps.customize_erosive_forces(i);
			}],
			['50-70% of surface', () => {
				i.settings.global.map.ocean_coverage = 0.6;
				i.steps.customize_erosive_forces(i);
			}],
			['70-90% of surface', () => {
				i.settings.global.map.ocean_coverage = 0.8;
				i.steps.customize_erosive_forces(i);
			}],
		]
	});
};
