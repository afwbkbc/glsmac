return (i) => {

	const glsmac = i.glsmac;
	const game = glsmac.game;
	const settings = game.get_settings();

	const readonly = game.is_slave();

	return i.make_section(settings.global.game_name, {
		width: 296,
		height: 358,
	}, (body) => {

		let lines = [];
		let line_keys = {};
		const make_line = (key, label, choices, initial_choice, converter) => {
			line_keys[key] = #sizeof(lines);
			lines :+{
				key: key,
				label: label,
				choices: choices,
				initial_choice: initial_choice,
				converter: converter,
			};
		};
		const make_empty_line = () => {
			lines :+[];
		};

		make_line('difficulty', 'Difficulty Level', [
			// TODO
			/*['0', 'Citizen'],
			['1', 'Specialist'],
			['2', 'Talent'],
			['3', 'Librarian'],
			['4', 'Thinker'],*/
			['5', 'Transcend'],
		], '5', (v) => {
			return #to_int(v);
		});

		make_line('timer', 'Time Controls', [
			['none', 'None'],
			// TODO
			/*['5', '5 min'],
			['15', '15 min'],*/
		], 'none', (v) => {
			return v;
		});

		make_empty_line();

		make_line('game_type', 'Type of Game', [
			['new', 'Random map'],
			//['load', 'Load game'],
		], 'new', (v) => {
			return v;
		});

		make_line('planet_size', 'Planet Size', [
			['68x34', 'Tiny planet'],
			['88x44', 'Small planet'],
			['112x56', 'Standard planet'],
			['140x70', 'Large planet'],
			['180x90', 'Huge planet'],
		], '112x56', (v) => {
			return v; // TODO
		});

		make_line('ocean_coverage', 'Ocean Coverage', [
			['0.4', '30-50% of surface'],
			['0.6', '50-70% of surface'],
			['0.8', '70-90% of surface'],
		], '0.6', (v) => {
			return #to_float(v);
		});

		make_line('erosive_forces', 'Erosive Forces', [
			['0.5', 'Strong'],
			['0.75', 'Average'],
			['1.0', 'Weak'],
		], '0.75', (v) => {
			return #to_float(v);
		});

		make_line('native_lifeforms', 'Native Lifeforms', [
			['0.25', 'Rare'],
			['0.5', 'Average'],
			['0.75', 'Abundant'],
		], '0.5', (v) => {
			return #to_float(v);
		});

		make_line('cloud_cover', 'Cloud Cover', [
			['0.25', 'Sparse'],
			['0.5', 'Average'],
			['0.75', 'Dense'],
		], '0.5', (v) => {
			return #to_float(v);
		});

		const line_height = 28;

		game.on('game_settings', (e) => {
			for (s of e.settings) {
				if (#is_defined(line_keys[s[0]])) {
					// TODO: one line
					const el = lines[line_keys[s[0]]].select;
					el.value = #to_string(s[1]);
				}
			}
		});

		let top = line_height / 2;
		let sz = #sizeof(lines);
		for (let idx = 0; idx < sz; idx++) {
			let line = lines[idx];
			if (!#is_empty(line)) {
				body.text({
					class: 'popup-panel-text',
					top: top + 3,
					text: line.label + ':',
					left: 10,
				});
				line.select = body.select({
					top: top,
					class: 'popup-list-select',
					width: 140,
					align: 'right',
					right: 10,
					items: line.choices,
					value: line.initial_choice,
					readonly: readonly,
				});
				line.select.on('select', (e) => {
					game.event('game_settings', {
						changes: [
							[line.key, line.converter(e.value)],
						],
					});
					return true;
				});
			}
			top += line_height;
		}
	});

};
