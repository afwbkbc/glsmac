return {

	init: (p) => {
		this.p = p;

		this.el = p.area.panel({
			class: 'base-screen-middle-area',
		});

		p.ui.class('base-screen-psych-status').extend('base-screen-frame-title').set({
			align: 'top center',
			top: 28,
		});

	},

	set: (data) => {
		const state = this.p.game.get('f_base_get_psych')(data.base);
		this.el.clear();
		this.el.text({
			class: 'base-screen-psych-status',
			color: state.is_rioting ? 'rgb(208,96,72)' : 'rgb(116,192,160)',
			text: state.is_rioting ? 'DRONE RIOTS' : 'POPULATION STABLE',
		});
		const rows = [
			['TALENTS', state.talents],
			['WORKERS', state.workers],
			['DRONES', state.drones],
			['SPECIALISTS', state.specialists],
			['PSYCH ENERGY', state.psych],
			['POLICE CONTROL', state.police.suppression],
			[
				'PACIFISM',
				#to_string(state.police.pacifism_drones) +
				' (' + #to_string(state.police.away_units) + ' away)',
			],
		];
		let top = 62;
		for (row of rows) {
			this.el.text({
				class: 'base-screen-frame-text',
				left: 95,
				top: top,
				text: row[0],
			});
			this.el.text({
				class: 'base-screen-frame-text-important',
				left: 260,
				top: top,
				text: #to_string(row[1]),
			});
			top += 24;
		}
	},

};
