const native_life = #include('../default/game/native_life');

const make_tile = (id, is_water, occupied) => {
	let tile = {
		id: id,
		is_water: is_water,
		is_land: !is_water,
		surrounding: [],
	};
	tile.get_base = () => { return null; };
	tile.get_units = () => { return occupied ? [{id: 99}] : []; };
	tile.get_surrounding_tiles = () => { return tile.surrounding; };
	return tile;
};

let next_roll = 1;
const native_player = {id: 8, name: 'Planet'};
let next_unit_id = 100;
let units = {};
const game = {
	random: {
		get_int: (low, high) => {
			test.assert(next_roll >= low && next_roll <= high);
			return next_roll;
		},
	},
	get_settings: () => {
		return {global: {map: {native_lifeforms: 0.75}}};
	},
	get_native_player: () => { return native_player; },
	um: {
		spawn_unit: (data) => {
			const id = next_unit_id;
			next_unit_id++;
			const unit = {
				id: id,
				def: data.def,
				owner: data.owner,
				tile: data.tile,
				transport_id: data.transport_id,
				movement: 1.0,
				moved_this_turn: false,
				present: true,
			};
			units['u' + #to_string(id)] = unit;
			return unit;
		},
		has_unit: (id) => {
			const key = 'u' + #to_string(id);
			return #is_defined(units[key]) && units[key].present;
		},
		get_unit: (id) => { return units['u' + #to_string(id)]; },
		despawn_unit: (unit) => { unit.present = false; },
	},
};

test.assert(native_life.get_life_level(game) == 3);
test.assert(native_life.get_life_level({}) == 2);

const land_center = make_tile('center', false, true);
const land_a = make_tile('a', false, false);
const land_b = make_tile('b', false, false);
const water = make_tile('water', true, false);
const occupied = make_tile('occupied', false, true);
land_center.surrounding = [land_a, water, occupied, land_b];

next_roll = 1;
const land = native_life.resolve_outbreak(game, land_center, 3, false);
test.assert(#sizeof(land.spawns) == 2);
test.assert(land.spawns[0].tile == land_b);
test.assert(land.spawns[1].tile == land_a);
test.assert(land.spawns[0].def == 'MindWorms');

const land_applied = native_life.apply_outbreak(game, land);
test.assert(#sizeof(land_applied.unit_ids) == 2);
for (id of land_applied.unit_ids) {
	const unit = game.um.get_unit(id);
	test.assert(unit.owner == native_player);
	test.assert(unit.movement == 0.0 && unit.moved_this_turn);
}
native_life.rollback_outbreak(game, land_applied);
for (id of land_applied.unit_ids) {
	test.assert(!game.um.has_unit(id));
}

const sea_center = make_tile('sea-center', true, true);
const sea_spawn = make_tile('sea-spawn', true, false);
sea_center.surrounding = [sea_spawn, land_a];
next_roll = 0;
const sea = native_life.resolve_outbreak(game, sea_center, 3, false);
test.assert(#sizeof(sea.spawns) == 3);
test.assert(sea.spawns[0].def == 'IsleOfTheDeep');
test.assert(sea.spawns[1].def == 'MindWorms' && sea.spawns[1].transport_index == 0);
test.assert(sea.spawns[2].def == 'MindWorms' && sea.spawns[2].transport_index == 0);

const sea_applied = native_life.apply_outbreak(game, sea);
const isle = game.um.get_unit(sea_applied.unit_ids[0]);
test.assert(isle.def == 'IsleOfTheDeep' && isle.transport_id == 0);
for (let i = 1; i < #sizeof(sea_applied.unit_ids); i++) {
	const worm = game.um.get_unit(sea_applied.unit_ids[i]);
	test.assert(worm.transport_id == isle.id);
}
native_life.rollback_outbreak(game, sea_applied);
for (id of sea_applied.unit_ids) {
	test.assert(!game.um.has_unit(id));
}
