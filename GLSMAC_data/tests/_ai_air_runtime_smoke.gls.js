#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let game = null;
	let aircraft_id = 0;
	let ai_id = 0 - 1;
	let home_tile = null;
	let staging_tile = null;
	let arrived_home = false;
	let setup_complete = false;
	let wait_ticks = 0;

	const fail = (message) => {
		#print('AI_AIR_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const complete_human_turn = () => {
		if (!game.is_game_over() && !game.is_turn_complete(game.get_player().id)) {
			game.event('complete_turn', {});
		}
	};

	const check_result = () => {
		wait_ticks++;
		if (aircraft_id == 0) {
			for (unit of staging_tile.get_units()) {
				if (unit.owner == ai_id && unit.get_def().chassis == 'Needlejet') {
					aircraft_id = unit.id;
					break;
				}
			}
			if (aircraft_id == 0) {
				if (wait_ticks >= 20) {
					fail('partially fueled AI aircraft was not spawned');
					return false;
				}
				return true;
			}
			setup_complete = true;
			complete_human_turn();
		}
		if (!game.get_um().has_unit(aircraft_id)) {
			fail('AI aircraft crashed instead of returning to refuel');
			return false;
		}
		const aircraft = game.get_um().get_unit(aircraft_id);
		if (!arrived_home && aircraft.get_tile() == home_tile) {
			arrived_home = true;
		}
		if (arrived_home && aircraft.fuel == aircraft.get_def().operational_range) {
			#print('AI_AIR_RUNTIME_PASS: partially fueled AI aircraft returned to base and refueled');
			#async(500, () => { glsmac.exit(); });
			return false;
		}
		if (wait_ticks >= 300) {
			fail(arrived_home ? 'AI aircraft did not refuel' : 'AI aircraft did not return to base');
			return false;
		}
		return true;
	};

	glsmac.on('configure_game', (e) => {
		game = e.game;

		game.on('start_ui', (e) => {
			let ai = null;
			for (player of game.get_players()) {
				if (player.type == 'ai') {
					ai = player;
					break;
				}
			}
			if (ai == null) {
				fail('computer player is missing');
				return;
			}
			ai_id = ai.id;
			let home_base = null;
			for (base of game.get_bm().get_bases()) {
				if (base.get_owner().id == ai_id) {
					home_base = base;
					break;
				}
			}
			if (home_base == null) {
				fail('computer player base is missing');
				return;
			}
			home_tile = home_base.get_tile();
			for (candidate of home_tile.get_surrounding_tiles()) {
				if (!candidate.is_locked() && candidate.get_base() == null && #sizeof(candidate.get_units()) == 0) {
					staging_tile = candidate;
					break;
				}
			}
			if (staging_tile == null) {
				fail('no adjacent aircraft staging tile is available');
				return;
			}
			let needlejet = null;
			for (def of game.get_um().get_unit_defs()) {
				if (def.chassis == 'Needlejet' && def.offense > 0) {
					needlejet = def;
					break;
				}
			}
			if (needlejet == null || needlejet.operational_range != 2) {
				fail('Needlejet definition is missing');
				return;
			}
			game.event('spawn_unit', {
				owner: ai,
				tile: staging_tile,
				type: needlejet.id,
				morale: 2,
				health: 1.0,
				fuel: 1,
			});
			#async(100, check_result);
		});

		game.on('turn', (e) => {
			if (setup_complete) {
				#async(100, complete_human_turn);
			}
		});
	});

	glsmac.run();

});
