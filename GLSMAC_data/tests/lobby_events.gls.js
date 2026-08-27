const ready_or_not = #include('../default/game/event/ready_or_not');

let is_ready = true;
let set_ready_calls = 0;
let game_started = false;

const player = {
	is_ready: () => {
		return is_ready;
	},
	set_ready: (value) => {
		is_ready = value;
		set_ready_calls++;
	},
};

const game = {
	is_started: () => {
		return game_started;
	},
	get_player: (id) => {
		return player;
	},
};

let event = {
	caller: 1,
	game: game,
	data: {
		ready: true,
	},
};

test.assert(!#is_defined(ready_or_not.validate(event)));
event.data.ready = 1;
test.assert(#is_defined(ready_or_not.validate(event)));
event.data.ready = true;
game_started = true;
test.assert(#is_defined(ready_or_not.validate(event)));
game_started = false;

event.applied = ready_or_not.apply(event);
test.assert(event.applied.was_ready == true);
test.assert(is_ready == true);
test.assert(set_ready_calls == 0);

is_ready = false;
ready_or_not.rollback(event);
test.assert(is_ready == true);
test.assert(set_ready_calls == 1);

is_ready = false;
set_ready_calls = 0;
event = {
	caller: 1,
	game: game,
	data: {
		ready: true,
	},
};

event.applied = ready_or_not.apply(event);
test.assert(event.applied.was_ready == false);
test.assert(is_ready == true);
test.assert(set_ready_calls == 1);

ready_or_not.rollback(event);
test.assert(is_ready == false);
test.assert(set_ready_calls == 2);

const game_settings = #include('../default/game/event/game_settings');

let player_ready = [true, false];
const players = [
	{
		id: 0,
		is_ready: () => {
			return player_ready[0];
		},
		set_ready: (value) => {
			player_ready[0] = value;
		},
	},
	{
		id: 1,
		is_ready: () => {
			return player_ready[1];
		},
		set_ready: (value) => {
			player_ready[1] = value;
		},
	},
];
const settings = {
	global: {
		map: {
			size_x: 40,
			size_y: 20,
		},
	},
};
let triggered_settings = null;
const settings_game = {
	get_settings: () => {
		return settings;
	},
	get_players: () => {
		return players;
	},
	get_player: (id) => {
		return players[id];
	},
	trigger: (name, data) => {
		test.assert(name == 'game_settings');
		triggered_settings = data.settings;
	},
};
const settings_event = {
	caller: 0,
	game: settings_game,
	data: {
		changes: [
			['planet_size', '64x32'],
		],
	},
};

const validation_game = {
	is_started: () => {
		return false;
	},
};
const validate_change = (change) => {
	return game_settings.validate({
		caller: 0,
		game: validation_game,
		data: {
			changes: [change],
		},
	});
};

test.assert(!#is_defined(validate_change(['planet_size', '64x32'])));
test.assert(!#is_defined(validate_change(['ocean_coverage', 0.6])));
test.assert(#is_defined(validate_change(['planet_size', '64'])));
test.assert(#is_defined(validate_change(['planet_size', '3x4'])));
test.assert(#is_defined(validate_change(['planet_size', '5x4'])));
test.assert(#is_defined(validate_change(['planet_size', '180x92'])));
test.assert(#is_defined(validate_change(['ocean_coverage', 2.0])));
test.assert(#is_defined(validate_change(['unknown', 0.5])));

settings_event.applied = game_settings.apply(settings_event);
test.assert(settings.global.map.size_x == 64);
test.assert(settings.global.map.size_y == 32);
test.assert(player_ready[0] == false);
test.assert(player_ready[1] == false);
test.assert(triggered_settings[0][0] == 'planet_size');
test.assert(triggered_settings[0][1] == '64x32');

game_settings.rollback(settings_event);
test.assert(settings.global.map.size_x == 40);
test.assert(settings.global.map.size_y == 20);
test.assert(player_ready[0] == true);
test.assert(player_ready[1] == false);
test.assert(triggered_settings == [['planet_size', '40x20']]);

settings_event.data.changes = [
	['planet_size', '64x32'],
	['planet_size', '80x40'],
];
settings_event.applied = game_settings.apply(settings_event);
test.assert(settings.global.map.size_x == 80);
test.assert(settings.global.map.size_y == 40);
test.assert(#sizeof(settings_event.applied.old_settings) == 2);
test.assert(settings_event.applied.old_ui_settings == [['planet_size', '40x20']]);

game_settings.rollback(settings_event);
test.assert(settings.global.map.size_x == 40);
test.assert(settings.global.map.size_y == 20);
test.assert(triggered_settings == [['planet_size', '40x20']]);

const select_faction = #include('../default/game/event/select_faction');
const factions = [
	{id: 'GAIANS'},
	{id: 'HIVE'},
];
let selected_factions = ['GAIANS', #undefined];
let faction_updates = 0;
const faction_players = [
	{
		id: 0,
		get_faction: () => {
			return #is_defined(selected_factions[0]) ? {id: selected_factions[0]} : #undefined;
		},
		set_faction_by_id: (id) => {
			selected_factions[0] = id;
		},
		unset_faction: () => {
			selected_factions[0] = #undefined;
		},
	},
	{
		id: 1,
		get_faction: () => {
			return #is_defined(selected_factions[1]) ? {id: selected_factions[1]} : #undefined;
		},
		set_faction_by_id: (id) => {
			selected_factions[1] = id;
		},
		unset_faction: () => {
			selected_factions[1] = #undefined;
		},
	},
];
const faction_game = {
	is_started: () => {
		return false;
	},
	get_fm: () => {
		return {
			list: () => {
				return factions;
			},
		};
	},
	get_players: () => {
		return faction_players;
	},
	get_player: (id) => {
		return faction_players[id];
	},
	trigger: (name, data) => {
		test.assert(name == 'player_update');
		faction_updates++;
	},
};
let faction_event = {
	caller: 1,
	game: faction_game,
	data: {
		faction: 'GAIANS',
	},
};

test.assert(#is_defined(select_faction.validate(faction_event)));
faction_event.data.faction = 'UNKNOWN';
test.assert(#is_defined(select_faction.validate(faction_event)));
faction_event.data.faction = 'HIVE';
test.assert(!#is_defined(select_faction.validate(faction_event)));

faction_event.applied = select_faction.apply(faction_event);
test.assert(faction_event.applied.faction == 'RANDOM');
test.assert(selected_factions[1] == 'HIVE');
select_faction.rollback(faction_event);
test.assert(!#is_defined(selected_factions[1]));

selected_factions[1] = 'HIVE';
faction_event.data.faction = 'RANDOM';
test.assert(!#is_defined(select_faction.validate(faction_event)));
faction_event.applied = select_faction.apply(faction_event);
test.assert(faction_event.applied.faction == 'HIVE');
test.assert(!#is_defined(selected_factions[1]));
select_faction.rollback(faction_event);
test.assert(selected_factions[1] == 'HIVE');
test.assert(faction_updates == 4);

const chat_message = #include('../default/game/event/chat_message');
const chat_player = {id: 1};
let delivered_chat = null;
const chat_game = {
	get_player: (id) => {
		test.assert(id == chat_player.id);
		return chat_player;
	},
	trigger: (name, data) => {
		test.assert(name == 'chat_message');
		delivered_chat = data;
	},
};
const chat_event = {
	caller: chat_player.id,
	game: chat_game,
	data: {
		text: 'Planetfall confirmed.',
	},
};

test.assert(!#is_defined(chat_message.validate(chat_event)));
chat_message.apply(chat_event);
test.assert(delivered_chat.player == chat_player);
test.assert(delivered_chat.text == chat_event.data.text);

chat_event.data.text = '';
test.assert(#is_defined(chat_message.validate(chat_event)));
chat_event.data.text = 123;
test.assert(#is_defined(chat_message.validate(chat_event)));

let boundary_chat = '';
while (#sizeof(boundary_chat) < 512) {
	boundary_chat += 'x';
}
chat_event.data.text = boundary_chat;
test.assert(!#is_defined(chat_message.validate(chat_event)));
chat_event.data.text = chat_event.data.text + 'x';
test.assert(#is_defined(chat_message.validate(chat_event)));
