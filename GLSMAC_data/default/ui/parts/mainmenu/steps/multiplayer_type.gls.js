return (i) => {
	let choices = null;
	i.popup.show({
		title: 'Multiplayer Setup',
		width: 480,
		height: 140,
		generator: (body) => {
			body.text({
				class: 'popup-text',
				text: 'Select a service...',
				align: 'top center',
				top: 5,
			});
			choices = body.choicelist({
				left: 10,
				right: 10,
				top: 30,
				keyboard: true,
				itemclass: 'popup-list-button',
				itemheight: 19,
				itempadding: 1,
				items: [
					['simple_tcpip', 'Simple Internet TCP/IP Connection'],
					['hotseat', 'Hotseat/Play-by-Email'],
				],
			});
			if (i.settings.local.network_type != 'none') {
				choices.value = i.settings.local.network_type;
			}
		},
		buttons: [
			{
				style: {
					text: 'OK',
					align: 'left',
					is_ok: true,
				},
				onclick: (x) => {
					i.settings.local.network_type = choices.value;
					if (i.settings.local.network_type == 'simple_tcpip') {
						i.steps.multiplayer_lobby(i);
					} else {
						i.steps.notimpl(i);
					}
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
