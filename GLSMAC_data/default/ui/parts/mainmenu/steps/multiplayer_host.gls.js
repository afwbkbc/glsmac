return (i) => {
	let l_gamename = null;
	let l_yourname = null;
	let settings = i.glsmac.game.get_settings();
	let account = settings.local.account;
	i.popup.show({
		title: 'Host Multiplayer Game',
		width: 480,
		height: 130,
		generator: (body) => {
			body.text({
				class: 'popup-text',
				text: 'Enter game name:',
				align: 'left',
				top: 13,
				left: 15,
			});
			l_gamename = body.input({
				class: 'popup-input',
				top: 12,
				left: 150,
				width: 310,
				value: account.game_name,
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
				value: account.player_name,
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
					account.game_name = #trim(l_gamename.value);
					account.player_name = #trim(l_yourname.value);
					if (account.game_name == '') {
						i.popup.error('Enter game name!');
						return true;
					}
					if (account.player_name == '') {
						i.popup.error('Enter your name!');
						return true;
					}
					settings.global.game_name = account.game_name;
					settings.local.player_name = account.player_name;
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
