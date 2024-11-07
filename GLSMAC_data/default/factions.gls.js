return {

	configure: (fm) => {
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
			fm.add(#to_uppercase(faction), #include('./factions/' + faction));
		}
	},

};
