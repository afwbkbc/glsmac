const combat_rules = #include('../default/game/combat_rules');
const attack_unit = #include('../default/game/event/attack_unit');
const move_unit = #include('../default/game/event/move_unit');
const advance_unit_after_combat = #include('../default/game/event/advance_unit_after_combat');

const attacker_owner = {id: 1, type: 'human'};
const defender_owner = {id: 2, type: 'ai'};

const make_tile = (is_land) => {
	let base = null;
	let units = [];
	let surrounding = [];
	const tile = {
		x: is_land ? 2 : 3,
		y: 4,
		is_land: is_land,
		is_water: !is_land,
		rockiness: 1,
		features: {xenofungus: false, river: false, unity_pod: false},
		terraforming: {road: false, bunker: false, forest: false, airbase: false},
		is_locked: () => { return false; },
		is_adjactent_to: (other) => { return true; },
		get_base: () => { return base; },
		set_base: (value) => { base = value; },
		get_units: (include_embarked) => { return units; },
		set_units: (value) => { units = value; },
		get_surrounding_tiles: () => { return surrounding; },
		set_surrounding_tiles: (value) => { surrounding = value; },
	};
	return tile;
};

const make_base = (id, owner, tile) => {
	const base = {
		id: id,
		get_owner: () => { return owner; },
		get_tile: () => { return tile; },
	};
	tile.set_base(base);
	return base;
};

const make_def = (id, abilities, movement_type, chassis) => {
	return {
		id: id,
		name: id,
		is_native: false,
		is_artillery: false,
		is_missile: false,
		is_psi_attack: false,
		is_psi_defense: false,
		offense: 2,
		defense: 2,
		movement_per_turn: movement_type == 'air' ? 8.0 : 1.0,
		abilities: abilities,
		chassis: chassis,
	};
};

const make_unit = (id, owner, tile, def, movement_type) => {
	return {
		id: id,
		owner: owner.id,
		movement: 1.0,
		morale: 2,
		health: 1.0,
		moved_this_turn: false,
		terraforming: 'none',
		transport_id: 0,
		is_immovable: false,
		is_land: movement_type == 'land',
		is_water: movement_type == 'water',
		is_air: movement_type == 'air',
		get_owner: () => { return owner; },
		get_tile: () => { return tile; },
		get_def: () => { return def; },
		get_transport: () => { return null; },
	};
};

const land_tile = make_tile(true);
const target_land = make_tile(true);
const water_tile = make_tile(false);
const sea_base_tile = make_tile(false);
make_base(7, defender_owner, sea_base_tile);
land_tile.set_surrounding_tiles([water_tile]);
target_land.set_surrounding_tiles([water_tile]);
water_tile.set_surrounding_tiles([land_tile]);
sea_base_tile.set_surrounding_tiles([land_tile]);

const ordinary_def = make_def('OrdinaryInfantry', [], 'land', 'Infantry');
const amphibious_def = make_def(
	'AmphibiousInfantry',
	['AmphibiousPods'],
	'land',
	'Infantry'
);
const sam_def = make_def('SAMInfantry', ['AirSuperiority'], 'land', 'Infantry');
const sea_sam_def = make_def('SAMFoil', ['AirSuperiority'], 'water', 'Foil');
const embarked_sam_def = make_def(
	'EmbarkedSAMInfantry',
	['AirSuperiority', 'AmphibiousPods'],
	'land',
	'Infantry'
);
const bomber_def = make_def('Needlejet', [], 'air', 'Needlejet');
bomber_def.offense = 6;
bomber_def.defense = 1;
const copter_def = make_def('Copter', [], 'air', 'Copter');

const ordinary = make_unit(1, attacker_owner, water_tile, ordinary_def, 'land');
ordinary.transport_id = 50;
const amphibious = make_unit(2, attacker_owner, water_tile, amphibious_def, 'land');
amphibious.transport_id = 50;
const land_defender = make_unit(3, defender_owner, target_land, ordinary_def, 'land');

test.assert(!combat_rules.can_attack_target(ordinary, land_defender));
test.assert(combat_rules.can_attack_target(amphibious, land_defender));
test.assert(combat_rules.can_advance_after_combat(amphibious, target_land));
test.assert(!combat_rules.can_advance_after_combat(amphibious, water_tile));
test.assert(combat_rules.can_advance_after_combat(amphibious, sea_base_tile));

