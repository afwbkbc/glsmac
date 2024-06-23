return {

	define: () => {
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
			#game.factions.define(#to_uppercase(faction), #include('./factions/' + faction));
		}
	},

};
