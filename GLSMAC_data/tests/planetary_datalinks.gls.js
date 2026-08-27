const define_projects = #include('../default/game/projects');
const process_planetary_datalinks = #include(
	'../default/game/event/process_planetary_datalinks'
);

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

const make_player = (id, technologies, target, progress) => {
	let state = {
		technologies: technologies,
		target: target,
		progress: progress,
	};
	const player = {
		id: id,
		name: 'Faction ' + #to_string(id),
		get_research_state: () => { return clone_state(state); },
		set_research_state: (updated) => { state = clone_state(updated); },
		has_technology: (technology_id) => {
			for (known_id of state.technologies) {
				if (known_id == technology_id) {
					return true;
				}
			}
			return false;
		},
	};
	return player;
};

const owner = make_player(1, [], 'Alpha', 7);
const rival_two = make_player(2, ['Alpha'], 'Beta', 0);
const rival_three = make_player(3, ['Alpha'], 'Beta', 0);
const rival_four = make_player(4, [], 'Alpha', 0);
const outsider = make_player(5, [], 'Alpha', 0);
const players = [owner, rival_two, rival_three, rival_four, outsider];
const datalinks = {id: 'ThePlanetaryDatalinks', is_project: true};
const bases = [
	{get_owner: () => { return owner; }, get_facilities: () => { return [datalinks]; }},
	{get_owner: () => { return outsider; }, get_facilities: () => { return []; }},
];
const definitions = {
	Alpha: {id: 'Alpha', name: 'Alpha'},
	Beta: {id: 'Beta', name: 'Beta'},
};
const values = {
	f_technology_get_order: () => { return ['Alpha', 'Beta']; },
	f_technology_get_definition: (id) => { return definitions[id]; },
	f_technology_get_next_target: (known, player) => {
		for (id of ['Alpha', 'Beta']) {
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
let master = true;
let queued_events = [];
let triggers = [];
let messages = [];
let turn_callback = null;
const game = {
	get_bm: () => { return {get_bases: () => { return bases; }}; },
	get_players: () => { return players; },
	get: (key) => { return values[key]; },
	set: (key, value) => { values[key] = value; },
	is_master: () => { return master; },
	event: (name, data) => { queued_events :+{name: name, data: data}; },
	trigger: (name, data) => { triggers :+{name: name, data: data}; },
	message: (message) => { messages :+message; },
	on: (name, callback) => {
		if (name == 'turn') {
			turn_callback = callback;
		}
	},
};

define_projects(game);

test.assert(#is_defined(turn_callback));
test.assert(values.f_project_get_planetary_datalinks_candidates(owner) == []);
test.assert(!values.f_project_queue_planetary_datalinks());
test.assert(queued_events == []);

rival_four.set_research_state({technologies: ['Alpha'], target: 'Beta', progress: 0});
test.assert(values.f_project_get_planetary_datalinks_candidates(owner) == ['Alpha']);
test.assert(values.f_project_get_planetary_datalinks_candidates(outsider) == ['Alpha']);
test.assert(values.f_project_queue_planetary_datalinks());
test.assert(queued_events == [{name: 'process_planetary_datalinks', data: {}}]);
test.assert(!values.f_project_queue_planetary_datalinks());

let event = {caller: 1, game: game, data: {}};
test.assert(#is_defined(process_planetary_datalinks.validate(event)));
event.caller = 0;
test.assert(!#is_defined(process_planetary_datalinks.validate(event)));
event.applied = process_planetary_datalinks.apply(event);
test.assert(owner.get_research_state() == {
	technologies: ['Alpha'],
	target: 'Beta',
	progress: 7,
});
test.assert(!outsider.has_technology('Alpha'));
test.assert(#sizeof(event.applied.players) == 1);
test.assert(triggers == [{name: 'research_updated', data: {player: owner}}]);
test.assert(messages == [
	'Faction 1 has acquired Alpha through The Planetary Datalinks.',
]);

process_planetary_datalinks.rollback(event);
test.assert(owner.get_research_state() == {
	technologies: [],
	target: 'Alpha',
	progress: 7,
});
test.assert(#sizeof(triggers) == 2);
test.assert(values.f_project_queue_planetary_datalinks());

master = false;
for (rival of [rival_two, rival_three, rival_four]) {
	rival.set_research_state({
		technologies: ['Alpha', 'Beta'], target: '', progress: 0,
	});
}
event.applied = process_planetary_datalinks.apply(event);
test.assert(owner.get_research_state() == {
	technologies: ['Alpha', 'Beta'], target: '', progress: 0,
});
test.assert(!values.f_project_queue_planetary_datalinks());
process_planetary_datalinks.rollback(event);
test.assert(owner.get_research_state() == {
	technologies: [], target: 'Alpha', progress: 7,
});
