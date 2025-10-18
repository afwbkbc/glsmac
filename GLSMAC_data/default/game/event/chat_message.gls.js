return {

	validate: (e) => {
		if (e.data.text == '') {
			return 'Chat message text can\'t be empty';
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
