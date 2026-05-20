return (i) => {
	i.sliding.show({
		title: 'Select ocean coverage',
		entries: [
			['30-50% of surface', () => {
				i.settings.global.map.ocean_coverage = 0.4;
				i.steps.customize_erosive_forces(i);
			}, () => {
				i.set_customize_preview({
					ocean_coverage_type: 1,
				});
			}],
			['50-70% of surface', () => {
				i.settings.global.map.ocean_coverage = 0.6;
				i.steps.customize_erosive_forces(i);
			}, () => {
				i.set_customize_preview({
					ocean_coverage_type: 2,
				});
			}],
			['70-90% of surface', () => {
				i.settings.global.map.ocean_coverage = 0.8;
				i.steps.customize_erosive_forces(i);
			}, () => {
				i.set_customize_preview({
					ocean_coverage_type: 3,
				});
			}],
		],
		destructor: () => {
			i.set_customize_preview({
				ocean_coverage_type: i.customize_preview_defaults.ocean_coverage_type,
			});
		},
	});
};
