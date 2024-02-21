// record execution
test.global_set('includes_executed', test.global_get('includes_executed') + [2]);

return {
    id: 2,
    set_var: (text) => {
        test.global_set('include_var', 'include_2_' + text);
    },
};
