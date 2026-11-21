return {

	validate: (e) => {

	},

	apply: (e) => {
		e.data.player.add_value(e.data.key, #to_float(e.data.value));
	},

	rollback: (e) => {
		e.data.player.add_value(e.data.key, 0.0 - #to_float(e.data.value));
	},

};
