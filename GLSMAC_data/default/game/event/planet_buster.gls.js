const PLANET_BUSTER_WEAPON = 'PlanetBuster';
const ORBITAL_DEFENSE = 'OrbitalDefensePod';
const SANCTION_YEARS = 20;
const MAX_MAJOR_ATROCITIES = 1000000;
const MAX_REACTOR_POWER = 4;
const entity_snapshots = #include('../entity_snapshots');
const snapshot_unit = entity_snapshots.snapshot_unit;
const spawn_unit_snapshot = entity_snapshots.spawn_unit_snapshot;
const despawn_unit_snapshots = entity_snapshots.despawn_unit_snapshots;
const spawn_unit_snapshots = entity_snapshots.spawn_unit_snapshots;
const rehome_surviving_units = entity_snapshots.rehome_surviving_units;
const restore_rehomed_units = entity_snapshots.restore_rehomed_units;

const is_un_charter_active = (game) => {
	const is_repealed = game.get('f_council_is_un_charter_repealed');
	return !#is_defined(is_repealed) || !is_repealed();
};

const tile_key = (tile) => {
	return #to_string(tile.x) + ':' + #to_string(tile.y);
};

const get_tiles_in_radius = (center, radius) => {
	let result = [center];
	let frontier = [center];
	let seen = {};
	const center_key = tile_key(center);
	seen[center_key] = true;
	for (let distance = 0; distance < radius; distance++) {
		let next = [];
		for (tile of frontier) {
			for (nearby of tile.get_surrounding_tiles()) {
				const key = tile_key(nearby);
				if (!#is_defined(seen[key])) {
					seen[key] = true;
					result :+nearby;
					next :+nearby;
				}
			}
		}
		frontier = next;
	}
	return result;
};

const snapshot_diplomacy = (game, actor) => {
	let result = [];
	for (other of game.get_players()) {
		if (other.id != actor.id) {
			result :+{
				player_id: other.id,
				state: game.get('f_diplomacy_snapshot_pair')(actor, other),
			};
		}
	}
	return result;
};

const set_global_vendettas = (game, actor, snapshots) => {
	for (snapshot of snapshots) {
		const other = game.get_player(snapshot.player_id);
		game.get('f_diplomacy_set_bilateral_relation')(actor, other, 'vendetta');
		game.get('f_diplomacy_clear_offers')(actor, other);
		game.trigger('diplomacy_updated', {
			player: actor,
			target: other,
			relation: 'vendetta',
		});
	}
};

