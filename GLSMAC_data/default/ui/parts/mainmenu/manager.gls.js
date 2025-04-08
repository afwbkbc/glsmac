const manager = {

	modules: {},
	data_stack: [],

	register: (type, module) => {
		this.modules[type] = module;
	},

	push: (type, data) => {
		this.data_stack :+ [type, data];
	},

	pop: (m) => {
		this.data_stack :~;
		if ( !#is_empty(this.data_stack)) {
			const type_and_data = this.data_stack:~;
			this.modules[type_and_data[0]].show(type_and_data[1]);
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

