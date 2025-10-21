return (i) => {

	let chathistory = null;

	return {
		section: i.make_section(#undefined, {
			left: 301,
			top: 207,
			right: 3,
			bottom: 208,
		}, (body, lobby) => {

			chathistory = body.listview({
				class: 'popup-list',
				left: 3,
				right: 3,
				top: 3,
				height: 123,
				max_items: 50,
			});
			const chatinput = body.input({
				class: 'popup-input',
				align: 'top',
				top: 128,
				left: 3,
				right: 3,
			});

			chatinput.on('input', (e) => {
				if (chatinput.value != '') {
					i.glsmac.game.event('chat_message', {
						text: chatinput.value,
					});
					chatinput.value = '';
				}
				return true;
			});
		}),
		add_message: (text) => {
			chathistory.text({
				class: 'popup-list-line',
				text: text,
			});
		},
	};
};
