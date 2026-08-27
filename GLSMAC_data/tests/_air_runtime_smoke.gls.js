#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let countdown_id = 0;
	let immediate_crash_id = 0;
	let copter_id = 0;
	let gravship_id = 0;
	let base_refuel_id = 0;
	let airbase_refuel_id = 0;
	let missile_crash_id = 0;
	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;

	const fail = (message) => {
		#print('AIR_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print('AIR_RUNTIME_PASS: validated range, crashes, field damage, airbase refueling, and unlimited flight');
			#async(500, () => { glsmac.exit(); });
		}
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		const find_def = (chassis) => {
			for (def of game.get_um().get_unit_defs()) {
				if (def.chassis == chassis && def.offense > 0) {
					return def;
				}
			}
			return null;
		};

		const find_empty_land_tiles = (count) => {
			let result = [];
			const tm = game.get_tm();
			for (let y = 0; y < tm.get_map_height() && #sizeof(result) < count; y++) {
				for (let x = 0; x < tm.get_map_width() && #sizeof(result) < count; x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const tile = tm.get_tile(x, y);
					if (
						!tile.is_land || tile.get_base() != null || tile.is_locked() ||
						#sizeof(tile.get_units()) > 0
					) {
						continue;
					}
					result :+tile;
				}
			}
			return result;
		};

		game.on('start_ui', (e) => {
			ui_started = true;
			finish_if_ready();
		});

		game.on('turn', (e) => {
			const turn_id = e.year - 2100;
			const um = game.get_um();
			const player = game.get_player();
			if (turn_id == 1) {
				const needlejet = find_def('Needlejet');
				const copter = find_def('Copter');
				const gravship = find_def('Gravship');
				const missile = find_def('Missile');
				if (needlejet == null || copter == null || gravship == null || missile == null) {
					fail('generated air chassis definitions are missing');
					return;
				}
				if (
					needlejet.operational_range != 2 || needlejet.is_missile ||
					copter.operational_range != 1 || copter.is_missile ||
					gravship.operational_range != 0 || gravship.is_missile ||
					missile.operational_range != 1 || !missile.is_missile
				) {
					fail('native air definition metadata is invalid');
					return;
				}
				const field_tiles = find_empty_land_tiles(2);
				if (#sizeof(field_tiles) != 2) {
					fail('could not find air lifecycle test tiles');
					return;
				}
				field_tiles[1].update_terraforming({airbase: true});
				let base = null;
				for (candidate of game.get_bm().get_bases()) {
					if (candidate.get_owner().id == player.id) {
						base = candidate;
						break;
					}
				}
				if (base == null) {
					fail('player base is missing');
					return;
				}
				const spawn = (def, tile, fuel) => {
					let data = {
						def: def.id,
						owner: player,
						tile: tile,
						morale: 2,
						health: 1.0,
					};
					if (fuel != null) {
						data.fuel = fuel;
					}
					return um.spawn_unit(data);
				};
				const countdown = spawn(needlejet, field_tiles[0], null);
				const immediate_crash = spawn(needlejet, field_tiles[0], 1);
				const copter_unit = spawn(copter, field_tiles[0], 1);
				const gravship_unit = spawn(gravship, field_tiles[0], 0);
				const base_refuel = spawn(needlejet, base.get_tile(), 0);
				const airbase_refuel = spawn(needlejet, field_tiles[1], 0);
				const missile_crash = spawn(missile, field_tiles[0], 1);
				countdown_id = countdown.id;
				immediate_crash_id = immediate_crash.id;
				copter_id = copter_unit.id;
				gravship_id = gravship_unit.id;
				base_refuel_id = base_refuel.id;
				airbase_refuel_id = airbase_refuel.id;
				missile_crash_id = missile_crash.id;
				if (countdown.fuel != 2) {
					fail('new Needlejet did not start with full fuel');
					return;
				}
				game.event('complete_turn', {});
				return;
			}

			if (turn_id == 2) {
				let wait_ticks = 0;
				#async(100, () => {
					wait_ticks++;
					if (um.has_unit(immediate_crash_id) || um.has_unit(missile_crash_id)) {
						if (wait_ticks >= 20) {
							fail('Needlejet or missile crash event timed out');
							return false;
						}
						return true;
					}
					if (
						!um.has_unit(countdown_id) || um.get_unit(countdown_id).fuel != 1
					) {
						fail('Needlejet countdown or crash timing is invalid');
						return false;
					}
					const copter = um.get_unit(copter_id);
					if (copter.fuel != 0 || copter.health < 0.699 || copter.health > 0.701) {
						fail('Copter field landing damage is invalid');
						return false;
					}
					if (
						!um.has_unit(gravship_id) || um.get_unit(gravship_id).fuel != 0 ||
						um.get_unit(base_refuel_id).fuel != 2 ||
						um.get_unit(airbase_refuel_id).fuel != 2
					) {
						fail('air refueling or Gravship range is invalid');
						return false;
					}
					game.event('complete_turn', {});
					return false;
				});
				return;
			}

			if (turn_id == 3) {
				let wait_ticks = 0;
				#async(100, () => {
					if (um.has_unit(countdown_id)) {
						wait_ticks++;
						if (wait_ticks >= 20) {
							fail('Needlejet survived beyond its operational range');
							return false;
						}
						return true;
					}
					const copter = um.get_unit(copter_id);
					if (copter.health < 0.399 || copter.health > 0.401) {
						fail('Copter did not take repeated field damage');
						return false;
					}
					if (!um.has_unit(gravship_id) || um.get_unit(gravship_id).health != 1.0) {
						fail('Gravship was incorrectly range-limited');
						return false;
					}
					runtime_complete = true;
					finish_if_ready();
					return false;
				});
				return;
			}

			fail('runtime test exceeded three turns');
		});
	});

	glsmac.run();

});
