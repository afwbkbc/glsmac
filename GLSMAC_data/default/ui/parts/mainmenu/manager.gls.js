const manager = {

	modules: {},
	data_stack: [],

	register: (type, module) => {
		this.modules[type] = module;
	},

	push: (type, data) => {
		this.data_stack :+ {
			type: type,
			data: data
		};
	},

	select: (index) => {
		this.data_stack[#sizeof(this.data_stack) - 1].index = index;
	},

	pop: (m) => {
		this.data_stack :~;
		if ( !#is_empty(this.data_stack)) {
			const entry = this.data_stack :~;
			this.modules[entry.type].show(entry.data);
			if ( #is_defined( entry.index ) ) {
				this.modules[entry.type].select(entry.index);
			}
		}
		else {
			m.glsmac.exit();
		}
	},

};

return (type, module) => {
	manager.register(type, module);
	return manager;
};

