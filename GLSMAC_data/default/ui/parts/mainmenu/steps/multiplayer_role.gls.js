return (i) => {
	let choices = null;
	i.popup.show({
		title: 'Host or Join Session',
		width: 480,
		height: 126,
		generator: (body) => {
			choices = body.choicelist({
				left: 10,
				right: 10,
				top: 16,
				keyboard: true,
				itemclass: 'popup-list-button',
				itemheight: 19,
				itempadding: 1,
				items: [
					['server', 'Host'],
					['client', 'Join'],
				],
			});
			if (i.settings.local.network_role != 'none') {
				choices.value = i.settings.local.network_role;
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
					i.settings.local.network_role = choices.value;
					if (i.settings.local.network_role == 'server') {
						i.steps.multiplayer_host(i);
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
