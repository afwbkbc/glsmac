#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let game = null;
	let target_id = 0;
	let target_initial_health = 0.0;
	let target_base = null;
	let target_owner_id = 0;
	let setup_complete = false;
	let combat_observed = false;
	let finished = false;

	const fail = (message) => {
		if (!finished) {
			finished = true;
			#print('NATIVE_LIFE_RUNTIME_FAIL: ' + message);
			glsmac.exit();
		}
	};

	const observe_combat = () => {
		if (finished || !setup_complete) {
			return;
		}
		const um = game.get_um();
		if (!um.has_unit(target_id)) {
			combat_observed = true;
		} else if (um.get_unit(target_id).health < target_initial_health) {
			combat_observed = true;
		}
		for (unit of um.get_units()) {
			if (
				unit.owner == game.get_native_player().id &&
				(unit.moved_this_turn || unit.health < 1.0)
			) {
				combat_observed = true;
			}
		}
		if (!combat_observed) {
			#async(100, observe_combat);
		}
	};

	glsmac.on('configure_game', (e) => {
		game = e.game;

		game.on('start_ui', (e) => {
			const human = game.get_player();
			const native = game.get_native_player();
			if (native.type != 'native' || native.get_faction().id != 'PLANET') {
				fail('Planet player identity is invalid');
				return;
			}
			let target = null;
			let spawn_tile = null;
			for (unit of game.get_um().get_units()) {
				if (
					unit.owner != human.id || !unit.is_land ||
					unit.get_def().offense <= 0 ||
					(#is_defined(unit.transport_id) && unit.transport_id > 0)
				) {
					continue;
				}
				for (candidate of unit.get_tile().get_surrounding_tiles()) {
					if (
						candidate.is_land && !candidate.is_locked() &&
						candidate.get_base() == null && #sizeof(candidate.get_units()) == 0
					) {
						target = unit;
						spawn_tile = candidate;
						break;
					}
				}
				if (spawn_tile != null) {
					break;
				}
			}
			if (target == null || spawn_tile == null) {
				fail('no deterministic adjacent native combat setup is available');
				return;
			}

			target_id = target.id;
			target_initial_health = target.health;
			target_base = target.get_tile().get_base();
			target_owner_id = human.id;
			game.event('spawn_unit', {
				owner: native,
				tile: spawn_tile,
				type: 'MindWorms',
				morale: 1,
				health: 1.0,
				home_base_id: 0,
			});
			setup_complete = true;
			#async(100, observe_combat);
			#async(500, () => {
				if (!game.is_turn_complete(human.id)) {
					game.event('complete_turn', {});
				}
			});
			#async(15000, () => { fail('native combat did not complete a turn in time'); });
		});

		game.on('turn', (e) => {
			if (!setup_complete || finished) {
				return;
			}
			if (!combat_observed) {
				fail('Planet completed its turn without acting on an adjacent hostile unit');
				return;
			}
			if (target_base != null && target_base.get_owner().id != target_owner_id) {
				fail('wild native combat transferred a base to Planet');
				return;
			}
			finished = true;
			#print('NATIVE_LIFE_RUNTIME_PASS: Planet attacked through synchronized events and advanced the turn without capturing a base');
			#async(500, () => { glsmac.exit(); });
		});
	});

	glsmac.run();

});
