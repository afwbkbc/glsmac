// record execution
test.global_set('includes_executed', test.global_get('includes_executed') + [1]);

return {
	id: 1,
	set_var: (text) => {
		test.global_set('include_var', 'include_1_' + text);
	},
};
