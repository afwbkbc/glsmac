return {

	validate: (e) => {
		if (e.game.is_started()) {
			return 'Game has already started';
		}
		if (e.caller != 0) {
			return 'Only master is allowed to change game settings';
		}
	},

	apply: (e) => {
		let old_settings = [];
		let settings = e.game.get_settings().global.map;
		for (c of e.data.changes) {
			const oldv = settings[c[0]]; // TODO: fix deref
			if (#is_defined(oldv)) { // TODO: support all fields
				old_settings :+[c[0], oldv];
				settings[c[0]] = c[1]; // TODO: ro/rw permissions on settings in all places
			}
		}
		e.game.trigger('game_settings', {
			settings: e.data.changes,
		});
		return {
			old_settings: old_settings,
		};
	},

	rollback: (e) => {
		let settings = e.game.get_settings().global.map;
		for (c of e.applied.old_settings) {
			settings[c[0]] = c[1];
		}
		e.game.trigger('game_settings', {
			settings: e.applied.old_settings
		});
	},

};
