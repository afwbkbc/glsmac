return (i) => {

	const glsmac = i.glsmac;
	const game = glsmac.game;

	return i.make_section(glsmac.config.gamename, {
		width: 296,
		height: 358,
	}, (body) => {

		let lines = [];
		const make_line = (key, label, choices, initial_choice, on_change) => {
			lines :+{
				key: key,
				label: label,
				choices: choices,
				initial_choice: initial_choice,
				on_change: on_change,
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
		], '5', (value) => {
			#print('DIFFICULTY:', value);
		});

		make_line('timer', 'Time Controls', [
			['none', 'None'],
			// TODO
			/*['5', '5 min'],
			['15', '15 min'],*/
		], 'none', (value) => {
			#print('TIME CONTROLS:', value);
		});

		make_empty_line();

		make_line('game_type', 'Type of Game', [
			['new', 'Random map'],
			//['load', 'Load game'],
		], 'new', (value) => {
			#print('TYPE OF GAME', value);
		});

		make_line('planet_size', 'Planet Size', [
			['68x34', 'Tiny planet'],
			['88x44', 'Small planet'],
			['112x56', 'Standard planet'],
			['140x70', 'Large planet'],
			['180x90', 'Huge planet'],
		], '112x56', (value) => {
			#print('PLANET SIZE', value);
		});

		make_line('ocean_coverage', 'Ocean Coverage', [
			['0.4', '30-50% of surface'],
			['0.6', '50-70% of surface'],
			['0.8', '70-90% of surface'],
		], '0.6', (value) => {
			#print('OCEAN COVERAGE', value);
		});

		make_line('erosive_forces', 'Erosive Forces', [
			['0.5', 'Strong'],
			['0.75', 'Average'],
			['1.0', 'Weak'],
		], '0.75', (value) => {
			#print('EROSIVE FORCES', value);
		});

		make_line('native_lifeforms', 'Native Lifeforms', [
			['0.25', 'Rare'],
			['0.5', 'Average'],
			['0.75', 'Abundant'],
		], '0.5', (value) => {
			#print('NATIVE LIFEFORMS', value);
		});

		make_line('cloud_cover', 'Cloud Cover', [
			['0.25', 'Sparse'],
			['0.5', 'Average'],
			['0.75', 'Dense'],
		], '0.5', (value) => {
			#print('CLOUD COVER', value);
		});
		// small hack until zindex is fully fixed: add lines in reverse order so that higher selects were drawn on top of lower selects
		let top = 23 * (#sizeof(lines) - 1) + 6;
		for (let idx = #sizeof(lines); idx > 0; idx--) {
			const line = lines[idx - 1];
			if (!#is_empty(line)) {
				body.text({
					class: 'popup-panel-text',
					top: top + 3,
					text: line.label + ':',
					left: 10,
				});
				const select = body.select({
					top: top,
					class: 'popup-list-select',
					width: 140,
					align: 'right',
					right: 10,
					items: line.choices,
					value: line.initial_choice,
				});
				select.on('select', (e) => {
					game.event('configure_game', {
						changes: [
							[line.key, e.value],
						],
					});
					line.on_change(e.value);
					return true;
				});
				line.on_change(line.initial_choice);
			}
			top -= 23;
		}
	});

};
