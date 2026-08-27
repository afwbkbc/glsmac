#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	const MINIMUM_TURN = 20;
	const FINAL_TURN = 30;
	let game = null;
	let ai_id = 0 - 1;
	let ui_started = false;
	let exit_scheduled = false;
	let max_base_size = 0;
	let saw_former = false;
	let saw_improvement = false;
	let saw_facility = false;
	let success_wait_ticks = 0;

	const fail = (message) => {
		#print('AI_ECONOMY_SOAK_FAIL: ' + message);
		glsmac.exit();
	};

	const complete_human_turn = () => {
		if (!game.is_game_over() && !game.is_turn_complete(game.get_player().id)) {
			game.event('complete_turn', {});
		}
	};

	const finish_success = () => {
		success_wait_ticks++;
		let map_locked = false;
		const tm = game.get_tm();
		for (let y = 0; y < tm.get_map_height() && !map_locked; y++) {
			for (let x = 0; x < tm.get_map_width(); x++) {
				if (x % 2 == y % 2 && tm.get_tile(x, y).is_locked()) {
					map_locked = true;
					break;
				}
			}
		}
		if (!game.is_turn_complete(ai_id) || map_locked) {
			if (success_wait_ticks >= 50) {
				fail('final AI turn or map animation did not settle before exit');
				return false;
			}
			return true;
		}
		#print('AI_ECONOMY_SOAK_PASS: AI sustained growth, research, infrastructure, defense, and terraforming');
		#async(2000, () => { glsmac.exit(); });
		return false;
	};

	const get_snapshot = (ai) => {
		let bases = 0;
		let population = 0;
		let facilities = 0;
		let queued_bases = 0;
		let garrisoned_bases = 0;
		let rioting_bases = 0;
		for (base of game.get_bm().get_bases()) {
			if (base.get_owner().id != ai.id) {
				continue;
			}
			bases++;
			population += base.get_size();
			max_base_size = #max(max_base_size, base.get_size());
			const base_facilities = #sizeof(base.get_facilities());
			facilities += base_facilities;
			if (base_facilities > 0) {
				saw_facility = true;
			}
			if (#sizeof(base.get_production_queue()) > 0) {
				queued_bases++;
			}
			for (unit of base.get_tile().get_units()) {
				if (unit.owner == ai.id && unit.get_def().offense > 0) {
					garrisoned_bases++;
					break;
				}
			}
			if (game.get('f_base_get_psych')(base).is_rioting) {
				rioting_bases++;
			}
			for (tile of base.get_workable_tiles()) {
				if (
					tile.terraforming.road ||
					tile.terraforming.farm ||
					tile.terraforming.mine ||
					tile.terraforming.solar ||
					tile.terraforming.forest
				) {
					saw_improvement = true;
				}
			}
		}
		let formers = 0;
		let colonies = 0;
		let combat = 0;
		for (unit of game.get_um().get_units()) {
			if (unit.owner != ai.id) {
				continue;
			}
			const def = unit.get_def();
			if (def.can_terraform) {
				formers++;
				saw_former = true;
			}
			if (def.can_found_base) {
				colonies++;
			}
			if (def.offense > 0) {
				combat++;
			}
		}
		return {
			bases: bases,
			population: population,
			facilities: facilities,
			queued_bases: queued_bases,
			garrisoned_bases: garrisoned_bases,
			rioting_bases: rioting_bases,
			formers: formers,
			colonies: colonies,
			combat: combat,
			technologies: #sizeof(ai.get_research_state().technologies),
			credits: ai.energy_credits,
		};
	};

	const print_snapshot = (turn, snapshot) => {
		#print(
			'AI_ECONOMY_SOAK_TRACE: turn=' + #to_string(turn) +
			' bases=' + #to_string(snapshot.bases) +
			' pop=' + #to_string(snapshot.population) +
			' facilities=' + #to_string(snapshot.facilities) +
			' formers=' + #to_string(snapshot.formers) +
			' colonies=' + #to_string(snapshot.colonies) +
			' combat=' + #to_string(snapshot.combat) +
			' garrisons=' + #to_string(snapshot.garrisoned_bases) +
			' techs=' + #to_string(snapshot.technologies) +
			' credits=' + #to_string(snapshot.credits)
		);
	};

	glsmac.on('configure_game', (e) => {
		game = e.game;

		game.on('start_ui', (e) => {
			ui_started = true;
			#async(250, complete_human_turn);
		});

		game.on('turn', (e) => {
			if (!ui_started || exit_scheduled) {
				return;
			}
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
			const turn = e.year - 2100;
			const snapshot = get_snapshot(ai);
			if (turn == 1 || turn % 5 == 0 || turn >= FINAL_TURN) {
				print_snapshot(turn, snapshot);
			}
			if (turn >= MINIMUM_TURN) {
				let incomplete = null;
				if (snapshot.bases <= 0 || snapshot.population < snapshot.bases || max_base_size < 2) {
					incomplete = 'AI did not sustain populated, growing bases';
				} else if (!saw_former || !saw_improvement) {
					incomplete = 'AI did not produce a former and improve workable terrain';
				} else if (!saw_facility) {
					incomplete = 'AI did not complete infrastructure during normal play';
				} else if (snapshot.garrisoned_bases < snapshot.bases - 1) {
					incomplete = 'AI left more than one frontier base without a garrison';
				} else if (snapshot.queued_bases != snapshot.bases) {
					incomplete = 'AI left one or more bases without production';
				} else if (snapshot.rioting_bases > 0) {
					incomplete = 'AI ended the soak with rioting bases';
				} else if (snapshot.technologies < 2) {
					incomplete = 'AI did not complete enough research milestones';
				} else if (snapshot.credits < 0) {
					incomplete = 'AI treasury became insolvent';
				}
				if (incomplete == null) {
					exit_scheduled = true;
					#async(100, finish_success);
					return;
				}
				if (turn >= FINAL_TURN) {
					fail(incomplete);
					return;
				}
			}
			#async(250, complete_human_turn);
		});
	});

	glsmac.run();

});
