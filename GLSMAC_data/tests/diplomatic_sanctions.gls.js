const process_sanctions = #include('../default/game/event/process_diplomatic_sanctions');

let sanction_turns = 2;
let triggers = [];
let messages = [];
const player = {
	id: 1,
	name: 'Sanctioned Faction',
	get_sanction_turns: () => { return sanction_turns; },
	set_sanction_turns: (turns) => { sanction_turns = turns; },
};
const game = {
	trigger: (name, data) => { triggers :+{name: name, data: data}; },
	message: (text) => { messages :+text; },
};

let event = {caller: 0, game: game, data: {player: player}};
test.assert(!#is_defined(process_sanctions.validate(event)));
event.applied = process_sanctions.apply(event);
test.assert(sanction_turns == 1);
test.assert(#sizeof(messages) == 0);
process_sanctions.rollback(event);
test.assert(sanction_turns == 2);

sanction_turns = 1;
event.applied = process_sanctions.apply(event);
test.assert(sanction_turns == 0);
test.assert(#sizeof(messages) == 1);
process_sanctions.rollback(event);
test.assert(sanction_turns == 1);

event.caller = 1;
test.assert(#is_defined(process_sanctions.validate(event)));
event.caller = 0;
sanction_turns = 0;
test.assert(#is_defined(process_sanctions.validate(event)));
test.assert(#sizeof(triggers) == 4);
