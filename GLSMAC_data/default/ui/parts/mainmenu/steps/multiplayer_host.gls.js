return (i) => {
	let l_gamename = null;
	let l_yourname = null;
	let settings = i.glsmac.game.get_settings();
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
				value: settings.global.game_name,
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
					settings.global.game_name = l_gamename.value;
					settings.local.player_name = l_yourname.value;
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
