return {

	init: (p) => {

		p.ui.class('base-screen-energy-percentage-header').extend('base-screen-side-header').set({
			left: 131,
			width: 44,
		});

		p.ui.class('base-screen-energy-data-text').extend('base-screen-side-header-text').set({
			align: 'top left',
		});

		this.frame = p.body.panel({
			class: 'base-screen-side-middle-frame',
			align: 'top right',
			top: 350,
			right: 3,
			height: 89,
		});

		this.frame.panel({
			class: 'base-screen-header',
		})
			.text({
				class: 'base-screen-side-header-text',
				color: 'rgb(224,156,28)',
				text: 'ENERGY ALLOCATION',
			})
		;

		const c_economy = 'rgb(232,212,140)';
		const c_labs = 'rgb(216,224,235)';
		const c_psych = 'rgb(116,192,160)';

		const header_height = 19;
		const top_economy = 5;
		const top_labs = 26;
		const top_psych = 47;

		const left_value = 7;
		const left_bonus = 94;
		const left_total = 176;

		this.frame.panel({
			class: 'base-screen-side-header',
			top: top_economy + header_height,
		})
			.text({
				class: 'base-screen-side-header-text',
				color: c_economy,
				text: 'ECONOMY',
			})
		;

		this.frame.panel({
			class: 'base-screen-side-header',
			top: top_labs + header_height,
		})
			.text({
				class: 'base-screen-side-header-text',
				color: c_labs,
				text: 'LABS',
			})
		;

		this.frame.panel({
			class: 'base-screen-side-header',
			top: top_psych + header_height,
		})
			.text({
				class: 'base-screen-side-header-text',
				color: c_psych,
				text: 'PSYCH',
			})
		;

		const body = this.frame.panel({
			class: 'base-screen-body',
			align: 'bottom right',
			width: 360,
			height: 62,
		}).area({
			align: 'center',
			width: 200,
		});

		for (top of [top_economy, top_labs, top_psych]) {
			body.text({
				class: 'base-screen-frame-info-text',
				top: top,
				left: 32,
				text: 'ENERGY +',
			});
			body.text({
				class: 'base-screen-frame-info-text',
				top: top,
				left: 120,
				text: 'BONUS =',
			});
		}

		const f = this.frame; // TODO: fix 'this' inside objects
		this.labels = {
			economy: {
				percentage: f.panel({
					class: 'base-screen-energy-percentage-header',
					top: top_economy + header_height,
				}).text({
					class: 'base-screen-side-header-text',
					color: c_economy,
				}),
				value: body.text({
					class: 'base-screen-energy-data-text',
					top: top_economy,
					left: left_value,
					color: c_economy,
				}),
				bonus: body.text({
					class: 'base-screen-energy-data-text',
					top: top_economy,
					left: left_bonus,
					color: c_economy,
				}),
				total: body.text({
					class: 'base-screen-energy-data-text',
					top: top_economy,
					left: left_total,
					color: c_economy,
				}),
			},
			labs: {
				percentage: f.panel({
					class: 'base-screen-energy-percentage-header',
					top: top_labs + header_height,
				}).text({
					class: 'base-screen-side-header-text',
					color: c_labs,
				}),
				value: body.text({
					class: 'base-screen-energy-data-text',
					top: top_labs,
					left: left_value,
					color: c_labs,
				}),
				bonus: body.text({
					class: 'base-screen-energy-data-text',
					top: top_labs,
					left: left_bonus,
					color: c_labs,
				}),
				total: body.text({
					class: 'base-screen-energy-data-text',
					top: top_labs,
					left: left_total,
					color: c_labs,
				}),
			},
			psych: {
				percentage: f.panel({
					class: 'base-screen-energy-percentage-header',
					top: top_psych + header_height,
				}).text({
					class: 'base-screen-side-header-text',
					color: c_psych,
				}),
				value: body.text({
					class: 'base-screen-energy-data-text',
					top: top_psych,
					left: left_value,
					color: c_psych,
				}),
				bonus: body.text({
					class: 'base-screen-energy-data-text',
					top: top_psych,
					left: left_bonus,
					color: c_psych,
				}),
				total: body.text({
					class: 'base-screen-energy-data-text',
					top: top_psych,
					left: left_total,
					color: c_psych,
				}),
			},
		};

	},

	_pad: (value) => {
		return #pad(#to_string(value), 'left', 3, ' ');
	},

	set: (data) => {
		for (type in data) {
			const d = data[type];
			const l = this.labels[type];
			l.percentage.text = this._pad(#round(d.allocation * 100.0)) + '%';
			l.value.text = this._pad(d.value);
			l.bonus.text = this._pad(d.bonus);
			l.total.text = this._pad(d.value + d.bonus);
		}
	},

};
