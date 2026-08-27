return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only the game master can process The Planetary Datalinks';
		}
	},

	apply: (e) => {
		return e.game.get('f_project_apply_planetary_datalinks')();
	},

	rollback: (e) => {
		e.game.get('f_project_rollback_planetary_datalinks')(e.applied);
	},

};
