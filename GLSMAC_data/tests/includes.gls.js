test.global_set('includes_executed', []);

// should be executed only once and cached
#include('./_include1');
#include('./_include1');
#include('./_include1');

let module2 = #include('./_include2');
let module1 = #include('./_include1');
test.assert(module1.id != module2.id);
// TODO: calls should have priority over .
test.assert(module1.id == (#include('./_include1')).id);
test.assert(module1.id != (#include('./_include2')).id);

module1.set_var('asdasd');
test.assert(test.global_get('include_var') == 'include_1_asdasd');
module2.set_var('qweqwe');
test.assert(test.global_get('include_var') == 'include_2_qweqwe');
module1.set_var('qweqwe');
test.assert(test.global_get('include_var') == 'include_1_qweqwe');

// each include must be executed only once (and cache returned later)
test.assert(test.global_get('includes_executed') == [1, 2]);

// test that scope of include is alive while object needs it
test.assert(test.global_get('include_testfunc1') == 'OK');
test.assert(test.global_get('include_testfunc2') == 'OK');
