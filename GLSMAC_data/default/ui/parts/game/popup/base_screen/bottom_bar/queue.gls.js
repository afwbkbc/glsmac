return {

	init: (p) => {

		this.frame = p.body.panel({
			class: 'default-panel',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 137,
			width: 106,
		});

		this.frame.button({
			class: 'game-popup-button',
			text: 'QUEUE',
			align: 'top',
			top: 2,
			left: 2,
			right: 2,
		});

		this.items = this.frame.listview({
			class: 'default-panel-inner',
			top: 24,
			bottom: 45,
			left: 3,
			right: 3,
			itemsize: 14,
			padding: 5,
			has_hscroll: false,
			has_vscroll: false,
		});

		this.frame.button({
			class: 'game-popup-button',
			text: 'CHANGE',
			align: 'bottom',
			bottom: 23,
			left: 2,
			right: 2,
		});
		this.frame.button({
			class: 'game-popup-button',
			text: 'HURRY',
			align: 'bottom',
			bottom: 2,
			left: 2,
			right: 2,
		});

	},

	set: (data) => {

		this.items.clear();

		let i = 0;
		for (item of data) {
			this.items.text({
				class: 'base-screen-frame-text',
				text: item,
			});
			i++;
			if (i == 8) {
				break;
			}
		}
		while (i < 8) {
			this.items.text({
				class: 'base-screen-frame-text',
				text: 'Empty Slot',
			});
			i++;
		}
		
	},

};
