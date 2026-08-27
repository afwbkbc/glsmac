const turns = #include('../default/units/turns');

const owner = {id: 1};
const other_owner = {id: 2};
const field_tile = {get_base: () => { return null; }};
const friendly_tile = {
	get_base: () => { return {get_owner: () => { return owner; }}; },
};
const hostile_tile = {
	get_base: () => { return {get_owner: () => { return other_owner; }}; },
};
const def = {
	health_max: 1.0,
	health_per_turn: 0.1,
};

const make_unit = (tile, health, moved, terraforming) => {
	return {
		owner: owner.id,
		health: health,
		moved_this_turn: moved,
		terraforming: terraforming,
		get_tile: () => { return tile; },
	};
};

let repair = turns.get_repair(make_unit(field_tile, 0.5, false, 'none'), def);
test.assert(repair > 0.099 && repair < 0.101);

repair = turns.get_repair(make_unit(friendly_tile, 0.5, false, 'none'), def);
test.assert(repair > 0.199 && repair < 0.201);

repair = turns.get_repair(make_unit(hostile_tile, 0.5, false, 'none'), def);
test.assert(repair > 0.099 && repair < 0.101);

repair = turns.get_repair(make_unit(friendly_tile, 0.5, true, 'none'), def);
test.assert(repair == 0.0);

repair = turns.get_repair(make_unit(friendly_tile, 0.5, false, 'farm'), def);
test.assert(repair == 0.0);

repair = turns.get_repair(make_unit(friendly_tile, 0.95, false, 'none'), def);
test.assert(repair > 0.049 && repair < 0.051);

repair = turns.get_repair(make_unit(field_tile, 0.75, false, 'none'), def);
test.assert(repair > 0.049 && repair < 0.051);

repair = turns.get_repair(make_unit(field_tile, 0.8, false, 'none'), def);
test.assert(repair == 0.0);

repair = turns.get_repair(make_unit(field_tile, 0.9, false, 'none'), def);
test.assert(repair == 0.0);

repair = turns.get_repair(make_unit(friendly_tile, 1.0, false, 'none'), def);
test.assert(repair == 0.0);

repair = turns.get_repair(
	make_unit(field_tile, 0.5, false, 'none'),
	def,
	{full_repair: true}
);
test.assert(repair == 0.5);

repair = turns.get_repair(
	make_unit(field_tile, 0.9, false, 'none'),
	def,
	{full_repair: true}
);
test.assert(repair > 0.099 && repair < 0.101);

const land_def = {
	health_max: 1.0,
	health_per_turn: 0.1,
	is_land: true,
	is_water: false,
	is_air: false,
	is_native: false,
};
const native_water_def = {
	health_max: 1.0,
	health_per_turn: 0.1,
	is_land: false,
	is_water: true,
	is_air: false,
	is_native: true,
};
const repair_facilities = [{full_repair_land: true}];
repair = turns.get_repair(
	make_unit(friendly_tile, 0.5, false, 'none'),
	land_def,
	{full_repair: false},
	repair_facilities
);
test.assert(repair == 0.5);
repair = turns.get_repair(
	make_unit(hostile_tile, 0.5, false, 'none'),
	land_def,
	{full_repair: false},
	repair_facilities
);
test.assert(repair > 0.099 && repair < 0.101);
repair = turns.get_repair(
	make_unit(friendly_tile, 0.5, false, 'none'),
	native_water_def,
	{full_repair: false},
	[{full_repair_native: true}]
);
test.assert(repair == 0.5);
test.assert(turns.facility_repairs_unit({full_repair_water: true}, native_water_def));
test.assert(turns.facility_repairs_unit(
	{full_repair_air: true},
	{is_land: false, is_water: false, is_air: true, is_native: false}
));
test.assert(!turns.facility_repairs_unit({full_repair_air: true}, land_def));

const bunker_tile = {
	terraforming: {bunker: true, airbase: false},
	get_base: () => { return null; },
};
repair = turns.get_repair(
	make_unit(bunker_tile, 0.5, false, 'none'),
	land_def,
	{full_repair: false}
);
test.assert(repair > 0.199 && repair < 0.201);

