return {

	init: (p) => {
		this.p = p;

		this.el = p.area.panel({
			class: 'base-screen-middle-area',
		});

		p.ui.class('base-screen-support-status').extend('base-screen-frame-title').set({
			align: 'top center',
			top: 20,
		});
	},

	set: (data) => {
		this.el.clear();
		const total = #sizeof(data.units);
		this.el.text({
			class: 'base-screen-support-status',
			text:
				#to_string(total) + ' UNITS  /  ' +
				#to_string(data.free_units) + ' FREE  /  ' +
				#to_string(data.mineral_upkeep) + ' MINERAL UPKEEP',
		});

		let top = 53;
		let shown = 0;
		for (unit of data.units) {
			if (shown == 6) {
				break;
			}
			this.el.text({
				class: 'base-screen-frame-text',
				left: 48,
				top: top,
				text: unit.get_def().name,
			});
			this.el.text({
				class: 'base-screen-frame-text-important',
				left: 270,
				top: top,
				text: #to_string(#round(unit.health * 100.0)) + '%',
			});
			top += 22;
			shown++;
		}
		if (shown < total) {
			this.el.text({
				class: 'base-screen-frame-text',
				left: 48,
				top: top,
				text: '+ ' + #to_string(total - shown) + ' MORE',
			});
		}
	},

};
