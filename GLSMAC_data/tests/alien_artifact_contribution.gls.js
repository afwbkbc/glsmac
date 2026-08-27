const artifact_rules = #include('../default/game/artifact_rules');
const contribute = #include('../default/game/event/contribute_alien_artifact');

let production = {
	id: 'TheWeatherParadigm',
	name: 'The Weather Paradigm',
	production_kind: 'project',
	mineral_cost: 200,
};
let minerals = 17;
let custom = {};
const player = {
	id: 1,
	name: 'The University',
	has_prototyped_component: (id) => { return false; },
};
const base = {
	id: 4,
	get_owner: () => { return player; },
	get_production: () => { return production; },
	get_accumulated_minerals: () => { return minerals; },
	set_accumulated_minerals: (value) => { minerals = value; },
	has_facility: (id) => { return false; },
	has: (key) => { return #is_defined(custom[key]); },
	get: (key) => { return custom[key]; },
	set: (key, value) => { custom[key] = value; },
	unset: (key) => { custom[key] = #undefined; },
};
const tile = {x: 3, y: 5, get_base: () => { return base; }};
const artifact_def = {weapon: 'AlienArtifact'};
const make_artifact = (id) => {
	return {
		id: id,
		def: 'AlienArtifact',
		owner: player.id,
		movement: 1.0,
		morale: 2,
		health: 1.0,
		moved_this_turn: false,
		terraforming: 'none',
		terraforming_turns_remaining: 0,
		home_base_id: base.id,
		fuel: 0,
		transport_id: 0,
		get_def: () => { return artifact_def; },
		get_tile: () => { return tile; },
	};
};

let turn_complete = false;
let despawned = null;
let spawned_data = null;
let restored_unit = null;
let messages = [];
const game = {
	is_turn_complete: (id) => { return turn_complete; },
	get_player: (id) => { return player; },
	message: (value) => { messages :+value; },
	um: {
		despawn_unit: (unit) => { despawned = unit; },
		spawn_unit: (data) => {
			spawned_data = data;
			restored_unit = {movement: 0.0, moved_this_turn: true};
			return restored_unit;
		},
	},
	tm: {get_tile: (x, y) => { return tile; }},
};

let artifact = make_artifact(9);
let event = {caller: player.id, game: game, data: {unit: artifact}};
test.assert(artifact_rules.contribution_minerals == 50);
test.assert(artifact_rules.get_contribution_target(base).kind == 'project');
test.assert(!#is_defined(contribute.validate(event)));
event.applied = contribute.apply(event);
test.assert(event.applied.target_kind == 'project');
test.assert(minerals == 67);
test.assert(despawned == artifact);
test.assert(messages == [
	'The University has applied an Alien Artifact to The Weather Paradigm.',
]);

contribute.rollback(event);
test.assert(minerals == 17);
test.assert(spawned_data.id == artifact.id);
test.assert(spawned_data.owner == player);
test.assert(spawned_data.tile == tile);
test.assert(restored_unit.movement == artifact.movement);
test.assert(restored_unit.moved_this_turn == artifact.moved_this_turn);

production = {
	id: 'PrototypeNeedlejet',
	name: 'Prototype Needlejet',
	production_kind: 'unit',
	chassis: 'Needlejet',
	weapon: 'MissileLauncher',
	armor: 'SynthmetalArmor',
};
artifact = make_artifact(10);
event = {caller: player.id, game: game, data: {unit: artifact}};
test.assert(artifact_rules.get_contribution_target(base).kind == 'prototype');
test.assert(!#is_defined(contribute.validate(event)));

player.has_prototyped_component = (id) => { return true; };
test.assert(artifact_rules.get_contribution_target(base) == null);
test.assert(#is_defined(contribute.validate(event)));

production = {
	id: 'ScoutPatrol',
	name: 'Scout Patrol',
	production_kind: 'unit',
	chassis: 'Infantry',
	weapon: 'HandWeapons',
	armor: 'NoArmor',
};
test.assert(artifact_rules.get_contribution_target(base) == null);

production = #undefined;
test.assert(artifact_rules.get_contribution_target(base) == null);
turn_complete = true;
test.assert(#is_defined(contribute.validate(event)));
