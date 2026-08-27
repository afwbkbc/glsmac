const MAX_CHAT_MESSAGE_LENGTH = 512;

return {

	validate: (e) => {
		if (#typeof(e.data.text) != 'String') {
			return 'Chat message text must be a string';
		}
		if (e.data.text == '') {
			return 'Chat message text can\'t be empty';
		}
		if (#sizeof(e.data.text) > MAX_CHAT_MESSAGE_LENGTH) {
			return 'Chat message text is too long';
		}
	},

	apply: (e) => {
		e.game.trigger('chat_message', {
			player: e.game.get_player(e.caller),
			text: e.data.text,
		});
	},

	rollback: (e) => {
	},

};
