const artifact_rules = #include('../default/game/artifact_rules');
const study_alien_artifact = #include('../default/game/event/study_alien_artifact');

const clone_state = (value) => {
	let technologies = [];
	for (id of value.technologies) {
		technologies :+id;
	}
	return {
		technologies: technologies,
		target: value.target,
		progress: value.progress,
	};
};

let state = {technologies: [], target: 'Alpha', progress: 7};
const player = {
	id: 1,
	name: 'The University',
	get_research_state: () => { return clone_state(state); },
	set_research_state: (value) => { state = clone_state(value); },
};
let has_node = true;
let has_translator = false;
let custom = {};
const base = {
	get_owner: () => { return player; },
	has_facility: (id) => {
		return (id == 'NetworkNode' && has_node) ||
			(id == 'TheUniversalTranslator' && has_translator);
	},
	has: (key) => { return #is_defined(custom[key]); },
	get: (key) => { return custom[key]; },
	set: (key, value) => { custom[key] = value; },
	unset: (key) => { custom[key] = #undefined; },
};
const tile = {
	x: 3,
	y: 5,
	get_base: () => { return base; },
};
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

const definitions = {
	Alpha: {id: 'Alpha', name: 'Alpha'},
	Beta: {id: 'Beta', name: 'Beta'},
	Gamma: {id: 'Gamma', name: 'Gamma'},
};
const order = ['Alpha', 'Beta', 'Gamma'];
let turn_complete = false;
let despawned = null;
let spawned_data = null;
let restored_unit = null;
let triggers = [];
let messages = [];
let datalinks_queues = 0;
const game = {
	is_turn_complete: (id) => { return turn_complete; },
	get: (key) => {
		if (key == 'f_technology_get_next_target') {
			return (known, owner) => {
				for (id of order) {
					let found = false;
					for (known_id of known) {
						if (known_id == id) {
							found = true;
							break;
						}
					}
					if (!found) {
						return id;
					}
				}
				return '';
			};
		}
		if (key == 'f_technology_get_definition') {
			return (id) => { return #is_defined(definitions[id]) ? definitions[id] : null; };
		}
		if (key == 'f_project_queue_planetary_datalinks') {
			return () => { datalinks_queues++; };
		}
		return #undefined;
	},
	trigger: (name, data) => { triggers :+{name: name, data: data}; },
	message: (value) => { messages :+value; },
	get_player: (id) => { return player; },
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

let artifact = make_artifact(7);
let event = {caller: player.id, game: game, data: {unit: artifact}};
test.assert(!#is_defined(study_alien_artifact.validate(event)));
test.assert(artifact_rules.get_study_method(base) == 'network_node');
event.applied = study_alien_artifact.apply(event);
test.assert(event.applied.method == 'network_node');
test.assert(custom.network_node_artifact_linked);
test.assert(state == {
	technologies: ['Alpha'], target: 'Beta', progress: 7,
});
test.assert(despawned == artifact);
test.assert(messages == ['The University has decoded Alpha from an Alien Artifact.']);
test.assert(datalinks_queues == 1);
test.assert(#is_defined(study_alien_artifact.validate(event)));

study_alien_artifact.rollback(event);
test.assert(!#is_defined(custom.network_node_artifact_linked));
test.assert(state == {technologies: [], target: 'Alpha', progress: 7});
test.assert(spawned_data.id == artifact.id);
test.assert(spawned_data.def == artifact.def);
test.assert(spawned_data.owner == player);
test.assert(spawned_data.tile == tile);
test.assert(restored_unit.movement == artifact.movement);
test.assert(restored_unit.moved_this_turn == artifact.moved_this_turn);
test.assert(#sizeof(triggers) == 2);

has_translator = true;
custom.network_node_artifact_linked = true;
artifact = make_artifact(8);
event = {caller: player.id, game: game, data: {unit: artifact}};
test.assert(artifact_rules.get_study_method(base) == 'universal_translator');
test.assert(!#is_defined(study_alien_artifact.validate(event)));
event.applied = study_alien_artifact.apply(event);
test.assert(event.applied.method == 'universal_translator');
test.assert(custom.network_node_artifact_linked);
test.assert(state.target == 'Beta');

const second_artifact = make_artifact(9);
const second_event = {caller: player.id, game: game, data: {unit: second_artifact}};
test.assert(!#is_defined(study_alien_artifact.validate(second_event)));
second_event.applied = study_alien_artifact.apply(second_event);
test.assert(state == {
	technologies: ['Alpha', 'Beta'], target: 'Gamma', progress: 7,
});
study_alien_artifact.rollback(second_event);
study_alien_artifact.rollback(event);
test.assert(state == {technologies: [], target: 'Alpha', progress: 7});
test.assert(custom.network_node_artifact_linked);

has_translator = false;
test.assert(#is_defined(study_alien_artifact.validate({
	caller: player.id,
	game: game,
	data: {unit: make_artifact(10)},
})));
custom.network_node_artifact_linked = #undefined;
has_node = false;
test.assert(#is_defined(study_alien_artifact.validate({
	caller: player.id,
	game: game,
	data: {unit: make_artifact(11)},
})));
has_node = true;
turn_complete = true;
test.assert(#is_defined(study_alien_artifact.validate({
	caller: player.id,
	game: game,
	data: {unit: make_artifact(12)},
})));
turn_complete = false;
state = {technologies: ['Alpha', 'Beta', 'Gamma'], target: '', progress: 0};
test.assert(#is_defined(study_alien_artifact.validate({
	caller: player.id,
	game: game,
	data: {unit: make_artifact(13)},
})));
