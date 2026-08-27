const move_unit = #include('../default/game/event/move_unit');

const make_terraforming = () => {
	return {
		road: false,
		mag_tube: false,
		forest: false,
		farm: false,
		soil_enricher: false,
		solar: false,
		mine: false,
		condenser: false,
		mirror: false,
		borehole: false,
		sensor: false,
		bunker: false,
		airbase: false,
		remove_fungus: false,
		plant_fungus: false,
	};
};

const make_tile = (x, has_pod) => {
	let units = [];
	const tile = {
		x: x,
		y: 0,
		is_land: true,
		is_water: false,
		rockiness: 1,
		elevation: 1000,
		features: {
			river: false,
			monolith: false,
			xenofungus: false,
			unity_pod: has_pod,
		},
		bonuses: {nutrient: false, energy: false, minerals: false},
		terraforming: make_terraforming(),
		get_base: () => { return null; },
		get_surrounding_tiles: () => { return []; },
		get_units: (include_embarked) => { return units; },
		set_units: (values) => { units = values; },
		is_locked: () => { return false; },
		update_features: (changes) => {
			if (#is_defined(changes.river)) { tile.features.river = changes.river; }
			if (#is_defined(changes.monolith)) { tile.features.monolith = changes.monolith; }
			if (#is_defined(changes.xenofungus)) { tile.features.xenofungus = changes.xenofungus; }
			if (#is_defined(changes.unity_pod)) { tile.features.unity_pod = changes.unity_pod; }
		},
		update_terraforming: (changes) => {
			if (#is_defined(changes.road)) { tile.terraforming.road = changes.road; }
			if (#is_defined(changes.mag_tube)) { tile.terraforming.mag_tube = changes.mag_tube; }
			if (#is_defined(changes.forest)) { tile.terraforming.forest = changes.forest; }
			if (#is_defined(changes.farm)) { tile.terraforming.farm = changes.farm; }
			if (#is_defined(changes.soil_enricher)) { tile.terraforming.soil_enricher = changes.soil_enricher; }
			if (#is_defined(changes.solar)) { tile.terraforming.solar = changes.solar; }
			if (#is_defined(changes.mine)) { tile.terraforming.mine = changes.mine; }
			if (#is_defined(changes.condenser)) { tile.terraforming.condenser = changes.condenser; }
			if (#is_defined(changes.mirror)) { tile.terraforming.mirror = changes.mirror; }
			if (#is_defined(changes.borehole)) { tile.terraforming.borehole = changes.borehole; }
			if (#is_defined(changes.sensor)) { tile.terraforming.sensor = changes.sensor; }
			if (#is_defined(changes.bunker)) { tile.terraforming.bunker = changes.bunker; }
			if (#is_defined(changes.airbase)) { tile.terraforming.airbase = changes.airbase; }
			if (#is_defined(changes.remove_fungus)) { tile.terraforming.remove_fungus = changes.remove_fungus; }
			if (#is_defined(changes.plant_fungus)) { tile.terraforming.plant_fungus = changes.plant_fungus; }
		},
		set_bonus: (name) => {
			tile.bonuses.nutrient = name == 'nutrient';
			tile.bonuses.energy = name == 'energy';
			tile.bonuses.minerals = name == 'minerals';
		},
	};
	return tile;
};

const source = make_tile(0, false);
const destination = make_tile(2, true);
source.is_adjactent_to = (tile) => { return tile == destination; };
destination.is_adjactent_to = (tile) => { return tile == source; };
destination.update_features = (changes) => {
	if (#is_defined(changes.river)) { destination.features.river = changes.river; }
	if (#is_defined(changes.monolith)) { destination.features.monolith = changes.monolith; }
	if (#is_defined(changes.xenofungus)) { destination.features.xenofungus = changes.xenofungus; }
	if (#is_defined(changes.unity_pod)) { destination.features.unity_pod = changes.unity_pod; }
};
destination.update_terraforming = (changes) => {
	if (#is_defined(changes.road)) { destination.terraforming.road = changes.road; }
	if (#is_defined(changes.mag_tube)) { destination.terraforming.mag_tube = changes.mag_tube; }
	if (#is_defined(changes.forest)) { destination.terraforming.forest = changes.forest; }
	if (#is_defined(changes.farm)) { destination.terraforming.farm = changes.farm; }
	if (#is_defined(changes.soil_enricher)) { destination.terraforming.soil_enricher = changes.soil_enricher; }
	if (#is_defined(changes.solar)) { destination.terraforming.solar = changes.solar; }
	if (#is_defined(changes.mine)) { destination.terraforming.mine = changes.mine; }
	if (#is_defined(changes.condenser)) { destination.terraforming.condenser = changes.condenser; }
	if (#is_defined(changes.mirror)) { destination.terraforming.mirror = changes.mirror; }
	if (#is_defined(changes.borehole)) { destination.terraforming.borehole = changes.borehole; }
	if (#is_defined(changes.sensor)) { destination.terraforming.sensor = changes.sensor; }
	if (#is_defined(changes.bunker)) { destination.terraforming.bunker = changes.bunker; }
	if (#is_defined(changes.airbase)) { destination.terraforming.airbase = changes.airbase; }
	if (#is_defined(changes.remove_fungus)) { destination.terraforming.remove_fungus = changes.remove_fungus; }
	if (#is_defined(changes.plant_fungus)) { destination.terraforming.plant_fungus = changes.plant_fungus; }
};
destination.set_bonus = (name) => {
	destination.bonuses.nutrient = name == 'nutrient';
	destination.bonuses.energy = name == 'energy';
	destination.bonuses.minerals = name == 'minerals';
};

const player = {
	id: 1,
	name: 'Gaia',
	energy_credits: 0,
	get_energy_credits: () => { return player.energy_credits; },
	set_energy_credits: (value) => { player.energy_credits = value; },
	has_technology: (id) => { return false; },
};
const definition = {
	id: 'ScoutPatrol',
	name: 'Scout Patrol',
	is_native: false,
	offense: 1,
	weapon: 'HandWeapons',
	cargo_capacity: 0,
};
let current_tile = source;
let unit = null;
unit = {
	id: 1,
	owner: player.id,
	movement: 1.0,
	moved_this_turn: false,
	health: 1.0,
	morale: 2,
	transport_id: 0,
	is_immovable: false,
	is_land: true,
	is_water: false,
	terraforming: 'none',
	get_def: () => { return definition; },
	get_owner: () => { return player; },
	get_tile: () => { return current_tile; },
	get_transport: () => { return null; },
	get_cargo: () => { return []; },
	move_to_tile: (tile, done) => {
		current_tile.set_units([]);
		current_tile = tile;
		current_tile.set_units([unit]);
		done();
	},
	embark: (transport) => {},
	disembark: () => {},
};
source.set_units([unit]);

let rolls = [91, 2];
let roll_index = 0;
let messages = [];
let triggers = [];
const game = {
	is_turn_complete: (id) => { return false; },
	get_turn: () => { return 1; },
	get_player: (id) => { return player; },
	get: (key) => { return #undefined; },
	random: {
		get_float: (low, high) => { throw Error('movement success should not need a random roll'); },
		get_int: (low, high) => {
			const value = rolls[roll_index];
			roll_index++;
			test.assert(value >= low && value <= high);
			return value;
		},
	},
	bm: {get_bases: () => { return []; }},
	tm: {get_distance: (first, second) => { return #abs(first.x - second.x); }},
	um: {
		get_unit_defs: () => { return [definition]; },
		has_unit: (id) => { return id == unit.id; },
		get_unit: (id) => { return unit; },
		despawn_unit: (value) => {},
	},
	message: (text) => { messages :+text; },
	trigger: (name, data) => { triggers :+{name: name, data: data}; },
};

let event = {
	caller: player.id,
	game: game,
	data: {unit: unit, tile: destination},
};
test.assert(!#is_defined(move_unit.validate(event)));
event.resolved = move_unit.resolve(event);
test.assert(event.resolved.is_movement_successful);
test.assert(event.resolved.unity_pod.kind == 'resource');
test.assert(event.resolved.unity_pod.bonus == 'minerals');

event.applied = move_unit.apply(event);
test.assert(current_tile.x == destination.x && current_tile.y == destination.y);
test.assert(!destination.features.unity_pod);
test.assert(destination.bonuses.minerals);
test.assert(#sizeof(messages) == 1 && #sizeof(triggers) == 1);

move_unit.rollback(event);
test.assert(current_tile.x == source.x && current_tile.y == source.y);
test.assert(destination.features.unity_pod);
test.assert(!destination.bonuses.minerals);
