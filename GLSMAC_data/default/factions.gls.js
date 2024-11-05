return {

	configure: (factions) => {
		for (faction of [
			'gaians',
			'hive',
			'university',
			'morganites',
			'spartans',
			'believers',
			'peacekeepers',
			'consciousness',
			'pirates',
			'drones',
			'angels',
			'planetcult',
			'caretakers',
			'usurpers'
		]) {
			factions.add(#to_uppercase(faction), #include('./factions/' + faction));
		}
	},

};
