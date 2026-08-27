const define_projects = #include('../default/game/projects');

const clone_state = (state) => {
	let technologies = [];
	for (id of state.technologies) {
		technologies :+id;
	}
	return {
		technologies: technologies,
		target: state.target,
		progress: state.progress,
	};
};

let state = {technologies: [], target: 'Alpha', progress: 7};
const owner = {
	id: 1,
	name: 'The University',
	get_research_state: () => { return clone_state(state); },
	set_research_state: (updated) => { state = clone_state(updated); },
	has_technology: (id) => {
		for (known of state.technologies) {
			if (known == id) {
				return true;
			}
		}
		return false;
	},
};
const translator = {id: 'TheUniversalTranslator', is_project: true};
const base = {
	get_owner: () => { return owner; },
	get_facilities: () => { return [translator]; },
};
const order = ['Alpha', 'Beta', 'Gamma'];
const definitions = {
	Alpha: {id: 'Alpha', name: 'Alpha'},
	Beta: {id: 'Beta', name: 'Beta'},
	Gamma: {id: 'Gamma', name: 'Gamma'},
};
const values = {
	f_technology_get_order: () => { return order; },
	f_technology_get_definition: (id) => { return definitions[id]; },
	f_technology_get_next_target: (known, player) => {
		test.assert(player == owner);
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
	},
};
let triggers = [];
let messages = [];
let datalinks_queues = 0;
const game = {
	get_bm: () => { return {get_bases: () => { return [base]; }}; },
	get_players: () => { return [owner]; },
	get: (key) => { return values[key]; },
	set: (key, value) => { values[key] = value; },
	is_master: () => { return true; },
	event: (name, data) => {},
	trigger: (name, data) => { triggers :+{name: name, data: data}; },
	message: (message) => { messages :+message; },
	on: (name, callback) => {},
};

define_projects(game);
values.f_project_queue_planetary_datalinks = () => {
	datalinks_queues++;
	return true;
};

let applied = values.f_project_apply_completion_effects(base, translator.id);
test.assert(applied.completed_count == 2);
test.assert(state == {
	technologies: ['Alpha', 'Beta'],
	target: 'Gamma',
	progress: 7,
});
test.assert(triggers == [{name: 'research_updated', data: {player: owner}}]);
test.assert(messages == [
	'The University has acquired Alpha through The Universal Translator.',
	'The University has acquired Beta through The Universal Translator.',
]);
test.assert(datalinks_queues == 1);

values.f_project_rollback_completion_effects(applied);
test.assert(state == {technologies: [], target: 'Alpha', progress: 7});
test.assert(#sizeof(triggers) == 2);

owner.set_research_state({
	technologies: ['Alpha', 'Beta'],
	target: 'Gamma',
	progress: 9,
});
triggers = [];
messages = [];
datalinks_queues = 0;
applied = values.f_project_apply_completion_effects(base, translator.id);
test.assert(applied.completed_count == 1);
test.assert(state == {
	technologies: ['Alpha', 'Beta', 'Gamma'],
	target: '',
	progress: 0,
});
test.assert(#sizeof(messages) == 1);
test.assert(#sizeof(triggers) == 1);
test.assert(datalinks_queues == 1);
values.f_project_rollback_completion_effects(applied);
test.assert(state == {
	technologies: ['Alpha', 'Beta'],
	target: 'Gamma',
	progress: 9,
});

owner.set_research_state({
	technologies: ['Alpha', 'Beta', 'Gamma'],
	target: '',
	progress: 0,
});
triggers = [];
messages = [];
datalinks_queues = 0;
test.assert(!#is_defined(
	values.f_project_apply_completion_effects(base, translator.id)
));
test.assert(!#is_defined(
	values.f_project_apply_completion_effects(base, 'TheHumanGenomeProject')
));
test.assert(triggers == []);
test.assert(messages == []);
test.assert(datalinks_queues == 0);
