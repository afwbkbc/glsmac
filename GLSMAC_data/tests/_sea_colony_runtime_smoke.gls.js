#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	const technologies = #include('../default/technologies');
	let ui_started = false;
	let runtime_started = false;
	let runtime_complete = false;
	let exit_scheduled = false;
	let start_runtime = null;

	const fail = (message) => {
		#print('SEA_COLONY_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (ui_started && runtime_complete && !exit_scheduled) {
			exit_scheduled = true;
			#print(
				'SEA_COLONY_RUNTIME_PASS: validated coastal production, naval docking, sea founding, and initial naval production'
			);
			#async(500, () => { glsmac.exit(); });
		}
	};

	const start_if_ready = () => {
		if (ui_started && start_runtime != null && !runtime_started) {
			runtime_started = true;
			#async(500, () => { start_runtime(); });
		}
	};

	const wait_for_arrival = (unit, tile, on_ready, attempts) => {
		if (unit.get_tile() == tile && !tile.is_locked()) {
			on_ready();
			return;
		}
		if (attempts >= 100) {
			fail('timed out waiting for naval docking');
			return;
		}
		#async(50, () => { wait_for_arrival(unit, tile, on_ready, attempts + 1); });
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('start_ui', (e) => {
			ui_started = true;
			start_if_ready();
			finish_if_ready();
		});

		game.on('turn', (e) => {
			if (e.year - 2100 != 1) {
				fail('runtime test exceeded one turn');
				return;
			}

			const player = game.get_player();
			let all_technologies = [];
			for (technology_id of technologies.order) {
				all_technologies :+technology_id;
			}
			player.set_research_state({
				technologies: all_technologies,
				target: '',
				progress: 0,
			});

			let sea_colony_def = null;
			let naval_def = null;
			for (def of game.get_um().get_unit_defs()) {
				if (
					def.is_water && def.can_found_base && def.chassis == 'Foil' &&
					(sea_colony_def == null || def.mineral_cost < sea_colony_def.mineral_cost)
				) {
					sea_colony_def = def;
				}
				if (
					def.is_water && !def.is_native && !def.is_missile &&
					def.offense > 0 && !def.can_found_base && !def.can_terraform &&
					def.cargo_capacity == 0 &&
					(naval_def == null || def.mineral_cost < naval_def.mineral_cost)
				) {
					naval_def = def;
				}
			}
			if (sea_colony_def == null || naval_def == null) {
				fail('generated sea colony or conventional naval definition is missing');
				return;
			}

			const tm = game.get_tm();
			let port_tile = null;
			let dock_water = null;
			for (let y = 0; y < tm.get_map_height() && port_tile == null; y++) {
				for (let x = 0; x < tm.get_map_width() && port_tile == null; x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const candidate = tm.get_tile(x, y);
					if (
						!candidate.is_land || candidate.get_base() != null ||
						#sizeof(candidate.get_units()) > 0
					) {
						continue;
					}
					for (nearby of candidate.get_surrounding_tiles()) {
						if (
							nearby.is_water && nearby.get_base() == null &&
							#sizeof(nearby.get_units()) == 0
						) {
							port_tile = candidate;
							dock_water = nearby;
							break;
						}
					}
				}
			}
			if (port_tile == null) {
				fail('could not find an empty coastal port tile');
				return;
			}

			const port = game.get_bm().spawn_base(player, port_tile, {
				name: 'Runtime Test Port',
				production: 'ScoutPatrol',
			});
			if (
				!port.can_set_production('unit', sea_colony_def.id) ||
				!port.can_set_production('unit', naval_def.id)
			) {
				fail('coastal land base cannot produce naval units');
				return;
			}

			let sea_site = null;
			for (let sea_y = 0; sea_y < tm.get_map_height() && sea_site == null; sea_y++) {
				for (let sea_x = 0; sea_x < tm.get_map_width() && sea_site == null; sea_x++) {
					if (sea_x % 2 != sea_y % 2) {
						continue;
					}
					const sea_candidate = tm.get_tile(sea_x, sea_y);
					if (
						!sea_candidate.is_water || sea_candidate.get_base() != null ||
						#sizeof(sea_candidate.get_units()) > 0
					) {
						continue;
					}
					let valid = true;
					for (nearby of sea_candidate.get_surrounding_tiles()) {
						if (nearby.get_base() != null) {
							valid = false;
							break;
						}
					}
					if (valid) {
						sea_site = sea_candidate;
					}
				}
			}
			if (sea_site == null) {
				fail('could not find an open ocean base site');
				return;
			}

			const naval_unit = game.get_um().spawn_unit({
				def: naval_def.id,
				owner: player,
				tile: dock_water,
				morale: 2,
				health: 1.0,
			});
			const colony = game.get_um().spawn_unit({
				def: sea_colony_def.id,
				owner: player,
				tile: sea_site,
				morale: 2,
				health: 1.0,
			});
			const colony_id = colony.id;

			start_runtime = () => {
				game.event('move_unit', {unit: naval_unit, tile: port_tile});
				wait_for_arrival(naval_unit, port_tile, () => {
					game.event('move_unit', {unit: naval_unit, tile: dock_water});
					wait_for_arrival(naval_unit, dock_water, () => {
						game.event('found_base', {
							unit: colony,
							name: 'Runtime Ocean Base',
						});
						#async(500, () => {
							const sea_base = sea_site.get_base();
							const production = sea_base == null ? null : sea_base.get_production();
							if (
								sea_base == null || sea_base.name != 'Runtime Ocean Base' ||
								sea_base.get_owner().id != player.id ||
								game.get_um().has_unit(colony_id) || production == null ||
								!production.is_water || production.is_native ||
								production.can_found_base ||
								!sea_base.can_set_production('unit', sea_colony_def.id)
							) {
								fail('live sea-base state or initial production is invalid');
								return;
							}
							runtime_complete = true;
							finish_if_ready();
						});
					}, 0);
				}, 0);
			};
			start_if_ready();
		});
	});

	glsmac.run();

});
