const define_projects = #include('../default/game/projects');
const base_capture = #include('../default/game/base_capture');

const make_player = (id, name) => {
	let infiltrated = {};
	return {
		id: id,
		name: name,
		has_infiltrated: (other) => {
			const key = 'p' + #to_string(other.id);
			return #is_defined(infiltrated[key]) && infiltrated[key];
		},
		set_infiltrated: (other, value) => {
			infiltrated['p' + #to_string(other.id)] = value;
		},
	};
};

const attacker = make_player(1, 'The Gaians');
const defender = make_player(2, 'The Hive');
const third_party = make_player(3, 'The University');
const players = [attacker, defender, third_party];
attacker.set_infiltrated(third_party, true);

let current_owner = attacker;
let production_queue = [];
const empath_guild = {id: 'TheEmpathGuild', is_project: true};
const base = {
	id: 7,
	get_owner: () => { return current_owner; },
	set_owner: (owner) => { current_owner = owner; },
	has_facility: (id) => { return id == empath_guild.id; },
	get_facilities: () => { return [empath_guild]; },
	get_production_queue: () => { return production_queue; },
	can_produce: (kind, id) => { return true; },
	set_production_queue: (queue) => { production_queue = queue; },
};
const values = {};
let messages = [];
const game = {
	get_players: () => { return players; },
	get_bm: () => { return game.bm; },
	get: (key) => { return values[key]; },
	set: (key, value) => { values[key] = value; },
	is_master: () => { return true; },
	event: (name, data) => {},
	trigger: (name, data) => {},
	message: (message) => { messages :+message; },
	on: (name, callback) => {},
	bm: {get_bases: () => { return [base]; }},
	um: {get_units: () => { return []; }},
	tm: {get_distance: (source, destination) => { return 0; }},
};

define_projects(game);

let applied = values.f_project_apply_completion_effects(base, empath_guild.id);
test.assert(applied.kind == 'empath_guild');
test.assert(#sizeof(applied.applied.infiltrated_players) == 1);
test.assert(applied.applied.infiltrated_players[0] == defender);
test.assert(attacker.has_infiltrated(defender));
test.assert(attacker.has_infiltrated(third_party));
test.assert(messages == [
	'The Gaians has infiltrated every faction through The Empath Guild.',
]);

values.f_project_rollback_completion_effects(applied);
test.assert(!attacker.has_infiltrated(defender));
test.assert(attacker.has_infiltrated(third_party));

current_owner = defender;
const capture = base_capture.capture_base(game, base, attacker);
test.assert(current_owner == attacker);
test.assert(attacker.has_infiltrated(defender));
test.assert(attacker.has_infiltrated(third_party));
test.assert(#is_defined(capture.empath_guild_infiltration));
test.assert(capture.empath_guild_infiltration.infiltrated_players == [defender]);

base_capture.restore_base(base, capture);
test.assert(current_owner == defender);
test.assert(!attacker.has_infiltrated(defender));
test.assert(attacker.has_infiltrated(third_party));

current_owner = attacker;
attacker.set_infiltrated(defender, true);
messages = [];
test.assert(!#is_defined(
	values.f_project_apply_completion_effects(base, empath_guild.id)
));
test.assert(messages == []);
