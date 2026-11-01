return {

	ITEM_WIDTH: 132,
	ITEM_HEIGHT: 105,
	ITEM_PADDING: 3,

	COLORS: {
		explore: '#588c2c',
		build: '#e8d48c',
		discover: '#adc4c0',
		conquer: '#da8677',
	},

	init: (p) => {

		this.area = p.body.panel({
			class: 'default-panel',
			top: 0,
			left: 0,
			right: 0,
			bottom: 0,
		});
		this.area.hide();

		return this.area;
	},

	show: () => {
		this.area.show();
	},

	hide: () => {
		this.area.hide();
	},

	set: (data) => {
		this.area.clear();
		const production_choices = data.base.get_production_choices();
		let x = this.ITEM_PADDING;
		let y = this.ITEM_PADDING;

		const intake = data.base.get_intake();
		const consumption = data.base.get_consumption();
		const minerals_per_turn = intake.MINERALS - consumption.MINERALS;

		for (choice of production_choices) {

			const panel = this.area.panel({
				class: 'default-panel-inner',
				align: 'top left',
				left: x,
				top: y,
				width: parent.ITEM_WIDTH,
				height: parent.ITEM_HEIGHT,
			});

			panel.text({
				class: 'base-screen-production-title',
				text: choice.name,
				color: parent.COLORS[choice.intent],
			});

			panel.surface({
				class: 'base-screen-production-preview',
				background: choice.texture + ':tint(' + parent.COLORS[choice.intent] + ',1.0):stretch',
			});

			let eta = '';
			if (#is_defined(choice.cost)) {
				const turns = #ceil(#to_float(choice.cost) / #to_float(minerals_per_turn));
				if (turns != 1) {
					eta = #to_string(turns) + ' turns';
				} else {
					eta = #to_string(turns) + ' turn';
				}
			} else if (choice.id == 'STOCKPILE_ENERGY') {
				eta = #to_string(#floor(#to_float(minerals_per_turn) / 2.0)) + ' Energy per turn';
			}

			panel.text({
				class: 'base-screen-production-eta',
				text: eta,
			});

			panel.text({
				class: 'base-screen-production-hint',
				text: choice.hint,
			});

			x += this.ITEM_WIDTH + this.ITEM_PADDING;
		}
	},

};
