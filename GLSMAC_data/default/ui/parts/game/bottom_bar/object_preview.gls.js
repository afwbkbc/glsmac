return {

	previews: {},
	last_object_class: null,

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
		});

		let line_top = 0;
		const f_line = (text, size, align) => {
			this.lines.text({
				align: 'top ' + align, // TODO: fix centered align
				color: 'rgb(116,156,56)',
				font: 'arialnb.ttf:' + #to_string(size),
				text: text,
				top: line_top,
				left: 3,
			});
			line_top += size + 1;
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
				stats_str += ' - ? - ' + #to_string(def.movement_per_turn);
				f_line(stats_str, 14, 'center');

				// TODO: morale

				if (!object.is_immovable) {
					f_line('Moves: ' + #to_string(object.movement), 14, 'left');
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
