return (glsmac) => {

	// TODO: refactor these into modules
	const factions = #include('../factions');
	const facilities = #include('../facilities');
	const resources = #include('../resources');
	const technologies = #include('../technologies');
	const units = #include('../units');
	const content_validator = #include('../content/validator');

	let content_validation_complete = false;
	const validate_content = () => {
		if (content_validation_complete) {
			return;
		}
		const result = content_validator.validate({
			technologies: {
				definitions: technologies.definitions,
				order: technologies.order,
			},
			facilities: facilities.definitions,
			facility_manifest: facilities.manifest,
			facility_coverage: facilities.coverage,
			project_coverage: facilities.project_coverage,
			units: units.definitions,
			unit_manifest: units.manifest,
			moralesets: units.moralesets,
			factions: factions.definitions,
		});
		if (#sizeof(result.errors) > 0) {
			for (error of result.errors) {
				#print('CONTENT_VALIDATION_FAIL: ' + error);
			}
			throw Error(
				'Content validation failed with ' + #to_string(#sizeof(result.errors)) +
				' error(s): ' + result.errors[0]
			);
		}
		#print(
			'CONTENT_VALIDATION_PASS: technologies=' + #to_string(result.counts.technologies) +
			' facilities=' + #to_string(result.counts.facilities) + '/' +
				#to_string(result.counts.base_facilities) +
				' (' + #to_string(result.counts.complete_facilities) + ' complete,' +
				#to_string(result.counts.partial_facilities) + ' partial)' +
			' projects=' + #to_string(result.counts.implemented_projects) + '/' +
				#to_string(result.counts.projects) +
				' (' + #to_string(result.counts.complete_projects) + ' complete,' +
				#to_string(result.counts.partial_projects) + ' partial)' +
			' units=' + #to_string(result.counts.units) +
			' predefined_units=' + #to_string(result.counts.predefined_units) +
			' components=' + #to_string(
				result.counts.chassis + result.counts.reactors + result.counts.weapons +
				result.counts.armors + result.counts.abilities
			) +
			' moralesets=' + #to_string(result.counts.moralesets) +
			' factions=' + #to_string(result.counts.factions)
		);
		content_validation_complete = true;
	};

	const modules = [
		'social_engineering', 'diplomacy', 'projects', 'unit_upgrades', 'orbitals', 'territory', 'bases', 'ecology', 'council', 'probes', 'conquest', 'transcendence', 'economic_victory',
		'economy', 'native_ai', 'ai',
	];
	let m = {};
	for (module of modules) {
		m[module] = #include(module);
	}

	glsmac.on('configure_state', (e) => {
		validate_content();
		factions.configure(e.fm);
	});

	#include('events')(glsmac.game);

	glsmac.on('configure_game', (e) => {
		validate_content();

		const game = e.game;

		game.on('configure', (e) => {

			for (module of m) {
				module(game);
			}

			const um = game.get_um();

			um.on('unit_spawn', (e) => {
				//
			});

			um.on('unit_despawn', (e) => {
				//
			});

			units.configure(game);
			resources.configure(game);
			technologies.configure(game);

			resources.define(game);
			units.define(game);
			facilities.define(game);

			const worldscript = #is_defined(glsmac.config.worldscript) ? glsmac.config.worldscript : 'default';
			game.on('create_world', (e) => {
				const generator = #include('world/' + worldscript);
				generator(e.game);
			});

			game.on('error', (e) => {
				glsmac.ui.error('Game initialization failed: ' + e.error, () => {
					if (
						#is_defined(glsmac.config.quickstart) ||
						#is_defined(glsmac.config.host) ||
						#is_defined(glsmac.config.join)
					) {
						glsmac.exit();
					} else {
						glsmac.reset();
					}
				});
			});

		});

	});

};
