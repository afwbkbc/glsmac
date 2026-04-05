return {

	init: (p) => {

		this.frame = p.body.panel({
			class: 'base-screen-side-frame',
			align: 'top left',
			top: 384,
			left: 3,
			height: 55,
		});

		const info = this.frame.panel({
			class: 'base-screen-block',
		});

		info.text({
			class: 'base-screen-frame-text',
			text: 'M.Y.',
			top: 4,
			left: 5,
		});
		this.info_year = info.text({
			class: 'base-screen-frame-text-important',
			top: 4,
			left: 32,
		});

		info.text({
			class: 'base-screen-frame-text',
			text: 'Energy:',
			top: 19,
			left: 5,
		});
		this.info_energy = info.text({
			class: 'base-screen-frame-text-important',
			top: 19,
			left: 49,
		});

		this.info_ecodamage = info.text({
			class: 'base-screen-frame-text',
			color: 'rgb(88,140,44)',
			top: 34,
			left: 5,
		});

	},

	set: (data) => {
		this.info_year.text = #to_string(data.year);
		this.info_energy.text = #to_string(data.energy);
		this.info_ecodamage.text = 'Eco-Damage: ' + #to_string(data.ecodamage);
	},

};
