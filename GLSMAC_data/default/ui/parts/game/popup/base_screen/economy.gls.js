return {

	init: (p) => {
		this.partner_names = [];
		this.partner_values = [];
		p.ui.class('base-screen-commerce-text').set({
			font: 'arialn.ttf:12',
			color: 'rgb(59,111,128)',
		});

		this.frame = p.body.panel({
			class: 'base-screen-side-frame',
			align: 'top left',
			top: 188,
			left: 3,
			height: 194,
		});

		const header = this.frame.panel({
			class: 'base-screen-header',
		});
		this.header = header.text({
			class: 'base-screen-frame-title',
		});

		const body = this.frame.panel({
			class: 'base-screen-body',
		});

		let row = 0;
		while (row < 6) {
			this.partner_names :+body.text({
				class: 'base-screen-commerce-text',
				text: '', left: 4, right: 32, top: 4 + row * 21,
			});
			this.partner_values :+body.text({
				class: 'base-screen-commerce-text',
				text: '', align: 'top right', right: 4, top: 4 + row * 21,
			});
			row++;
		}
		this.total_name = body.text({
			class: 'base-screen-frame-text-important',
			text: 'TOTAL', left: 4, top: 137,
		});
		this.total_value = body.text({
			class: 'base-screen-frame-text-important',
			text: '0', align: 'top right', right: 4, top: 137,
		});

	},

	clear: () => {
		let row = 0;
		while (row < #sizeof(this.partner_names)) {
			this.partner_names[row].text = '';
			this.partner_values[row].text = '';
			row++;
		}
		this.total_name.text = 'TOTAL';
		this.total_value.text = '0';
	},

	set_commerce: (data) => {
		this.header.text = 'COMMERCE';
		this.clear();
		let row = 0;
		while (row < #sizeof(data.partners) && row < #sizeof(this.partner_names)) {
			const partner = data.partners[row];
			this.partner_names[row].text = partner.player_name;
			this.partner_values[row].text = '+' + #to_string(partner.value);
			row++;
		}
		this.total_value.text = '+' + #to_string(data.total);
	},

	set_energy_grid: (data) => {
		this.header.text = 'ENERGY GRID';
		this.clear();
		this.total_name.text = '';
		this.total_value.text = '';
	},

};