ordinary.transport_id = 0;
ordinary.get_tile = () => { return land_tile; };
const sea_base_defender = make_unit(4, defender_owner, sea_base_tile, ordinary_def, 'water');
const open_water_defender = make_unit(5, defender_owner, water_tile, ordinary_def, 'water');
test.assert(!combat_rules.can_attack_target(ordinary, sea_base_defender));
const coastal_amphibious = make_unit(6, attacker_owner, land_tile, amphibious_def, 'land');
test.assert(combat_rules.can_attack_target(coastal_amphibious, sea_base_defender));
test.assert(!combat_rules.can_attack_target(coastal_amphibious, open_water_defender));
make_base(8, attacker_owner, land_tile);
test.assert(combat_rules.can_attack_target(coastal_amphibious, open_water_defender));

const bomber = make_unit(7, defender_owner, target_land, bomber_def, 'air');
const sam = make_unit(8, attacker_owner, land_tile, sam_def, 'land');
const rifle = make_unit(9, attacker_owner, land_tile, ordinary_def, 'land');
test.assert(combat_rules.is_air_unit_in_flight(bomber));
test.assert(!combat_rules.can_attack_target(rifle, bomber));
test.assert(combat_rules.can_attack_target(sam, bomber));
const bomber_over_water = make_unit(17, defender_owner, water_tile, bomber_def, 'air');
const sea_sam = make_unit(18, attacker_owner, water_tile, sea_sam_def, 'water');
test.assert(combat_rules.can_attack_target(sam, bomber_over_water));
test.assert(combat_rules.can_attack_target(sea_sam, bomber));
sam.transport_id = 50;
test.assert(!combat_rules.can_attack_target(sam, bomber));
const embarked_sam = make_unit(
	19,
	attacker_owner,
	water_tile,
	embarked_sam_def,
	'land'
);
embarked_sam.transport_id = 50;
test.assert(combat_rules.can_attack_target(embarked_sam, bomber));
sam.transport_id = 0;
test.assert(
	combat_rules.get_combat_powers(sam, bomber).attack ==
		combat_rules.get_combat_powers(rifle, bomber).attack
);
test.assert(
	combat_rules.get_combat_powers(sam, bomber).defence ==
		1.0 * combat_rules.get_morale_multiplier(bomber) * bomber.health
);

const interceptor_def = make_def(
	'InterceptorNeedlejet',
	['AirSuperiority'],
	'air',
	'Needlejet'
);
const interceptor = make_unit(10, attacker_owner, land_tile, interceptor_def, 'air');
test.assert(
	combat_rules.get_combat_powers(interceptor, bomber).attack ==
		combat_rules.get_combat_powers(rifle, bomber).attack * 2.0
);
test.assert(
	combat_rules.get_combat_powers(interceptor, bomber).defence ==
		6.0 * combat_rules.get_morale_multiplier(bomber) * bomber.health
);
test.assert(
	combat_rules.get_combat_powers(interceptor, land_defender).attack ==
		combat_rules.get_combat_powers(rifle, land_defender).attack * 0.5
);
const copter = make_unit(11, defender_owner, target_land, copter_def, 'air');
test.assert(!combat_rules.is_air_unit_in_flight(copter));
test.assert(combat_rules.can_attack_target(rifle, copter));
target_land.terraforming.airbase = true;
test.assert(!combat_rules.is_air_unit_in_flight(bomber));
test.assert(combat_rules.can_attack_target(rifle, bomber));
target_land.terraforming.airbase = false;

