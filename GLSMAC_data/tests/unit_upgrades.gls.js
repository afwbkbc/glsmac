const rules = #include('../default/game/unit_upgrade_rules');
const upgrade_unit = #include('../default/game/event/upgrade_unit');

const make_def = (
	id,
	name,
	mineral_cost,
	chassis,
	weapon,
	armor,
	offense,
	defense,
	required_technology,
	abilities
) => {
	return {
		id: id,
		name: name,
		mineral_cost: mineral_cost,
		chassis: chassis,
		weapon: weapon,
		armor: armor,
		reactor: 'FissionPlant',
		reactor_power: 1,
		offense: offense,
		defense: defense,
		required_technology: required_technology,
		abilities: abilities,
		is_native: false,
		movement_per_turn: chassis == 'Speeder' ? 2.0 : 1.0,
		operational_range: chassis == 'Needlejet' ? 2 : 0,
	};
};

const scout = make_def(
	'ScoutPatrol', 'Scout Patrol', 10, 'Infantry', 'HandWeapons', 'NoArmor',
	1, 1, '', []
);
const laser = make_def(
	'LaserInfantry', 'Laser Infantry', 20, 'Infantry', 'Laser', 'NoArmor',
	2, 1, 'AppliedPhysics', []
);
const synth = make_def(
	'SynthmetalSentinels', 'Synthmetal Sentinels', 20,
	'Infantry', 'HandWeapons', 'SynthmetalArmor', 1, 2, 'IndustrialBase', []
);
const laser_synth = make_def(
	'LaserSynth', 'Laser Synthmetal Infantry', 30,
	'Infantry', 'Laser', 'SynthmetalArmor', 4, 2, 'IndustrialBase', []
);
const rover = make_def(
	'ReconRover', 'Recon Rover', 20, 'Speeder', 'HandWeapons', 'NoArmor',
	1, 1, 'DoctrineMobility', []
);
const former = make_def(
	'Former', 'Former', 20, 'Infantry', 'TerraformingUnit', 'NoArmor',
	0, 1, 'CentauriEcology', []
);
const locked = make_def(
	'LockedLaser', 'Locked Laser Infantry', 20,
	'Infantry', 'Laser', 'NoArmor', 2, 1, 'Superconductor', []
);
const unprototyped = make_def(
	'ImpactInfantry', 'Impact Infantry', 30,
	'Infantry', 'ParticleImpactor', 'NoArmor', 4, 1, 'NonlinearMathematics', []
);
const artillery = make_def(
	'Artillery', 'Artillery', 30, 'Infantry', 'Laser', 'NoArmor',
	2, 1, 'AppliedPhysics', ['HeavyArtillery']
);
const needlejet = make_def(
	'Needlejet', 'Needlejet', 40, 'Needlejet', 'Laser', 'NoArmor',
	2, 1, 'DoctrineAirPower', ['AirSuperiority']
);
const needlejet_without_superiority = make_def(
	'NeedlejetGroundAttack', 'Ground Attack Needlejet', 40,
	'Needlejet', 'Laser', 'NoArmor', 2, 1, 'DoctrineAirPower', []
);

const definitions = [
	scout, laser, synth, laser_synth, rover, former, locked, unprototyped,
	artillery, needlejet, needlejet_without_superiority,
];
let known = [
	'AppliedPhysics', 'IndustrialBase', 'DoctrineMobility', 'CentauriEcology',
	'NonlinearMathematics', 'DoctrineAirPower',
];
let components = [
	'HandWeapons', 'Infantry', 'Laser', 'Needlejet', 'NoArmor', 'Speeder',
	'SynthmetalArmor', 'TerraformingUnit',
];
let nano_factory = false;
let player = {
	id: 1,
	name: 'Test Faction',
	energy_credits: 100,
	has_technology: (id) => {
		for (technology of known) {
			if (technology == id) { return true; }
		}
		return false;
	},
	has_prototyped_component: (id) => {
		for (component of components) {
			if (component == id) { return true; }
		}
		return false;
	},
};
player.set_energy_credits = (value) => { player.energy_credits = value; };

const tile = {x: 4, y: 6};
let current_unit = null;
const make_unit = (data) => {
	const def = rules.find_definition(game, data.def);
	let unit = {
		id: data.id,
		def: data.def,
		owner: data.owner.id,
		movement: #is_defined(data.movement) ? data.movement : def.movement_per_turn,
		morale: data.morale,
		health: data.health,
		moved_this_turn: #is_defined(data.moved_this_turn) ? data.moved_this_turn : false,
		terraforming: #is_defined(data.terraforming) ? data.terraforming : 'none',
		terraforming_turns_remaining: #is_defined(data.terraforming_turns_remaining)
			? data.terraforming_turns_remaining : 0,
		home_base_id: #is_defined(data.home_base_id) ? data.home_base_id : 0,
		fuel: #is_defined(data.fuel) ? data.fuel : def.operational_range,
		transport_id: #is_defined(data.transport_id) ? data.transport_id : 0,
	};
	unit.get_def = () => { return rules.find_definition(game, unit.def); };
	unit.get_tile = () => { return tile; };
	unit.get_cargo = () => { return []; };
	return unit;
};

