const unready_players = (players) => {
	for (player of players) {
		player.set_ready(false);
	}
};

const validate_planet_size = (value) => {
	if (#typeof(value) != 'String') {
		return 'Planet size must use WIDTHxHEIGHT format';
	}
	const xy = #split(value, 'x');
	if (#sizeof(xy) != 2) {
		return 'Planet size must use WIDTHxHEIGHT format';
	}
	let width = 0;
	let height = 0;
	let parse_error = false;
	try {
		width = #to_int(xy[0]);
		height = #to_int(xy[1]);
	} catch {
	:
		(e) => {
			parse_error = true;
		}
	}
	if (parse_error) {
		return 'Planet width and height must be whole numbers';
	}
	if (width < 4 || height < 4 || width % 2 != 0 || height % 2 != 0) {
		return 'Planet width and height must be even numbers of at least 4';
	}
	const max_area = 180 * 90;
	if (width > max_area || height > max_area || width * height > max_area) {
		return 'Planet area cannot exceed Huge Planet (180x90)';
	}
};

const validate_percentage = (name, value) => {
	if (#typeof(value) != 'Float' || value < 0.0 || value > 1.0) {
		return name + ' must be a number from 0.0 to 1.0';
	}
};

return {

	validate: (e) => {
		if (e.game.is_started()) {
			return 'Game has already started';
		}
		if (e.caller != 0) {
			return 'Only master is allowed to change game settings';
		}
		if (#typeof(e.data.changes) != 'Array' || #sizeof(e.data.changes) == 0) {
			return 'Game settings changes must be a non-empty array';
		}
		for (c of e.data.changes) {
			if (#typeof(c) != 'Array' || #sizeof(c) != 2 || #typeof(c[0]) != 'String') {
				return 'Each game settings change must contain a name and value';
			}
			let error = #undefined;
			switch (c[0]) {
				case 'planet_size': {
					error = validate_planet_size(c[1]);
					break;
				}
				case 'ocean_coverage': {
					error = validate_percentage('Ocean coverage', c[1]);
					break;
				}
				case 'erosive_forces': {
					error = validate_percentage('Erosive forces', c[1]);
					break;
				}
				case 'native_lifeforms': {
					error = validate_percentage('Native lifeforms', c[1]);
					break;
				}
				case 'cloud_cover': {
					error = validate_percentage('Cloud cover', c[1]);
					break;
				}
				default: {
					return 'Unsupported game setting: ' + c[0];
				}
			}
			if (#is_defined(error)) {
				return error;
			}
		}
	},

	apply: (e) => {
		let old_settings = [];
		let old_ui_settings = [];
		let ready_states = [];
		let settings = e.game.get_settings().global.map;
		let changes = [];
		let captured_settings = {};
		let captured_ui_settings = {};
		for (player of e.game.get_players()) {
			ready_states :+[player.id, player.is_ready()];
		}
		for (c of e.data.changes) {
			if (!#is_defined(captured_ui_settings[c[0]])) {
				if (c[0] == 'planet_size') {
					old_ui_settings :+[
						'planet_size',
						#to_string(settings.size_x) + 'x' + #to_string(settings.size_y),
					];
				} else {
					old_ui_settings :+[c[0], settings[c[0]]];
				}
				captured_ui_settings[c[0]] = true;
			}
			if (c[0] == 'planet_size') {
				const xy = #split(c[1], 'x');
				changes :+['size_x', #to_int(xy[0])];
				changes :+['size_y', #to_int(xy[1])];
			} else {
				changes :+c;
			}
		}
		for (c of changes) {
			const oldv = settings[c[0]];
			if (#is_defined(oldv)) {
				if (!#is_defined(captured_settings[c[0]])) {
					old_settings :+[c[0], oldv];
					captured_settings[c[0]] = true;
				}
				settings[c[0]] = c[1]; // TODO: ro/rw permissions on settings in all places
			}
		}

		unready_players(e.game.get_players());

		e.game.trigger('game_settings', {
			settings: e.data.changes,
		});
		return {
			old_settings: old_settings,
			old_ui_settings: old_ui_settings,
			ready_states: ready_states,
		};
	},

	rollback: (e) => {
		let settings = e.game.get_settings().global.map;
		for (c of e.applied.old_settings) {
			settings[c[0]] = c[1];
		}

		for (state of e.applied.ready_states) {
			e.game.get_player(state[0]).set_ready(state[1]);
		}

		e.game.trigger('game_settings', {
			settings: e.applied.old_ui_settings
		});
	},

};
