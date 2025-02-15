// record execution
test.global_set('includes_executed', test.global_get('includes_executed') + [2]);

const testfunc1 = () => {
	test.global_set('include_testfunc1', 'OK');
};

return {
	id: 2,
	testfunc2: () => {
		test.global_set('include_testfunc2', 'OK');
	},
	set_var: (text) => {
		test.global_set('include_var', 'include_2_' + text);
		testfunc1();
		this.testfunc2();
	},
};
