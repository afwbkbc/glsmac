const native_strategy = #include('../default/game/native_strategy');

const native = {id: 7, type: 'native'};
const human = {id: 1, type: 'human'};

const make_tile = (x, y, is_water) => {
	let tile = {
		x: x,
		y: y,
		is_water: is_water,
		is_land: !is_water,
		rockiness: 1,
		features: {xenofungus: false},
		terraforming: {bunker: false},
		surrounding: [],
		units: [],
		base: null,
		locked: false,
	};
	tile.get_surrounding_tiles = () => { return tile.surrounding; };
	tile.get_units = () => { return tile.units; };
	tile.get_base = () => { return tile.base; };
	tile.is_locked = () => { return tile.locked; };
	return tile;
};

const worm_def = {
	id: 'MindWorms',
	is_native: true,
	is_artillery: false,
	offense: 1,
	defense: 1,
	movement_per_turn: 1,
	abilities: [],
};
const scout_def = {
	id: 'ScoutPatrol',
	is_native: false,
	is_artillery: false,
	offense: 1,
	defense: 1,
	movement_per_turn: 1,
	abilities: [],
};

const source = make_tile(0, 0, false);
const east = make_tile(2, 0, false);
const south = make_tile(1, 1, false);
source.surrounding = [east, south];
east.surrounding = [source];
south.surrounding = [source];

const worm = {
	id: 10,
	owner: native.id,
	health: 1.0,
	morale: 1,
	movement: 1.0,
	is_land: true,
	is_water: false,
	is_air: false,
	get_tile: () => { return source; },
	get_def: () => { return worm_def; },
};
const defender = {
	id: 20,
	owner: human.id,
	health: 1.0,
	morale: 1,
	is_land: true,
	is_water: false,
	is_air: false,
	get_tile: () => { return east; },
	get_def: () => { return scout_def; },
};
east.units = [defender];

let units = [worm, defender];
let bases = [];
let random_values = [0];
let random_index = 0;
let turn = 40;
let density = 0.5;
let map_tiles = {};
for (tile of [source, east, south]) {
	map_tiles[#to_string(tile.x) + '_' + #to_string(tile.y)] = tile;
}
const tm = {
	get_distance: (first, second) => {
		return #abs(first.x - second.x) + #abs(first.y - second.y);
	},
	get_map_width: () => { return 4; },
	get_map_height: () => { return 2; },
	get_tile: (x, y) => { return map_tiles[#to_string(x) + '_' + #to_string(y)]; },
};
const um = {get_units: () => { return units; }};
const bm = {get_bases: () => { return bases; }};
const game = {
	random: {
		get_int: (low, high) => {
			const value = random_values[random_index];
			random_index++;
			test.assert(value >= low && value <= high);
			return value;
		},
	},
	get_native_player: () => { return native; },
	get_settings: () => { return {global: {map: {native_lifeforms: density}}}; },
	get_turn: () => { return turn; },
	get_um: () => { return um; },
	get_bm: () => { return bm; },
	tm: tm,
};

let action = native_strategy.choose_action(game, worm);
test.assert(action.kind == 'attack' && action.defender_id == defender.id);

east.units = [];
const distant = make_tile(6, 0, false);
const distant_defender = {
	id: 21,
	owner: human.id,
	health: 1.0,
	morale: 1,
	is_land: true,
	is_water: false,
	is_air: false,
	get_tile: () => { return distant; },
	get_def: () => { return scout_def; },
};
units = [worm, distant_defender];
random_values = [0];
random_index = 0;
action = native_strategy.choose_action(game, worm);
test.assert(action.kind == 'move' && action.tile_x == east.x && action.tile_y == east.y);

east.base = {get_owner: () => { return human; }};
test.assert(!native_strategy.can_enter(worm, east));
east.base = null;

const empty_a = make_tile(0, 1, false);
const fungus_land = make_tile(1, 1, false);
const empty_b = make_tile(2, 1, false);
const fungus_sea = make_tile(3, 1, true);
for (tile of [source, empty_a, east, fungus_land, empty_b, fungus_sea]) {
	map_tiles[#to_string(tile.x) + '_' + #to_string(tile.y)] = tile;
}
fungus_land.features.xenofungus = true;
random_values = [0, 0];
random_index = 0;
units = [];
let spawn = native_strategy.select_ambient_spawn(game);
test.assert(spawn.type == 'MindWorms' && spawn.tile == fungus_land);

fungus_land.features.xenofungus = false;
fungus_sea.features.xenofungus = true;
random_values = [0, 0];
random_index = 0;
spawn = native_strategy.select_ambient_spawn(game);
test.assert(spawn.type == 'IsleOfTheDeep' && spawn.tile == fungus_sea);

turn = 80;
random_values = [0, 0, 0];
random_index = 0;
spawn = native_strategy.select_ambient_spawn(game);
test.assert(spawn.type == 'LocustsOfChiron');

density = 0.0;
random_values = [];
random_index = 0;
test.assert(native_strategy.select_ambient_spawn(game) == null);
