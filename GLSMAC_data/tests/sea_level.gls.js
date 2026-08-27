const change_sea_level = #include('../default/game/event/change_sea_level');

const key = (prefix, id) => { return prefix + #to_string(id); };

const make_world = () => {
	let units = {};
	let bases = {};
	let base_pops = {};
	let base_facilities = {};
	let snapshots = {};
	let messages = [];
	let triggers = [];
	let tiles = [];
	let sea_level = 0;
	let terrain_restores = 0;

	const players = [
		{id: 0, name: 'Host'},
		{id: 1, name: 'Gaia'},
		{id: 2, name: 'Sparta'},
		{id: 3, name: 'University'},
		{id: 4, name: 'Morgan'},
	];
	const get_player = (id) => {
		for (player of players) {
			if (player.id == id) { return player; }
		}
		return null;
	};

	const make_tile = (x, y, is_water, next_water) => {
		let tile = {
			x: x,
			y: y,
			is_water: is_water,
			is_land: !is_water,
			next_water: next_water,
			base: null,
		};
		tile.get_base = () => { return tile.base; };
		tile.get_units = (include_embarked) => {
			let result = [];
			for (unit_id in units) {
				const unit = units[unit_id];
				if (
					unit != null && unit.get_tile() == tile &&
					(include_embarked || unit.transport_id == 0)
				) {
					result :+unit;
				}
			}
			return result;
		};
		tiles :+tile;
		return tile;
	};

	const flooded = make_tile(0, 0, false, true);
	const last_base_tile = make_tile(2, 0, false, true);
	const doomed_tile = make_tile(4, 0, false, true);
	const safe_tile = make_tile(6, 0, false, false);
	const dome_tile = make_tile(1, 1, false, true);
	const exposed_tile = make_tile(3, 1, true, false);
	make_tile(5, 1, true, true);
	make_tile(7, 1, true, true);

	const make_base = (id, name, owner_id, tile, size, has_dome) => {
		const state_key = key('b', id);
		base_pops[state_key] = [];
		base_facilities[state_key] = {PressureDome: has_dome};
		const owner = get_player(owner_id);
		let base = {
			id: id,
			name: name,
		};
		for (let i = 0; i < size; i++) {
			base_pops[state_key] :+{id: i};
		}
		base.get_owner = () => { return owner; };
		base.get_tile = () => { return tile; };
		base.get_size = () => { return #sizeof(base_pops[state_key]); };
		base.get_pops = () => { return base_pops[state_key]; };
		base.has_facility = (id) => {
			return #is_defined(base_facilities[state_key][id]) && base_facilities[state_key][id];
		};
		base.add_facility = (id) => { base_facilities[state_key][id] = true; };
		base.destroy_pop = (pop) => {
			let remaining = [];
			for (candidate of base_pops[state_key]) {
				if (candidate != pop) { remaining :+candidate; }
			}
			base_pops[state_key] = remaining;
		};
		const base_id = key('b', id);
		bases[base_id] = base;
		return base;
	};

	const damaged = make_base(10, 'Flooded', 1, flooded, 4, false);
	const last_base = make_base(20, 'Last Stand', 2, last_base_tile, 1, false);
	const doomed = make_base(30, 'Low Point', 3, doomed_tile, 1, false);
	const safe = make_base(31, 'High Point', 3, safe_tile, 2, false);
	const domed = make_base(40, 'Shelter', 4, dome_tile, 5, true);

	const make_unit = (id, owner_id, tile, domain, transport_id, home_base_id) => {
		let unit = {
			id: id,
			def: domain,
			owner: owner_id,
			movement: 1.0,
			morale: 1,
			health: 1.0,
			moved_this_turn: false,
			terraforming: 'none',
			terraforming_turns_remaining: 0,
			home_base_id: home_base_id,
			fuel: 0,
			transport_id: transport_id,
			is_land: domain == 'land',
			is_water: domain == 'water',
			is_air: domain == 'air',
		};
		unit.get_tile = () => { return tile; };
		unit.set_home_base_id = (value) => { unit.home_base_id = value; };
		const unit_id = key('u', id);
		units[unit_id] = unit;
		return unit;
	};

	const drowned = make_unit(100, 1, flooded, 'land', 0, damaged.id);
	const aircraft = make_unit(101, 1, flooded, 'air', 0, damaged.id);
	const transport = make_unit(102, 3, exposed_tile, 'water', 0, safe.id);
	const cargo = make_unit(103, 3, exposed_tile, 'land', transport.id, safe.id);
	const rehomed = make_unit(104, 3, safe_tile, 'land', 0, doomed.id);

	const game = {
		random: {get_int: (low, high) => {
			test.assert(low == 2 && high == 4);
			return 2;
		}},
		get_player: get_player,
		message: (message) => { messages :+message; },
		trigger: (name, data) => { triggers :+{name: name, data: data}; },
	};
	game.tm = {
		get_map_width: () => { return 8; },
		get_map_height: () => { return 2; },
		get_tile: (x, y) => {
			for (tile of tiles) {
				if (tile.x == x && tile.y == y) { return tile; }
			}
			return null;
		},
		get_distance: (first, second) => {
			return #abs(first.x - second.x) + #abs(first.y - second.y);
		},
		get_sea_level: () => { return sea_level; },
		apply_sea_level_change: (amount) => {
			test.assert(amount == 100);
			sea_level += amount;
			for (tile of tiles) {
				tile.is_water = tile.next_water;
				tile.is_land = !tile.is_water;
			}
			return 'sea:before';
		},
		restore_sea_level: (snapshot) => {
			test.assert(snapshot == 'sea:before');
			sea_level = 0;
			terrain_restores++;
			for (tile of tiles) {
				tile.is_water = tile == exposed_tile || tile.x >= 5 && tile.y == 1;
				tile.is_land = !tile.is_water;
			}
		},
	};
	game.um = {
		get_units: () => {
			let result = [];
			for (unit_id in units) {
				if (units[unit_id] != null) { result :+units[unit_id]; }
			}
			return result;
		},
		has_unit: (id) => {
			const unit_id = key('u', id);
			return #is_defined(units[unit_id]) && units[unit_id] != null;
		},
		get_unit: (id) => {
			const unit_id = key('u', id);
			return units[unit_id];
		},
		despawn_unit: (unit) => {
			const unit_id = key('u', unit.id);
			units[unit_id] = null;
		},
		spawn_unit: (info) => {
			if (info.transport_id != 0) {
				test.assert(game.um.has_unit(info.transport_id));
			}
			const unit = make_unit(
				info.id,
				info.owner.id,
				info.tile,
				info.def,
				info.transport_id,
				info.home_base_id
			);
			unit.morale = info.morale;
			unit.health = info.health;
			unit.terraforming = info.terraforming;
			unit.terraforming_turns_remaining = info.terraforming_turns_remaining;
			unit.fuel = info.fuel;
			return unit;
		},
	};
	game.bm = {
		get_bases: () => {
			let result = [];
			for (base_id in bases) {
				if (bases[base_id] != null) { result :+bases[base_id]; }
			}
			return result;
		},
		snapshot_base: (base) => {
			const snapshot = 'base:' + #to_string(base.id);
			snapshots[snapshot] = {
				id: base.id,
				name: base.name,
				owner_id: base.get_owner().id,
				tile: base.get_tile(),
				size: base.get_size(),
				has_dome: base.has_facility('PressureDome'),
			};
			return snapshot;
		},
		despawn_base: (base_or_id) => {
			const id = #typeof(base_or_id) == 'Int' ? base_or_id : base_or_id.id;
			const base_id = key('b', id);
			const base = bases[base_id];
			const base_tile = base.get_tile();
			base_tile.base = null;
			bases[base_id] = null;
		},
		restore_base: (snapshot) => {
			const data = snapshots[snapshot];
			return make_base(
				data.id,
				data.name,
				data.owner_id,
				data.tile,
				data.size,
				data.has_dome
			);
		},
	};

	return {
		game: game,
		bases: {damaged: damaged, last: last_base, doomed: doomed, safe: safe, domed: domed},
		units: {drowned: drowned, aircraft: aircraft, transport: transport, cargo: cargo, rehomed: rehomed},
		get_base: (id) => {
			const base_id = key('b', id);
			return bases[base_id];
		},
		get_messages: () => { return messages; },
		get_triggers: () => { return triggers; },
		get_terrain_restores: () => { return terrain_restores; },
	};
};

let world = make_world();
let event = {caller: 1, data: {amount: 100}, game: world.game};
test.assert(change_sea_level.validate(event) == 'Only the host can change sea level');
event.caller = 0;
event.data.amount = 0;
test.assert(#is_defined(change_sea_level.validate(event)));
event.data.amount = 100;
test.assert(!#is_defined(change_sea_level.validate(event)));

const active_bases = world.game.bm.get_bases();
test.assert(#sizeof(active_bases) == 5);
test.assert(#typeof(active_bases[0]) == 'Object');
test.assert(active_bases[0].id > 0);
event.resolved = change_sea_level.resolve(event);
event.applied = change_sea_level.apply(event);
test.assert(world.game.tm.get_sea_level() == 100);
test.assert(world.bases.damaged.get_size() == 2);
test.assert(world.bases.damaged.has_facility('PressureDome'));
test.assert(world.bases.last.get_size() == 1);
test.assert(world.bases.last.has_facility('PressureDome'));
test.assert(world.get_base(world.bases.doomed.id) == null);
test.assert(world.get_base(world.bases.safe.id) != null);
test.assert(world.bases.domed.get_size() == 5);
test.assert(world.game.um.has_unit(world.units.drowned.id) == false);
test.assert(world.game.um.has_unit(world.units.aircraft.id));
test.assert(world.game.um.has_unit(world.units.transport.id) == false);
test.assert(world.game.um.has_unit(world.units.cargo.id) == false);
test.assert(
	world.game.um.get_unit(world.units.rehomed.id).home_base_id == world.bases.safe.id
);
test.assert(#sizeof(event.applied.units) == 3);
test.assert(#sizeof(event.applied.bases) == 3);
test.assert(#sizeof(world.get_messages()) == 5);
test.assert(#sizeof(world.get_triggers()) == 1);

change_sea_level.rollback(event);
test.assert(world.game.tm.get_sea_level() == 0);
test.assert(world.get_terrain_restores() == 1);
test.assert(world.get_base(world.bases.damaged.id).get_size() == 4);
test.assert(!world.get_base(world.bases.damaged.id).has_facility('PressureDome'));
test.assert(world.get_base(world.bases.last.id).get_size() == 1);
test.assert(!world.get_base(world.bases.last.id).has_facility('PressureDome'));
test.assert(world.get_base(world.bases.doomed.id).get_size() == 1);
test.assert(world.get_base(world.bases.domed.id).has_facility('PressureDome'));
test.assert(world.game.um.has_unit(world.units.drowned.id));
test.assert(world.game.um.has_unit(world.units.transport.id));
test.assert(world.game.um.has_unit(world.units.cargo.id));
test.assert(
	world.game.um.get_unit(world.units.rehomed.id).home_base_id == world.bases.doomed.id
);
