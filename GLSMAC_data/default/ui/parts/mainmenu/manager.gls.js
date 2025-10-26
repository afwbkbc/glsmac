const manager = {

	modules: {},
	data_stack: [],

	exit_timer: null,

	register: (type, module) => {
		this.modules[type] = module;
	},

	push: (type, data) => {
		if (this.exit_timer != null) {
			this.exit_timer.stop();
			this.exit_timer = null;
		}
		this.data_stack :+ {
			type: type,
			data: data
		};
	},

	select: (index) => {
		this.data_stack[#sizeof(this.data_stack) - 1].index = index;
	},

	pop: (m, reset) => {
		const last_entry = this.data_stack :~;
		if (#is_defined(last_entry) && #is_defined(last_entry.data.destructor)) {
			last_entry.data.destructor();
		}
		if ( !#is_empty(this.data_stack)) {
			const entry = this.data_stack :~;
			this.modules[entry.type].show(entry.data);
			if ( #is_defined( entry.index ) ) {
				this.modules[entry.type].select(entry.index);
			}
		}
		else {
			// go async in case some other popup such as error gets displayed immediately after
			this.exit_timer = #async(0, () => {
				this.exit_timer = null;
				if ( reset ) {
					m.glsmac.reset();
				}
				else {
					m.glsmac.exit();
				}
			});
		}
	},

	close: (m) => {
		let last_entry = this.data_stack :~;
		while ( #is_defined( last_entry ) ) {
			if ( #is_defined(last_entry.data.destructor) ) {
				last_entry.data.destructor();
			}
			last_entry = this.data_stack :~;
		}
	},

};

return (type, module) => {
	manager.register(type, module);
	return manager;
};