const validate_game = {
	is_turn_complete: (player_id) => { return false; },
	tm: {get_distance: (from, to) => { return 1; }},
};
ordinary.get_tile = () => { return water_tile; };
ordinary.transport_id = 50;
test.assert(
	attack_unit.validate({
		caller: attacker_owner.id,
		game: validate_game,
		data: {attacker: ordinary, defender: land_defender},
	}) == 'Only units with Amphibious Pods can attack from a transport'
);
test.assert(!#is_defined(attack_unit.validate({
	caller: attacker_owner.id,
	game: validate_game,
	data: {attacker: amphibious, defender: land_defender},
})));
test.assert(
	attack_unit.validate({
		caller: attacker_owner.id,
		game: validate_game,
		data: {attacker: rifle, defender: bomber},
	}) == 'Only units with Air Superiority can attack air units in flight'
);
test.assert(!#is_defined(attack_unit.validate({
	caller: attacker_owner.id,
	game: validate_game,
	data: {attacker: sam, defender: bomber},
})));
sam.transport_id = 50;
test.assert(
	attack_unit.validate({
		caller: attacker_owner.id,
		game: validate_game,
		data: {attacker: sam, defender: bomber},
	}) == 'Only units with Amphibious Pods can attack from a transport'
);
sam.transport_id = 0;

const movement_game = {
	is_turn_complete: (player_id) => { return false; },
	get: (name) => { return #undefined; },
};
const sea_base_entry = make_unit(12, attacker_owner, target_land, ordinary_def, 'land');
sea_base_entry.get_tile = () => { return target_land; };
test.assert(#is_defined(move_unit.validate({
	caller: attacker_owner.id,
	game: movement_game,
	data: {unit: sea_base_entry, tile: sea_base_tile},
})));
const amphibious_entry = make_unit(13, attacker_owner, target_land, amphibious_def, 'land');
test.assert(!#is_defined(move_unit.validate({
	caller: attacker_owner.id,
	game: movement_game,
	data: {unit: amphibious_entry, tile: sea_base_tile},
})));
const entry_resolution = move_unit.resolve({
	game: movement_game,
	data: {unit: amphibious_entry, tile: sea_base_tile},
});
test.assert(entry_resolution.is_movement_successful && entry_resolution.transport_id == 0);

const sea_base_source = make_tile(false);
make_base(9, attacker_owner, sea_base_source);
sea_base_source.set_surrounding_tiles([target_land]);
const ordinary_exit = make_unit(14, attacker_owner, sea_base_source, ordinary_def, 'land');
test.assert(#is_defined(move_unit.validate({
	caller: attacker_owner.id,
	game: movement_game,
	data: {unit: ordinary_exit, tile: target_land},
})));
const amphibious_exit = make_unit(15, attacker_owner, sea_base_source, amphibious_def, 'land');
test.assert(!#is_defined(move_unit.validate({
	caller: attacker_owner.id,
	game: movement_game,
	data: {unit: amphibious_exit, tile: target_land},
})));

let current_tile = water_tile;
let transport_id = 50;
let stopped_animation = 0;
let embark_calls = 0;
let advance_event = null;
const transport = {id: 50, owner: attacker_owner.id, get_tile: () => { return water_tile; }};
let assault_unit = null;
assault_unit = {
	id: 16,
	owner: attacker_owner.id,
	health: 1.0,
	is_land: true,
	is_water: false,
	transport_id: transport_id,
	get_def: () => { return amphibious_def; },
	get_owner: () => { return attacker_owner; },
	get_tile: () => { return current_tile; },
	disembark: () => {
		transport_id = 0;
		advance_event.data.unit.transport_id = 0;
	},
	embark: (carrier) => {
		embark_calls++;
		test.assert(carrier == transport && current_tile == water_tile);
		transport_id = carrier.id;
		advance_event.data.unit.transport_id = carrier.id;
	},
	move_to_tile: (tile, oncomplete) => { current_tile = tile; oncomplete(); },
};
advance_event = {
	caller: 0,
	game: {
		am: {stop_animations: (id) => { stopped_animation = id; }},
		um: {get_unit: (id) => { test.assert(id == transport.id); return transport; }},
	},
	data: {unit: assault_unit, tile: target_land, animations_id: 77},
};
test.assert(!#is_defined(advance_unit_after_combat.validate(advance_event)));
advance_event.applied = advance_unit_after_combat.apply(advance_event);
test.assert(stopped_animation == 77);
test.assert(current_tile == target_land && advance_event.data.unit.transport_id == 0);
test.assert(advance_event.applied.orig_transport_id == transport.id);
advance_unit_after_combat.rollback(advance_event);
test.assert(
	current_tile == water_tile &&
	advance_event.data.unit.transport_id == transport.id &&
	embark_calls == 1
);