const set_affected_vendettas = (game, actor, snapshots, applied) => {
	let affected = {};
	if (applied.defense.player_id >= 0) {
		affected['p' + #to_string(applied.defense.player_id)] = true;
	}
	for (unit of applied.units) {
		affected['p' + #to_string(unit.owner)] = true;
	}
	for (base of applied.bases) {
		affected['p' + #to_string(base.owner_id)] = true;
	}
	for (snapshot of snapshots) {
		if (!#is_defined(affected['p' + #to_string(snapshot.player_id)])) { continue; }
		const other = game.get_player(snapshot.player_id);
		game.get('f_diplomacy_set_bilateral_relation')(actor, other, 'vendetta');
		game.get('f_diplomacy_clear_offers')(actor, other);
		game.trigger('diplomacy_updated', {
			player: actor,
			target: other,
			relation: 'vendetta',
		});
	}
};

const restore_diplomacy = (game, actor, snapshots) => {
	for (snapshot of snapshots) {
		const other = game.get_player(snapshot.player_id);
		game.get('f_diplomacy_restore_pair')(actor, other, snapshot.state);
		game.trigger('diplomacy_updated', {
			player: actor,
			target: other,
			relation: snapshot.state.player_relation,
		});
	}
};

return {

	validate: (e) => {
		const unit = e.data.unit;
		const target = e.data.tile;
		if (unit.owner != e.caller) {
			return 'Planet Buster can only be launched by its owner';
		}
		if (e.game.is_turn_complete(e.caller)) {
			return 'Player has already completed this turn';
		}
		if (unit.health <= 0.0) {
			return 'Destroyed Planet Buster cannot be launched';
		}
		if (#is_defined(unit.transport_id) && unit.transport_id > 0) {
			return 'Embarked Planet Buster must disembark before launch';
		}
		if (unit.terraforming != 'none') {
			return 'Cancel the unit\'s terraforming order before launch';
		}
		if (unit.movement <= 0.0) {
			return 'Planet Buster is out of moves';
		}
		const definition = unit.get_def();
		if (
			!definition.is_missile || definition.weapon != PLANET_BUSTER_WEAPON ||
			definition.reactor_power <= 0 || definition.reactor_power > MAX_REACTOR_POWER
		) {
			return 'Unit is not a Planet Buster';
		}
		const source = unit.get_tile();
		if (source == target) {
			return 'Planet Buster target must differ from its launch tile';
		}
		if (!source.is_adjactent_to(target)) {
			return 'Planet Buster target is not adjacent to the missile';
		}
		if (source.is_locked() || target.is_locked()) {
			return 'Planet Buster launch tiles are locked';
		}
		for (blast_tile of get_tiles_in_radius(target, definition.reactor_power)) {
			if (blast_tile.is_locked()) {
				return 'Planet Buster blast radius contains locked tiles';
			}
		}
		if (e.game.get_player(e.caller).get_major_atrocities() >= MAX_MAJOR_ATROCITIES) {
			return 'Major atrocity limit has been reached';
		}
	},

	resolve: (e) => {
		const target_base = e.data.tile.get_base();
		let defense = {
			player_id: 0 - 1,
			count: 0,
			deployments: 0,
			attempted: false,
			intercepted: false,
			sacrificed: false,
		};
		if (target_base != null) {
			const defender = target_base.get_owner();
			if (defender.id != e.caller) {
				defense.player_id = defender.id;
				defense.count = defender.get_orbital_facility_count(ORBITAL_DEFENSE);
				defense.deployments = defender.get_orbital_defense_deployments();
				if (defense.deployments < defense.count) {
					defense.attempted = true;
					defense.intercepted = e.game.random.get_int(0, 1) == 0;
				} else if (defense.count > 0) {
					defense.intercepted = true;
					defense.sacrificed = true;
				}
			}
		}
		return {
			radius: e.data.unit.get_def().reactor_power,
			defense: defense,
		};
	},

	apply: (e) => {
		const actor = e.game.get_player(e.caller);
		const missile = snapshot_unit(e.data.unit);
		let applied = {
			missile: missile,
			units: [],
			bases: [],
			rehomed_units: [],
			actor_atrocities: actor.get_major_atrocities(),
			actor_sanction_turns: actor.get_sanction_turns(),
			diplomacy: snapshot_diplomacy(e.game, actor),
			defense: e.resolved.defense,
			terrain_snapshot: null,
		};

		const defense = e.resolved.defense;
		if (defense.player_id >= 0) {
			const defender = e.game.get_player(defense.player_id);
			if (defense.attempted) {
				defender.set_orbital_defense_deployments(defense.deployments + 1);
			} else if (defense.sacrificed) {
				defender.set_orbital_facility_count(ORBITAL_DEFENSE, defense.count - 1);
				defender.set_orbital_defense_deployments(#max(0, defense.deployments - 1));
			}
		}

		if (!defense.intercepted) {
			const tiles = get_tiles_in_radius(e.data.tile, e.resolved.radius);
			for (tile of tiles) {
				for (unit of tile.get_units(true)) {
					if (unit.id != missile.id) {
						applied.units :+snapshot_unit(unit);
					}
				}
				const base = tile.get_base();
				if (base != null) {
					applied.bases :+{
						id: base.id,
						name: base.name,
						owner_id: base.get_owner().id,
						snapshot: e.game.bm.snapshot_base(base),
					};
				}
			}
			despawn_unit_snapshots(e.game, applied.units);
			applied.rehomed_units = rehome_surviving_units(
				e.game,
				applied.bases,
				missile.id
			);
			for (base of applied.bases) {
				e.game.bm.despawn_base(base.id);
			}
		}

		if (e.game.um.has_unit(missile.id)) {
			e.game.um.despawn_unit(e.game.um.get_unit(missile.id));
		}
		if (!defense.intercepted) {
			applied.terrain_snapshot = e.game.tm.apply_crater(e.data.tile, e.resolved.radius);
		}

		actor.set_major_atrocities(applied.actor_atrocities + 1);
		const charter_active = is_un_charter_active(e.game);
		if (charter_active) {
			actor.set_sanction_turns(#min(
				MAX_MAJOR_ATROCITIES,
				applied.actor_sanction_turns + SANCTION_YEARS
			));
			set_global_vendettas(e.game, actor, applied.diplomacy);
			e.game.trigger('diplomatic_sanctions_updated', {
				player: actor,
				turns: actor.get_sanction_turns(),
			});
		} else {
			set_affected_vendettas(e.game, actor, applied.diplomacy, applied);
		}
		e.game.trigger('planet_buster', {
			player: actor,
			tile: e.data.tile,
			intercepted: defense.intercepted,
			radius: e.resolved.radius,
		});

		const animation_tile = defense.intercepted
			? e.game.tm.get_tile(missile.tile_x, missile.tile_y)
			: e.data.tile;
		applied.animations_id = e.game.am.show_animations([{
			id: 'DEATH_PSI',
			tile: animation_tile,
		}]);
		if (defense.intercepted) {
			e.game.message('Orbital defenses intercepted a Planet Buster launched by ' + actor.name + '.');
		} else {
			e.game.message(actor.name + ' committed a Planet Buster atrocity.');
		}
		if (charter_active) {
			e.game.message('Economic sanctions imposed against ' + actor.name + ' for 20 years.');
		}
		return applied;
	},

	rollback: (e) => {
		const actor = e.game.get_player(e.caller);
		e.game.am.stop_animations(e.applied.animations_id);
		if (e.applied.terrain_snapshot != null) {
			e.game.tm.restore_terrain(e.applied.terrain_snapshot);
		}
		for (base of e.applied.bases) {
			e.game.bm.restore_base(base.snapshot);
		}
		spawn_unit_snapshots(e.game, e.applied.units);
		spawn_unit_snapshot(e.game, e.applied.missile);
		restore_rehomed_units(e.game, e.applied.rehomed_units);

		const defense = e.applied.defense;
		if (defense.player_id >= 0) {
			const defender = e.game.get_player(defense.player_id);
			defender.set_orbital_facility_count(ORBITAL_DEFENSE, defense.count);
			defender.set_orbital_defense_deployments(defense.deployments);
		}

		actor.set_major_atrocities(e.applied.actor_atrocities);
		actor.set_sanction_turns(e.applied.actor_sanction_turns);
		restore_diplomacy(e.game, actor, e.applied.diplomacy);
		e.game.trigger('diplomatic_sanctions_updated', {
			player: actor,
			turns: e.applied.actor_sanction_turns,
		});
	},

};
