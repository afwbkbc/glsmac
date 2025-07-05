return (i) => {
	let l_address = null;
	let l_yourname = null;
	let settings = i.glsmac.game.get_settings();
	i.popup.show({
		title: 'Join Multiplayer Game',
		width: 480,
		height: 130,
		generator: (body) => {
			body.text({
				class: 'popup-text',
				text: 'Enter address:',
				align: 'left',
				top: 13,
				left: 15,
			});
			l_address = body.input({
				class: 'popup-input',
				top: 12,
				left: 150,
				width: 310,
				value: settings.local.remote_address,
			});
			body.text({
				class: 'popup-text',
				text: 'Enter your name:',
				align: 'left',
				top: 40,
				left: 15,
			});
			l_yourname = body.input({
				class: 'popup-input',
				top: 39,
				left: 150,
				width: 310,
				value: settings.local.player_name,
			});
		},
		buttons: [
			{
				style: {
					text: 'OK',
					align: 'left',
					is_ok: true,
				},
				onclick: (x) => {
					settings.local.remote_address = #trim(l_address.value);
					settings.local.player_name = #trim(l_yourname.value);
					if (settings.local.remote_address == '') {
						i.popup.error('Enter address!');
						return true;
					}
					if (settings.local.player_name == '') {
						i.popup.error('Enter your name!');
						return true;
					}
					i.steps.multiplayer_lobby(i);
					return true;
				},
			},
			{
				style: {
					text: 'Cancel',
					align: 'right',
					is_cancel: true,
				},
				onclick: (x) => {
					i.popup.back();
					return true;
				},
			},
		],
	});
};
