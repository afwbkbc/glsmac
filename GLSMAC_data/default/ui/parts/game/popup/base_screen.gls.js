return {

	available_sections: [
		'top_buttons',
		'nutrients',
		'economy',
		'game_state',
		'middle_area',
		'facilities',
		'resources',
		'energy',
		'buttons',
		'bottom_bar',
	],

	production_selection_no_hiding: {
		buttons: true,
		bottom_bar: true,
	},

	init: (p) => {

		this.sections = {};
		this.p = p;
		this.is_open = false;
		this.base_id = null;

		this.production_selection = null;

		p.ui.class('base-screen-frame').set({
			border: 'rgb(35,59,34),2',
		});
		p.ui.class('base-screen-block').extend('default-panel-inner').set({
			top: 3,
			left: 3,
			bottom: 3,
			right: 3,
		});

		p.ui.class('base-screen-header').extend('base-screen-block').set({
			align: 'top',
			height: 20,
		});
		p.ui.class('base-screen-body').extend('base-screen-block').set({
			align: 'top',
			top: 24,
		});

		p.ui.class('base-screen-side-frame').extend('base-screen-frame').set({
			width: 132,
		});
		p.ui.class('base-screen-side-middle-frame').extend('base-screen-frame').set({
			width: 539,
		});

		p.ui.class('base-screen-frame-title').set({
			align: 'center',
			font: 'arialnb.ttf:16',
			color: 'rgb(53,61,115)',
		});
		p.ui.class('base-screen-frame-text').set({
			font: 'arialnb.ttf:14', // TODO: investigate why non-bold fonts look bad (LOD settings?)
			color: 'rgb(59,111,128)',
		});

		p.ui.class('base-screen-frame-info-text').set({
			font: 'arialnb.ttf:14',
			color: 'rgb(42,101,120)',
		});

		p.ui.class('base-screen-frame-text-important').extend('base-screen-frame-text').set({
			color: 'rgb(120,164,212)',
		});

		p.ui.class('base-screen-side-header').extend('base-screen-body').set({
			left: 3,
			width: 127,
			height: 20,
		});

		p.ui.class('base-screen-side-header-text').set({
			font: 'arialnb.ttf:15',
			align: 'center',
		});

		return p.create('', 680, 442, (body, cb) => {

			this.body = body;

			body.listen(p.game, 'update_base', (e) => {
				if (this.is_open && e.base.id == this.base_id) {
					this.set({
						base: e.base,
					});
				}
			});

			const pp = {
				body: body,
				ui: p.ui,
				game: p.game,
				hide: p.hide,
				modules: p.modules,
				utils: {
					set_cells: parent.parent.set_cells,
					show_production_selection: parent.parent.show_production_selection,
					hide_production_selection: parent.parent.hide_production_selection,
					update_production_selection: parent.parent.update_production_selection,
				},
			};

			for (s of this.available_sections) {
				this.sections[s] = #include('base_screen/' + s);
				this.sections[s].body = this.sections[s].init(pp);
			}

		});

	},

	set: (data) => {

		const game = this.p.game;
		const base = data.base;
		const owner = base.get_owner();
		const faction = owner.get_faction();

		this.base_id = base.id;

		const intake = base.get_intake();
		const consumption = base.get_consumption();

		this.hide_production_selection();

		// dummy data for now

		this.sections.nutrients.set({
			rows: base.get_size() + 1,
			columns: game.get('map_growth_base'),
			filled: base.get('accumulated_nutrients'),
			pending: game.get('f_base_get_pending_growth')(base),
		});

		if (faction.is_progenitor) {
			this.sections.economy.set_energy_grid({
				// TODO
			});
		} else {
			this.sections.economy.set_commerce({
				// TODO
			});
		}

		this.sections.game_state.set({
			year: game.get_year(),
			energy: 0, // TODO
			ecodamage: 0, // TODO
		});

		let facilities = [];
		const ff = base.get_facilities();
		for (f in ff) {
			facilities :+ff[f].name;
		}

		this.sections.facilities.set(facilities);

		const resource_data = {
			nutrients: {
				profit: intake.NUTRIENTS,
				loss: consumption.NUTRIENTS,
			},
			minerals: {
				profit: intake.MINERALS,
				loss: consumption.MINERALS,
			},
			energy: {
				profit: intake.ENERGY,
				loss: consumption.ENERGY,
			},
		};
		this.sections.resources.set(resource_data);

		const allocation_labs = 0.4;
		const allocation_psych = 0.2;

		const total_energy = resource_data.energy.profit - resource_data.energy.loss;
		const energy_data = {
			labs: {
				allocation: allocation_labs,
				value: #round(#to_float(total_energy) * allocation_labs),
				bonus: 0,
			},
			psych: {
				allocation: allocation_psych,
				value: #round(#to_float(total_energy) * allocation_psych),
				bonus: 0,
			},
		};
		energy_data.economy = {
			allocation: 1.0 - energy_data.labs.allocation - energy_data.psych.allocation,
			value: total_energy - energy_data.labs.value - energy_data.psych.value,
			bonus: 0,
		};
		this.sections.energy.set(energy_data);

		this.sections.middle_area.set({
			base: base,
		});

		this.sections.bottom_bar.set({
			base: base,
		});
	},

	on_hide: () => {
		this.sections.bottom_bar.frame.hide();
		this.hide_production_selection();
		this.is_open = false;
	},

	on_show: () => {
		this.is_open = true;
		this.sections.bottom_bar.frame.show();
	},

	set_cells: (total_width, total_height, columns, rows, filled, pending, cells_el, cell_baseclass, label_el, f_label) => {
		cells_el.clear();

		const width = #floor(#to_float(total_width) / #to_float(columns));
		const height = #floor(#to_float(total_height) / #to_float(rows));

		this.p.ui.class(cell_baseclass).set({
			width: width - 1,
			height: height - 1,
		});

		const offset_left = (total_width - (columns * width)) / 2;
		let left = offset_left;
		let top = (total_height - (rows * height)) / 2;

		let i = 0;
		let cls = '';

		for (let y = 0; y < rows; y++) {
			for (let x = 0; x < columns; x++) {
				if (i < filled) {
					if (pending < 0 && i >= filled + pending) {
						cls = 'deficit';
					} else {
						cls = 'full';
					}
				} else if (i < filled + pending) {
					cls = 'pending';
				} else {
					cls = 'empty';
				}
				i++;
				cells_el.panel({
					class: cell_baseclass + '-' + cls,
					left: left + 1,
					top: top + 1,
				});
				left += width;
			}
			top += height;
			left = offset_left;
		}

		let progress_in = 0;
		if (pending > 0) {
			progress_in = #ceil(#to_float(rows * columns - filled) / #to_float(pending));
		}
		label_el.text = f_label(progress_in);
	},

	hide_production_selection: () => {
		if (this.production_selection != null) {
			this.production_selection.remove();
			this.production_selection = null;
			this.sections.buttons.show_default();
			for (s in this.sections) {
				if (!#is_defined(this.production_selection_no_hiding[s])) {
					this.sections[s].body.show();
				}
			}
		}
	},

	show_production_selection: () => {
		if (this.production_selection == null) {
			for (s in this.sections) {
				if (!#is_defined(this.production_selection_no_hiding[s])) {
					this.sections[s].body.hide();
				}
			}
			this.production_selection = this.body.panel({
				class: 'default-panel',
				top: 0,
				left: 0,
				right: 0,
				bottom: 0,
			});
			this.sections.buttons.show_production_selection();
		}
	},

	update_production_selection: () => {
		if (this.production_selection != null) {

			// TODO: update selection to selected item

			this.hide_production_selection();
		}
	},

};
