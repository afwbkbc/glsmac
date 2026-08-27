return {

	init: (p) => {
		this.p = p;

		this.frame = p.body.panel({
			class: 'default-panel',
			align: 'bottom right',
			top: 59,
			bottom: 6,
			right: 7,
			width: 236,
		});

		const header = this.frame.panel({
			class: 'base-screen-header',
		});
		header.text({
			class: 'base-screen-frame-title',
			text: 'FORCES SUPPORTED',
		});
		const body = this.frame.panel({
			class: 'base-screen-body',
		});
		this.unit_count = body.text({
			class: 'base-screen-frame-text',
			align: 'top center',
			top: 10,
		});
		this.upkeep = body.text({
			class: 'base-screen-frame-text-important',
			align: 'top center',
			top: 34,
		});

	},

	set: (data) => {
		this.unit_count.text =
			#to_string(#sizeof(data.units)) + ' UNITS / ' +
			#to_string(data.free_units) + ' FREE';
		this.upkeep.text = #to_string(data.mineral_upkeep) + ' MINERAL UPKEEP';
	},

};
