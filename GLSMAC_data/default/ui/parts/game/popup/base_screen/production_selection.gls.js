return {

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
		#print('CHOICES', production_choices);
	},

};
