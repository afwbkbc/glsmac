return {

	_candidate_key: (def) => {
		return def.production_kind + ':' + def.id;
	},

	_candidate_label: (def) => {
		return #to_string(def.name);
	},

	_candidate_signature: (candidates, empty_label) => {
		if (#sizeof(candidates) == 0) {
			return empty_label;
		}
		let signature = '';
		for (candidate of candidates) {
			signature += '|' + this._candidate_key(candidate) + ':' + candidate.name;
		}
		return signature;
	},

	init: (p) => {
		this.p = p;
		this.base = null;
		this.production = #undefined;
		this.queue = [];
		this.set_candidates = [];
		this.queue_candidates = [];
		this.set_candidates_by_key = {};
		this.queue_candidates_by_key = {};
		this.queue_candidate = null;
		this.set_candidates_signature = null;
		this.queue_candidates_signature = null;

		this.frame = p.body.panel({
			class: 'default-panel',
			align: 'top left',
			top: 59,
			bottom: 7,
			left: 137,
			width: 106,
		});

		p.ui.class('base-screen-production-select').extend('popup-list-select').set({
			itemclass: 'base-screen-production-select-item',
		});
		p.ui.class('base-screen-production-select-item').extend('popup-list-select-item').set({
			font: 'arialnb.ttf:11',
		});

		this.queue_select = this.frame.select({
			class: 'base-screen-production-select',
			align: 'top',
			top: 2,
			left: 2,
			right: 2,
			items: [['', 'QUEUE EMPTY']],
			value: '',
			readonly: true,
		});
		this.queue_select.on('select', (e) => {
			this.queue_candidate = #is_defined(this.queue_candidates_by_key[e.value])
				? this.queue_candidates_by_key[e.value]
				: null;
			return true;
		});

		this.items = this.frame.listview({
			class: 'default-panel-inner',
			top: 24,
			bottom: 45,
			left: 3,
			right: 3,
			itemsize: 14,
			padding: 5,
			has_hscroll: false,
			has_vscroll: false,
		});

		this.change_select = this.frame.select({
			class: 'base-screen-production-select',
			align: 'bottom',
			bottom: 23,
			left: 2,
			right: 2,
			items: [['', 'NOTHING']],
			value: '',
			readonly: true,
		});
		this.change_select.on('select', (e) => {
			if (this.base == null || !#is_defined(this.set_candidates_by_key[e.value])) {
				return true;
			}
			const selected = this.set_candidates_by_key[e.value];
			this.p.game.event('set_base_production', {
				base: this.base,
				kind: selected.production_kind,
				id: selected.id,
			});
			return true;
		});

		this.add_button = this.frame.button({
			class: 'game-popup-button',
			text: '+',
			align: 'bottom left',
			bottom: 2,
			left: 2,
			width: 49,
		});
		this.add_button.on('click', (e) => {
			if (this.base == null || this.queue_candidate == null) {
				return true;
			}
			this.p.game.event('queue_base_production', {
				base: this.base,
				kind: this.queue_candidate.production_kind,
				id: this.queue_candidate.id,
			});
			return true;
		});

		this.remove_button = this.frame.button({
			class: 'game-popup-button',
			text: '-',
			align: 'bottom right',
			bottom: 2,
			right: 2,
			width: 49,
		});
		this.remove_button.on('click', (e) => {
			if (this.base == null || #sizeof(this.queue) == 0) {
				return true;
			}
			this.p.game.event('remove_base_production', {
				base: this.base,
				index: #sizeof(this.queue) - 1,
			});
			return true;
		});
	},

	set: (data) => {
		this.base = data.base;
		this.production = data.production;
		this.queue = data.queue;
		this.set_candidates = data.set_candidates;
		this.queue_candidates = data.queue_candidates;
		this.set_candidates_by_key = {};
		this.queue_candidates_by_key = {};

		let set_items = [];
		if (!#is_defined(this.production)) {
			set_items :+['', 'NOTHING'];
		}
		for (candidate of this.set_candidates) {
			const key = this._candidate_key(candidate);
			this.set_candidates_by_key[key] = candidate;
			set_items :+[key, this._candidate_label(candidate)];
		}
		if (#sizeof(set_items) == 0) {
			set_items :+['', 'NOTHING'];
		}
		const set_candidates_signature = this._candidate_signature(
			this.set_candidates,
			#is_defined(this.production) ? 'CURRENT' : 'NOTHING'
		);
		if (set_candidates_signature != this.set_candidates_signature) {
			this.change_select.items = set_items;
			this.set_candidates_signature = set_candidates_signature;
		}
		this.change_select.readonly = #sizeof(this.set_candidates) == 0;
		this.change_select.value = #is_defined(this.production)
			? this._candidate_key(this.production)
			: '';

		let queue_items = [];
		for (candidate of this.queue_candidates) {
			const key = this._candidate_key(candidate);
			this.queue_candidates_by_key[key] = candidate;
			queue_items :+[key, this._candidate_label(candidate)];
		}
		if (#sizeof(queue_items) == 0) {
			queue_items :+['', 'QUEUE FULL'];
		}
		const previous_queue_key = this.queue_select.value;
		const queue_candidates_signature = this._candidate_signature(
			this.queue_candidates,
			'QUEUE FULL'
		);
		if (queue_candidates_signature != this.queue_candidates_signature) {
			this.queue_select.items = queue_items;
			this.queue_candidates_signature = queue_candidates_signature;
		}
		this.queue_select.readonly = #sizeof(this.queue_candidates) == 0;
		this.queue_candidate = #is_defined(this.queue_candidates_by_key[previous_queue_key])
			? this.queue_candidates_by_key[previous_queue_key]
			: (#sizeof(this.queue_candidates) > 0 ? this.queue_candidates[0] : null);
		this.queue_select.value = this.queue_candidate == null
			? ''
			: this._candidate_key(this.queue_candidate);

		this.items.clear();
		let i = 0;
		for (item of this.queue) {
			this.items.text({
				class: 'base-screen-frame-text',
				text: item.name,
			});
			i++;
			if (i == 8) {
				break;
			}
		}
		while (i < 8) {
			this.items.text({
				class: 'base-screen-frame-text',
				text: 'Empty Slot',
			});
			i++;
		}
	},

};