const triggers = [];
let message = '';
const um = {
	get_unit_defs: () => { return definitions; },
	spawn_unit: (data) => {
		current_unit = make_unit(data);
		return current_unit;
	},
	despawn_unit: (unit) => { current_unit = null; },
	has_unit: (id) => { return current_unit != null && current_unit.id == id; },
	get_unit: (id) => { return current_unit; },
};
const game = {
	um: um,
	tm: {get_tile: (x, y) => { return tile; }},
	get_um: () => { return um; },
	get_player: (id) => { return player; },
	is_turn_complete: (id) => { return false; },
	get: (key) => {
		if (key == 'f_project_get_player_effects') {
			return (target) => {
				return {unit_upgrade_cost_multiplier: nano_factory ? 0.5 : 1.0};
			};
		}
		return #undefined;
	},
	trigger: (name, data) => { triggers :+name; },
	message: (value) => { message = value; },
};

test.assert(rules.is_compatible(scout, laser));
test.assert(!rules.is_compatible(laser, scout));
test.assert(!rules.is_compatible(scout, rover));
test.assert(!rules.is_compatible(former, scout));
test.assert(!rules.is_compatible(artillery, laser));
test.assert(!rules.is_compatible(needlejet, needlejet_without_superiority));
test.assert(rules.get_targets(game, player, scout) == [laser, synth, laser_synth]);
test.assert(!rules.is_available(player, locked));
test.assert(!rules.is_available(player, unprototyped));

test.assert(rules.get_cost(game, player, scout, laser_synth) == 50);
nano_factory = true;
test.assert(rules.get_cost(game, player, scout, laser_synth) == 20);
nano_factory = false;

current_unit = make_unit({
	id: 7,
	def: scout.id,
	owner: player,
	morale: 4,
	health: 0.7,
	home_base_id: 3,
	fuel: 0,
});
let event = {
	caller: player.id,
	game: game,
	data: {unit: current_unit, target_def_id: laser_synth.id},
};
test.assert(!#is_defined(upgrade_unit.validate(event)));
event.resolved = upgrade_unit.resolve(event);
test.assert(event.resolved == {target_def_id: laser_synth.id, cost: 50});
event.applied = upgrade_unit.apply(event);
test.assert(current_unit.id == 7);
test.assert(current_unit.def == laser_synth.id);
test.assert(current_unit.morale == 4);
test.assert(current_unit.health == 0.7);
test.assert(current_unit.home_base_id == 3);
test.assert(current_unit.movement == 0.0);
test.assert(current_unit.moved_this_turn);
test.assert(player.energy_credits == 50);
test.assert(#sizeof(triggers) == 2);
test.assert(
	message == 'Test Faction upgraded Scout Patrol to Laser Synthmetal Infantry for ' +
		'50 energy credits.'
);

upgrade_unit.rollback(event);
test.assert(current_unit.id == 7);
test.assert(current_unit.def == scout.id);
test.assert(current_unit.movement == 1.0);
test.assert(!current_unit.moved_this_turn);
test.assert(current_unit.morale == 4);
test.assert(current_unit.health == 0.7);
test.assert(player.energy_credits == 100);
test.assert(#sizeof(triggers) == 4);

current_unit.moved_this_turn = true;
test.assert(
	upgrade_unit.validate({
		caller: player.id,
		game: game,
		data: {unit: current_unit, target_def_id: laser.id},
	}) == 'Unit must not have used movement this turn'
);
current_unit.moved_this_turn = false;
current_unit.get_cargo = () => { return [{id: 99}]; };
test.assert(
	upgrade_unit.validate({
		caller: player.id,
		game: game,
		data: {unit: current_unit, target_def_id: laser.id},
	}) == 'A transport carrying units cannot be upgraded'
);
current_unit.get_cargo = () => { return []; };
test.assert(
	rules.get_error(game, current_unit, player.id, unprototyped.id) ==
		'Target design must be prototyped before units can upgrade to it'
);
test.assert(
	rules.get_error(game, current_unit, player.id, locked.id) ==
		'Target design technology is not available'
);

test.assert(rules.choose_ai_target(game, player, current_unit) == laser_synth);
player.energy_credits = 55;
test.assert(rules.choose_ai_target(game, player, current_unit) == laser);
player.energy_credits = 40;
test.assert(rules.choose_ai_target(game, player, current_unit) == null);
