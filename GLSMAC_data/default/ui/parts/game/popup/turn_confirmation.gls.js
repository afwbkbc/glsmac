return {

	init: (p) => {

		return p.create('OPERATIONS DIRECTOR', 500, 92, (body, cb) => {

			const prompt = body.panel({
				class: 'game-popup-block',
				height: 42,
			});
			prompt.text({
				class: 'game-popup-text',
				text: 'Some of our units have not yet moved this turn.',
				top: 3,
			});
			prompt.text({
				class: 'game-popup-text',
				text: 'Do you really want to end the turn now?',
				top: 20,
			});

			body.button({
				class: 'game-popup-button',
				text: 'Cancel',
				top: 46,
				is_cancel: true,
			}).on('click', (e) => {
				cb(false);
				return true;
			});

			body.button({
				class: 'game-popup-button',
				text: 'Yes, end the turn.',
				top: 68,
				is_ok: true,
			}).on('click', (e) => {
				cb(true);
				return true;
			});

		});

	},

	show: (cb) => {


	},

};
