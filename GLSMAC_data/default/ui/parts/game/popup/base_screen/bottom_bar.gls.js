return {

	available_parts: [
		'production',
		'queue',
		'middle_area',
		'support',
	],

	init: (p) => {

		this.p = p;

		this.parts = {};

		this.frame = p.ui.root.area({
			zindex: 0.85,
			align: 'bottom',
			height: p.modules.bottom_bar.height,
			left: 0,
			right: 0,
		});
		this.frame.on('mousedown', (e) => {
			// prevent clickthroughs

			const left = e.ax;
			const right = p.ui.get_width() - e.ax;
			const bottom = p.ui.get_height() - e.ay;

			if (left >= 252 && right >= 262 && bottom >= 10 && bottom <= 60) {
				// allow clickthroughs to objects list
				// TODO: better way to do this?
				return false;
			}

			// block everything else
			return true;
		});
		this.frame.hide();

		// hide menus
		p.ui.class('base-screen-bottombar-menu-hidebutton').set({
			width: 106,
			height: 14,
			background: 'black',
		});
		this.frame.surface({
			class: 'base-screen-bottombar-menu-hidebutton',
			align: 'top left',
			left: 11,
			top: 20,
		}).on('mousedown', (e) => {
			return true;
		});
		this.frame.surface({
			class: 'base-screen-bottombar-menu-hidebutton',
			align: 'top right',
			right: 11,
			top: 22,
		}).on('mousedown', (e) => {
			return true;
		});

		const pp = {
			ui: p.ui,
			game: p.game,
			body: parent.frame,
			utils: p.utils,
		};

		for (s of this.available_parts) {
			this.parts[s] = #include('bottom_bar/' + s);
			this.parts[s].init(pp);
		}

	},

	set: (data) => {
		const base = data.base;
		const production = base.get_production();
		const queue = base.get_production_queue();
		const pending = this.p.game.get('f_base_get_pending_production')(base);
		let definitions = [];
		for (def of this.p.game.get_um().get_unit_defs()) {
			definitions :+def;
		}
		for (def of this.p.game.get_bm().get_facility_defs()) {
			definitions :+def;
		}
		let set_candidates = [];
		let queue_candidates = [];
		for (def of definitions) {
			if (base.can_set_production(def.production_kind, def.id)) {
				set_candidates :+def;
			}
			if (base.can_queue_production(def.production_kind, def.id)) {
				queue_candidates :+def;
			}
		}

		if (#is_defined(production)) {
			const production_cost = this.p.game.get('f_base_get_production_cost')(
				base,
				production
			);
			const is_mineral_conversion =
				#is_defined(production.mineral_to_energy_divisor) &&
				production.mineral_to_energy_divisor > 0;
			const stockpile_energy = is_mineral_conversion
				? this.p.game.get('f_economy_get_base_stockpile_energy')(
					this.p.game,
					base
				)
				: 0;
			this.parts.production.set({
				name: production.name,
				rows: #max(#ceil(#to_float(production_cost) / 10.0), 1),
				columns: 10,
				filled: is_mineral_conversion
					? 0
					: #min(base.get_accumulated_minerals(), production_cost),
				pending: is_mineral_conversion ? 0 : pending,
				conversion_label: is_mineral_conversion
					? #to_string(stockpile_energy) + ' EC / TURN'
					: #undefined,
			});
		} else {
			this.parts.production.set({
				name: 'NOTHING',
				rows: 1,
				columns: 10,
				filled: 0,
				pending: 0,
			});
		}

		this.parts.queue.set({
			base: base,
			production: production,
			queue: queue,
			set_candidates: set_candidates,
			queue_candidates: queue_candidates,
		});

		this.parts.middle_area.set({
			name: base.name,
			owner: base.get_owner(),
			pops: base.get_pops(),
		});

		this.parts.support.set(data.support);

	},

};
