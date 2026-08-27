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

	init: (p) => {

		this.sections = {};
		this.p = p;
		this.is_open = false;
		this.base_id = null;

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
				},
			};

			for (s of this.available_sections) {
				this.sections[s] = #include('base_screen/' + s);
				this.sections[s].init(pp);
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
		let supported_units = [];
		for (unit of game.get_um().get_units()) {
			if (unit.owner == owner.id && unit.home_base_id == base.id) {
				supported_units :+unit;
			}
		}
		const free_support_capacity = #max(base.get_size(), 1);
		const support = {
			units: supported_units,
			free_units: #min(#sizeof(supported_units), free_support_capacity),
			mineral_upkeep: consumption.MINERALS,
		};

		// dummy data for now

		this.sections.nutrients.set({
			rows: base.get_size() + 1,
			columns: game.get('map_growth_base'),
			capacity: game.get('f_base_get_nutrients_for_growth')(game, base),
			filled: base.get('accumulated_nutrients'),
			pending: game.get('f_base_get_pending_growth')(base),
		});

		if (faction.is_progenitor) {
			this.sections.economy.set_energy_grid({
				// TODO
			});
		} else {
			this.sections.economy.set_commerce(
				game.get('f_economy_get_base_commerce')(game, base)
			);
		}

		this.sections.game_state.set({
			year: game.get_year(),
			energy: owner.energy_credits,
			ecodamage: game.get('f_ecology_get_base_damage')(base).percent,
		});

		let facility_names = [];
		for (facility of base.get_facilities()) {
			facility_names :+(facility.is_project ? 'PROJECT: ' : '') + facility.name;
		}
		this.sections.facilities.set(facility_names);

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

		this.sections.energy.set(game.get('f_economy_get_base_allocation')(game, base));

		this.sections.middle_area.set({
			base: base,
			support: support,
		});
		this.sections.buttons.set({
			base: base,
		});

		this.sections.bottom_bar.set({
			base: base,
			support: support,
		});
	},

	on_hide: () => {
		this.sections.bottom_bar.frame.hide();
		this.is_open = false;
	},

	on_show: () => {
		this.is_open = true;
		this.sections.bottom_bar.frame.show();
	},

	set_cells: (total_width, total_height, columns, rows, filled, pending, cells_el, cell_baseclass, label_el, f_label, capacity_in) => {
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
		const capacity = #is_defined(capacity_in) ? capacity_in : rows * columns;

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
				if (i <= capacity) {
					cells_el.panel({
						class: cell_baseclass + '-' + cls,
						left: left + 1,
						top: top + 1,
					});
				}
				left += width;
			}
			top += height;
			left = offset_left;
		}

		let progress_in = 0;
		if (pending > 0) {
			progress_in = #ceil(#to_float(capacity - filled) / #to_float(pending));
		}
		label_el.text = f_label(progress_in);
	},

};
