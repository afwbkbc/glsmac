#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	const combat_rules = #include('../default/game/combat_rules');
	const attack_unit = #include('../default/game/event/attack_unit');
	const advance_unit_after_combat = #include('../default/game/event/advance_unit_after_combat');
	const entity_snapshots = #include('../default/game/entity_snapshots');
	let runtime_complete = false;
	let ui_started = false;
	let exit_scheduled = false;

	const fail = (message) => {
		#print('COMBAT_ACCESS_RUNTIME_FAIL: ' + message);
		glsmac.exit();
	};

	const finish_if_ready = () => {
		if (runtime_complete && ui_started && !exit_scheduled) {
			exit_scheduled = true;
			#print(
				'COMBAT_ACCESS_RUNTIME_PASS: validated generated amphibious and SAM designs, ' +
				'transport assault state, airborne targeting, and air-to-air strength'
			);
			#async(500, () => { glsmac.exit(); });
		}
	};

	const has_ability = (def, id) => {
		for (ability of def.abilities) {
			if (ability == id) {
				return true;
			}
		}
		return false;
	};

	const is_clear = (tile) => {
		return tile.get_base() == null && !tile.is_locked() && #sizeof(tile.get_units(true)) == 0;
	};

	const is_flat_clear_land = (tile) => {
		return (
			tile.is_land && is_clear(tile) && tile.rockiness < 3 &&
			!tile.features.xenofungus && !tile.terraforming.bunker &&
			!tile.terraforming.airbase
		);
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;

		game.on('start_ui', (e) => {
			ui_started = true;
			finish_if_ready();
		});

		game.on('turn', (e) => {
			if (e.year - 2100 != 1) {
				fail('runtime test exceeded one turn');
				return;
			}

			const um = game.get_um();
			const tm = game.get_tm();
			const player = game.get_player();
			const opponent = game.get_native_player();
			if (opponent == null) {
				fail('opponent player is missing');
				return;
			}

			let transport_def = null;
			let amphibious_def = null;
			let land_sam_def = null;
			let sea_sam_def = null;
			let air_sam_def = null;
			let bomber_def = null;
			for (def of um.get_unit_defs()) {
				if (def.chassis == 'Foil' && def.weapon == 'TroopTransport') {
					transport_def = def;
				}
				if (def.offense > 0 && has_ability(def, 'AmphibiousPods')) {
					if (!def.is_land) {
						fail(
							'Amphibious Pods were generated for non-land unit ' + def.id +
							' chassis=' + def.chassis
						);
						return;
					}
					if (amphibious_def == null) {
						amphibious_def = def;
					}
				}
				if (def.offense > 0 && has_ability(def, 'AirSuperiority')) {
					if (def.is_land && land_sam_def == null) {
						land_sam_def = def;
					} else if (def.is_water && sea_sam_def == null) {
						sea_sam_def = def;
					} else if (def.is_air && air_sam_def == null) {
						air_sam_def = def;
					}
				}
				if (
					def.chassis == 'Needlejet' && def.offense > 0 &&
					!has_ability(def, 'AirSuperiority') && bomber_def == null
				) {
					bomber_def = def;
				}
			}
			if (
				transport_def == null || amphibious_def == null || land_sam_def == null ||
				sea_sam_def == null || air_sam_def == null || bomber_def == null
			) {
				fail('generated combat-access unit definitions are incomplete');
				return;
			}

			let coast_water = null;
			let coast_land = null;
			let air_source = null;
			let air_target = null;
			for (let y = 0; y < tm.get_map_height(); y++) {
				for (let x = 0; x < tm.get_map_width(); x++) {
					if (x % 2 != y % 2) {
						continue;
					}
					const tile = tm.get_tile(x, y);
					if (coast_water == null && tile.is_water && is_clear(tile)) {
						for (nearby of tile.get_surrounding_tiles()) {
							if (nearby != tile && is_flat_clear_land(nearby)) {
								coast_water = tile;
								coast_land = nearby;
								break;
							}
						}
					}
					if (air_source == null && is_flat_clear_land(tile)) {
						for (nearby of tile.get_surrounding_tiles()) {
							if (nearby != tile && is_flat_clear_land(nearby)) {
								air_source = tile;
								air_target = nearby;
								break;
							}
						}
					}
				}
			}
			if (coast_water == null || air_source == null) {
				fail('could not find clear coastal and air-combat test tiles');
				return;
			}

			const spawn = (def, owner, tile, transport_id) => {
				let data = {
					def: def,
					owner: owner,
					tile: tile,
					morale: 2,
					health: 1.0,
				};
				if (transport_id > 0) {
					data.transport_id = transport_id;
				}
				return um.spawn_unit(data);
			};

			const carrier = spawn(transport_def.id, player, coast_water, 0);
				const ordinary_cargo = spawn('ScoutPatrol', player, coast_water, carrier.id);
				const amphibious = spawn(amphibious_def.id, player, coast_water, carrier.id);
				const unit_snapshot = entity_snapshots.snapshot_unit(ordinary_cargo);
				ordinary_cargo.disembark();
				if (unit_snapshot.transport_id != carrier.id || ordinary_cargo.transport_id != 0) {
					fail('native unit snapshot retained mutable wrapper values');
					return;
				}
				ordinary_cargo.embark(carrier);
				const coastal_defender = spawn('ScoutPatrol', opponent, coast_land, 0);
				const validation_game = {
					is_turn_complete: (player_id) => { return false; },
					tm: tm,
				};
				if (
					attack_unit.validate({
						caller: player.id,
						game: validation_game,
						data: {attacker: ordinary_cargo, defender: coastal_defender},
					}) != 'Only units with Amphibious Pods can attack from a transport'
				) {
					fail('ordinary embarked unit was allowed to attack');
					return;
				}
				if (#is_defined(attack_unit.validate({
					caller: player.id,
					game: validation_game,
					data: {attacker: amphibious, defender: coastal_defender},
				}))) {
					fail('amphibious embarked attack was rejected');
					return;
				}

				const coastal_sam = spawn(land_sam_def.id, player, coast_land, 0);
				const bomber_over_water = spawn(bomber_def.id, opponent, coast_water, 0);
				if (#is_defined(attack_unit.validate({
					caller: player.id,
					game: validation_game,
					data: {attacker: coastal_sam, defender: bomber_over_water},
				}))) {
					fail('land SAM could not attack an aircraft over water');
					return;
				}
				um.despawn_unit(coastal_sam);
				const sea_sam = spawn(sea_sam_def.id, player, coast_water, 0);
				const bomber_over_land = spawn(bomber_def.id, opponent, coast_land, 0);
				if (#is_defined(attack_unit.validate({
					caller: player.id,
					game: validation_game,
					data: {attacker: sea_sam, defender: bomber_over_land},
				}))) {
					fail('naval SAM could not attack an aircraft over land');
					return;
				}

				const ordinary = spawn('ScoutPatrol', player, air_source, 0);
				const land_sam = spawn(land_sam_def.id, player, air_source, 0);
				const bomber = spawn(bomber_def.id, opponent, air_target, 0);
				if (
					attack_unit.validate({
						caller: player.id,
						game: validation_game,
						data: {attacker: ordinary, defender: bomber},
					}) != 'Only units with Air Superiority can attack air units in flight'
				) {
					fail('ordinary unit was allowed to attack an aircraft in flight');
					return;
				}
				if (#is_defined(attack_unit.validate({
					caller: player.id,
					game: validation_game,
					data: {attacker: land_sam, defender: bomber},
				}))) {
					fail('land SAM attack against aircraft in flight was rejected');
					return;
				}

				const air_powers = combat_rules.get_combat_powers(land_sam, bomber);
				const expected_attack = #to_float(land_sam_def.offense) *
					combat_rules.get_morale_multiplier(land_sam) * land_sam.health;
				const expected_defence = #to_float(bomber_def.defense) *
					combat_rules.get_morale_multiplier(bomber) * bomber.health;
				if (
					air_powers.attack < expected_attack - 0.001 ||
					air_powers.attack > expected_attack + 0.001 ||
					air_powers.defence < expected_defence - 0.001 ||
					air_powers.defence > expected_defence + 0.001
				) {
					fail('surface SAM combat did not use ordinary weapon and armor strengths');
					return;
				}

				const interceptor = spawn(air_sam_def.id, player, air_source, 0);
				const interceptor_powers = combat_rules.get_combat_powers(interceptor, bomber);
				const expected_interceptor_attack = #to_float(air_sam_def.offense) *
					combat_rules.get_morale_multiplier(interceptor) * interceptor.health * 2.0;
				const expected_interceptor_defence = #to_float(bomber_def.offense) *
					combat_rules.get_morale_multiplier(bomber) * bomber.health;
				if (
					interceptor_powers.attack < expected_interceptor_attack - 0.001 ||
					interceptor_powers.attack > expected_interceptor_attack + 0.001 ||
					interceptor_powers.defence < expected_interceptor_defence - 0.001 ||
					interceptor_powers.defence > expected_interceptor_defence + 0.001
				) {
					fail('air-to-air combat did not use interceptor bonus and defender offense');
					return;
				}
				const surface_defender = spawn('ScoutPatrol', opponent, air_target, 0);
				const surface_powers = combat_rules.get_combat_powers(
					interceptor,
					surface_defender
				);
				const expected_surface_attack = #to_float(air_sam_def.offense) *
					combat_rules.get_morale_multiplier(interceptor) * interceptor.health * 0.5;
				if (
					surface_powers.attack < expected_surface_attack - 0.001 ||
					surface_powers.attack > expected_surface_attack + 0.001
				) {
					fail('air interceptor surface-attack penalty is invalid');
					return;
				}

				um.despawn_unit(bomber_over_land);
				um.despawn_unit(coastal_defender);
				um.despawn_unit(bomber);
				um.despawn_unit(surface_defender);
				const saved_transport_id =
					advance_unit_after_combat.snapshot_transport_id(amphibious);
				amphibious.disembark();
				if (saved_transport_id != carrier.id || amphibious.transport_id != 0) {
					fail('native transport snapshot changed when amphibious cargo disembarked');
					return;
				}
				amphibious.embark(carrier);
				if (
					amphibious.transport_id != carrier.id ||
					#sizeof(carrier.get_cargo()) != 2
				) {
					fail('native amphibious cargo could not restore its transport relationship');
					return;
				}

			runtime_complete = true;
			finish_if_ready();
		});
	});

	glsmac.run();

});
