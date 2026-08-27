#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;

	const fail = (message) => {
		#print('PLANETARY_DATALINKS_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print(
				'PLANETARY_DATALINKS_RUNTIME_PASS: copied a technology known by exactly three other factions'
			);
			#async(2500, () => { glsmac.exit(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('start_ui', (e) => {
			ui_started = true;
			finish_if_ready();
		});

		game.on('turn', (e) => {
			if (e.year - 2100 != 1) {
				return;
			}
			const players = game.get_players();
			if (#sizeof(players) != 4) {
				fail('quickstart did not create four factions');
				return;
			}
			const owner = game.get_player();
			let project_base = null;
			for (base of game.get_bm().get_bases()) {
				if (base.get_owner().id == owner.id) {
					project_base = base;
					break;
				}
			}
			if (project_base == null) {
				fail('project owner has no base');
				return;
			}
			project_base.add_facility('ThePlanetaryDatalinks');
			owner.set_research_state({technologies: [], target: '', progress: 0});
			let rivals = [];
			for (player of players) {
				if (player.id != owner.id) {
					rivals :+player;
				}
			}
			rivals[0].set_research_state({
				technologies: ['Biogenetics'], target: '', progress: 0,
			});
			rivals[1].set_research_state({
				technologies: ['Biogenetics'], target: '', progress: 0,
			});
			rivals[2].set_research_state({technologies: [], target: '', progress: 0});

			const queue_datalinks = game.get('f_project_queue_planetary_datalinks');
			if (queue_datalinks()) {
				fail('two other factions incorrectly satisfied the Datalinks threshold');
				return;
			}
			rivals[2].set_research_state({
				technologies: ['Biogenetics'], target: '', progress: 0,
			});
			if (!queue_datalinks() || queue_datalinks()) {
				fail('Datalinks event queueing was not unique at the three-faction threshold');
				return;
			}

			let wait_ticks = 0;
			#async(100, () => {
				wait_ticks++;
				if (!owner.has_technology('Biogenetics')) {
					if (wait_ticks >= 100) {
						fail('Datalinks technology grant timed out');
						return false;
					}
					return true;
				}
				const state = owner.get_research_state();
				if (
					state.technologies != ['Biogenetics'] ||
					state.target != '' || state.progress != 0
				) {
					fail('Datalinks grant produced an invalid research state');
					return false;
				}
				runtime_complete = true;
				finish_if_ready();
				return false;
			});
		});
	});

	glsmac.run();

});
