return {

	init: (p) => {

		this.sound = null;

		return p.create('SID MEIER\'S ALPHA CENTAURI', 670, 171, (body, cb) => {

			body.surface({
				left: 3,
				top: 3,
				background: 'space_sm.pcx',
				width: 200,
				height: 120,
			});

			const prompt = body.panel({
				class: 'game-popup-block',
				height: 24,
				left: 205,
			});
			prompt.text({
				class: 'game-popup-text',
				text: 'Do you really want to quit?',
				top: 3,
			});

			body.button({
				class: 'game-popup-button',
				text: 'Oops, no, wait!',
				top: 125,
				is_cancel: true,
			}).on('click', (e) => {
				cb(false);
				return true;
			});

			body.button({
				class: 'game-popup-button',
				text: 'Yes, that\'s an order mister!',
				top: 147,
				is_ok: true,
			}).on('click', (e) => {
				cb(true);
				return true;
			});

			this.sound = body.sound({
				sound: 'CPU please don\'t go.wav',
				volume: 0.6,
			});

		});

	},

	on_show: () => {
		this.sound.play();
	},

	on_hide: () => {
		this.sound.stop();
	}

};
