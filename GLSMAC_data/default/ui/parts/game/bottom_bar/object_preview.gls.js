return {

	previews: {},
	last_object_class: null,

	moralesets: {},

	get_morale: (type, value) => {
		if (!#is_defined(this.moralesets[type])) {
			this.moralesets[type] = this.um.get_moraleset('NATIVE');
		}
		if (value >= #sizeof(this.moralesets[type])) {
			console.warn('Morale not found: ' + type = '/' + #to_string(value));
			return '(???)';
		}
		return this.moralesets[type][value];
	},

	format_movement: (movement) => {
		return #to_string(#to_float(#round(movement * 100.0)) / 100.0);
	},

	set_image: (object) => {
		const cls = #classof(object);
		if (cls == 'Base') {
			return; // TODO
		}
		if (this.last_object_class != cls) {
			if (this.last_object_class != null) {
				this.previews[this.last_object_class].hide();
			}
		}
		let type = '';
		let data = {};
		switch (cls) {
			case 'Unit': {
				type = 'unit-preview';
				data.unit = object;
				break;
			}
			case 'Base': {
				type = 'base-preview';
				data.base = object;
				break;
			}
			default: {
				throw Error('Unknown object class: ' + cls);
			}
		}
		if (!#is_defined(this.previews[cls])) {
			this.previews[cls] = this.frame.widget({
				type: type,
				data: data,
				align: 'top center',
				top: 12,
				width: 100,
				height: 75,
			});
		} else {
			this.previews[cls].data = data;
		}
		if (this.last_object_class != cls) {
			this.previews[cls].show();
			this.last_object_class = cls;
		}
	},

	set_lines: (object) => {

		if (#is_defined(this.lines)) {
			this.lines.remove(); // TODO: fix this.lines.clear();
		}
		this.lines = this.frame.listview({
			left: 3,
			right: 3, // TODO: fix
			top: 86,
			bottom: 3,
			itemsize: 17,
		});

		const f_line = (text, size, align) => {
			this.lines.text({
				align: 'top ' + align, // TODO: fix centered align
				color: 'rgb(116,156,56)',
				font: 'arialnb.ttf:' + #to_string(size),
				text: text,
				left: 3,
			});
		};

		switch (#classof(object)) {
			case 'Unit': {
				const def = object.get_def();

				f_line(def.name, 16, 'center');
				let stats_str = '';

				// tmp
				if (def.id == 'SporeLauncher') {
					stats_str += '(?)';
				} else {
					stats_str += '?';
				}
				stats_str += ' - ? - ' + #to_string(#round(def.movement_per_turn));
				f_line(stats_str, 14, 'center');

				f_line(this.get_morale('NATIVE', object.morale), 14, 'left');

				if (!object.is_immovable) {
					f_line('Moves: ' + this.format_movement(object.movement), 14, 'left');
				}

				break;
			}
			case 'Base': {
				break;
			}
			default: {
				throw Error('Unknown object class: ' + #classof(object));
			}
		}
	},

	init: (p) => {

		this.um = p.game.get_um();

		this.frame = p.el.panel({
			class: 'bottombar-frame',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 6,
			width: 129,
		});

		const f_update = (object) => {
			this.set_image(object);
			this.set_lines(object);
		};

		p.map.on('unit_select', (e) => {
			f_update(e.unit);
		});
		p.map.on('base_select', (e) => {
			f_update(e.base);
		})

	},

};
