return (i) => {

	const glsmac = i.glsmac;

	#print('X', glsmac);

	return i.make_section(glsmac.config.gamename, {
		width: 296,
		height: 358,
	}, (body) => {

		let lines = [];
		const make_line = (label, choices, on_change) => {
			lines :+[label, choices, on_change];
		};
		const make_empty_line = () => {
			lines :+[];
		};

		make_line('Difficulty Level', [
			// TODO
			/*['0', 'Citizen'],
			['1', 'Specialist'],
			['2', 'Talent'],
			['3', 'Librarian'],
			['4', 'Thinker'],*/
			['5', 'Transcend'],
		], (value) => {
			#print('DIFFICULTY:', value);
		});

		make_line('Time Controls', [
			['none', 'None'],
			// TODO
			/*['5', '5 min'],
			['15', '15 min'],*/
		], (value) => {
			#print('TIME CONTROLS:', value);
		});

		make_empty_line();

		make_line('Type of Game', [
			['new', 'Random map'],
			//['load', 'Load game'],
		], (value) => {
			#print('TYPE OF GAME', value);
		});

		make_line('Planet Size', [
			['68x34', 'Tiny planet'],
			['88x44', 'Small planet'],
			['112x56', 'Standard planet'],
			['140x70', 'Large planet'],
			['180x90', 'Huge planet'],
		], (value) => {
			#print('PLANET SIZE', value);
		});

		make_line('Ocean Coverage', [
			['0.4', '30-50% of surface'],
			['0.6', '50-70% of surface'],
			['0.8', '70-90% of surface'],
		], (value) => {
			#print('OCEAN COVERAGE', value);
		});

		make_line('Erosive Forces', [
			['0.5', 'Strong'],
			['0.75', 'Average'],
			['1.0', 'Weak'],
		], (value) => {
			#print('EROSIVE FORCES', value);
		});

		make_line('Native Lifeforms', [
			['0.25', 'Rare'],
			['0.5', 'Average'],
			['0.75', 'Abundant'],
		], (value) => {
			#print('NATIVE LIFEFORMS', value);
		});

		make_line('Cloud Cover', [
			['0.25', 'Sparse'],
			['0.5', 'Average'],
			['0.75', 'Dense'],
		], (value) => {
			#print('CLOUD COVER', value);
		});
		// small hack until zindex is fully fixed: add lines in reverse order so that higher selects were drawn on top of lower selects
		let top = 23 * (#sizeof(lines) - 1) + 6;
		for (i = #sizeof(lines); i > 0; i--) {
			const line = lines[i - 1];
			if (#sizeof(line) > 0) {
				body.text({
					class: 'popup-panel-text',
					top: top + 3,
					text: line[0] + ':',
					left: 10,
				});
				const select = body.select({
					top: top,
					class: 'popup-list-select',
					width: 140,
					align: 'right',
					right: 10,
					items: line[1],
				});
				select.on('select', (e) => {
					line[2](e.value);
					return true;
				});
			}
			top -= 23;
		}
	});

};
