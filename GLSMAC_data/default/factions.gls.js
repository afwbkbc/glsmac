return {

	define: () => {
		const factions = ['gaians', 'hive', 'university', 'morganites', 'spartans', 'believers', 'peacekeepers', 'consciousness', 'pirates', 'drones', 'angels', 'planetcult', 'caretakers', 'usurpers'];
		let i = 0;
		let sz = #size(factions);
		while (i < sz) {
			#print(#to_uppercase(factions[i]));
			#game.factions.define(#to_uppercase(factions[i]), #include('./factions/' + factions[i]));
			i++;
		}
	},

};