const airbase_tile = {
	terraforming: {bunker: false, airbase: true},
	get_base: () => { return null; },
};
const air_def = {
	health_max: 1.0,
	health_per_turn: 0.1,
	is_land: false,
	is_water: false,
	is_air: true,
	is_native: false,
};
repair = turns.get_repair(
	make_unit(airbase_tile, 0.5, false, 'none'),
	air_def,
	{full_repair: false}
);
test.assert(repair > 0.199 && repair < 0.201);

const repair_bay_unit = {
	owner: owner.id,
	health: 0.5,
	moved_this_turn: false,
	terraforming: 'none',
	is_embarked: true,
	get_tile: () => { return field_tile; },
	get_transport: () => {
		return {get_def: () => { return {abilities: ['RepairBay']}; }};
	},
};
repair = turns.get_repair(repair_bay_unit, land_def, {full_repair: false});
test.assert(repair > 0.199 && repair < 0.201);

const naval_def = {
	health_max: 1.0,
	health_per_turn: 0.1,
	is_immovable: false,
	is_native: false,
	movement_per_turn: 3.0,
};
const naval_unit = {
	owner: owner.id,
	health: 0.5,
	movement: 0.0,
	moved_this_turn: false,
	terraforming: 'none',
	is_water: true,
	get_def: () => { return naval_def; },
	get_owner: () => { return owner; },
	get_tile: () => { return field_tile; },
};
test.assert(turns.get_movement(
	naval_unit,
	naval_def,
	{naval_movement_bonus: 2.0, full_repair: true}
) == 5.0);

const make_air_tile = (base_owner, airbase) => {
	return {
		terraforming: {airbase: airbase},
		get_base: () => {
			return base_owner == null
				? null
				: {get_owner: () => { return base_owner; }};
		},
		get_units: () => { return []; },
	};
};

const needlejet_def = {
	is_air: true,
	operational_range: 2,
	chassis: 'Needlejet',
};
const make_air_unit = (tile, fuel) => {
	return {
		id: 10,
		owner: owner.id,
		fuel: fuel,
		get_tile: () => { return tile; },
	};
};

let air_state = turns.get_air_turn_state(
	make_air_unit(make_air_tile(null, false), 2),
	needlejet_def
);
test.assert(air_state == {fuel: 1, damage: 0.0, crash: false, refueling: false});

air_state = turns.get_air_turn_state(
	make_air_unit(make_air_tile(null, false), 1),
	needlejet_def
);
test.assert(air_state == {fuel: 0, damage: 0.0, crash: true, refueling: false});

air_state = turns.get_air_turn_state(
	make_air_unit(make_air_tile(owner, false), 0),
	needlejet_def
);
test.assert(air_state == {fuel: 2, damage: 0.0, crash: false, refueling: true});

air_state = turns.get_air_turn_state(
	make_air_unit(make_air_tile(other_owner, false), 1),
	needlejet_def
);
test.assert(air_state == {fuel: 0, damage: 0.0, crash: true, refueling: false});

air_state = turns.get_air_turn_state(
	make_air_unit(make_air_tile(null, true), 0),
	needlejet_def
);
test.assert(air_state == {fuel: 2, damage: 0.0, crash: false, refueling: true});

const carrier_tile = make_air_tile(null, false);
const carrier_aircraft = make_air_unit(carrier_tile, 0);
carrier_tile.get_units = () => {
	return [
		carrier_aircraft,
		{
			id: 11,
			owner: owner.id,
			get_def: () => { return {abilities: ['CarrierDeck']}; },
		},
	];
};
air_state = turns.get_air_turn_state(carrier_aircraft, needlejet_def);
test.assert(air_state == {fuel: 2, damage: 0.0, crash: false, refueling: true});

air_state = turns.get_air_turn_state(
	make_air_unit(make_air_tile(null, false), 1),
	{is_air: true, operational_range: 1, chassis: 'Copter'}
);
test.assert(air_state == {fuel: 0, damage: 0.3, crash: false, refueling: false});

air_state = turns.get_air_turn_state(
	make_air_unit(make_air_tile(null, false), 0),
	{is_air: true, operational_range: 0, chassis: 'Gravship'}
);
test.assert(air_state == {fuel: 0, damage: 0.0, crash: false, refueling: false});
