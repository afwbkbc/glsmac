#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let game = null;
	let attacker_id = 0;
	let spawn_tile = null;
	let native_unit_ids = [];
	let initial_relation = '';
	let setup_complete = false;
	let setup_started = false;
	let ui_started = false;
	let runtime_started = false;
	let start_runtime = null;
	let finished = false;

	const fail = (message) => {
		if (!finished) {
			finished = true;
			#print('NATIVE_CAPTURE_RUNTIME_FAIL: ' + message);
			glsmac.exit();
		}
	};

	const observe_capture = () => {
		if (finished || !setup_complete) {
			return;
		}
		const human = game.get_player();
		const native = game.get_native_player();
		if (human.get_diplomatic_relation(native) != initial_relation) {
			fail('capturing wild native life changed faction diplomacy');
			return;
		}
		if (!game.get_um().has_unit(attacker_id)) {
			fail('capturing unit disappeared');
			return;
		}
		let all_captured = true;
		for (id of native_unit_ids) {
			if (!game.get_um().has_unit(id)) {
				fail('captured stack member disappeared');
				return;
			}
			const unit = game.get_um().get_unit(id);
			if (unit.owner != human.id) {
				all_captured = false;
				break;
			}
			if (
				unit.movement != 0.0 || !unit.moved_this_turn ||
				unit.native_capture_attempted
			) {
				fail('captured native unit state is invalid');
				return;
			}
		}
		if (!all_captured) {
			#async(100, observe_capture);
			return;
		}
		const attacker = game.get_um().get_unit(attacker_id);
		if (attacker.get_tile() != spawn_tile) {
			#async(100, observe_capture);
			return;
		}
		if (attacker.movement != 0.0 || !attacker.moved_this_turn) {
			fail('capturing unit did not spend its attack movement');
			return;
		}
		finished = true;
		#print('NATIVE_CAPTURE_RUNTIME_PASS: positive PLANET captured a complete wild stack, preserved diplomacy, and advanced the attacker');
		#async(500, () => { glsmac.exit(); });
	};

	const start_if_ready = () => {
		if (ui_started && start_runtime != null && !runtime_started && !finished) {
			runtime_started = true;
			#async(500, () => { start_runtime(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		game = e.game;

		game.on('start_ui', (e) => {
			ui_started = true;
			start_if_ready();
		});

		game.on('turn', (e) => {
			if (setup_started || e.year - 2100 != 1) {
				return;
			}
			setup_started = true;
			const human = game.get_player();
			const native = game.get_native_player();
			if (human.get_faction().id != 'GAIANS') {
				fail('runtime scenario requires the Gaians');
				return;
			}
			let attacker = null;
			for (unit of game.get_um().get_units()) {
				if (
					unit.owner != human.id || !unit.is_land ||
					unit.get_def().offense <= 0 || unit.get_def().is_native
				) {
					continue;
				}
				for (candidate of unit.get_tile().get_surrounding_tiles()) {
					if (
						candidate.is_land && !candidate.is_locked() &&
						candidate.get_base() == null && #sizeof(candidate.get_units(true)) == 0
					) {
						attacker = unit;
						spawn_tile = candidate;
						break;
					}
				}
				if (spawn_tile != null) { break; }
			}
			if (attacker == null || spawn_tile == null) {
				fail('no deterministic adjacent capture setup is available');
				return;
			}
			attacker_id = attacker.id;
			attacker.movement = 1.0;
			attacker.moved_this_turn = false;
			initial_relation = human.get_diplomatic_relation(native);
			for (let i = 0; i < 2; i++) {
				const unit = game.get_um().spawn_unit({
					def: 'MindWorms',
					owner: native,
					tile: spawn_tile,
					morale: 1,
					health: 1.0,
					home_base_id: 0,
				});
				unit.movement = 0.0;
				unit.moved_this_turn = true;
				native_unit_ids :+unit.id;
			}
			setup_complete = true;
			start_runtime = () => {
				game.event('attack_unit', {
					attacker: game.get_um().get_unit(attacker_id),
					defender: game.get_um().get_unit(native_unit_ids[0]),
				});
				#async(100, observe_capture);
				#async(15000, () => { fail('native capture did not complete in time'); });
			};
			start_if_ready();
		});
	});

	glsmac.run();

});
