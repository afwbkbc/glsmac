return (i) => {
	let entries = [];
	for (difficulty_level of i.settings.global.rules.difficulty_levels) {
		const d = difficulty_level; // TODO: gse fix: keep difficulty_level available here
		entries :+[difficulty_level, () => {
			i.settings.global.difficulty_level = d;
			i.steps.select_rules(i);
		}];
	}
	i.sliding.show({
		title: 'Pick a difficulty level',
		entries: entries
	});
};
