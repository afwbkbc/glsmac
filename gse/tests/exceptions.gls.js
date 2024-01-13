// this should do nothing
test.assert(true, 'true is false');

let was_caught = false;
try {

    // this should do nothing aswell
    test.assert(true, 'true is false');

    // this should fail but get caught later
    test.assert(false, 'assert failed!');

} catch {
    TestError: (e) => {
        was_caught = true;
    }
}
test.assert(was_caught, 'exception was not caught');

let expected_type = '';
was_caught = false;
let testcatch = (t, f) => {
    expected_type = t;
    was_caught = false;
    try {
        f();
    } catch {
        : (e) => { // default handler
            test.assert(e.type == expected_type, 'expected:' + expected_type + ' got:' + e.type);
            was_caught = true;
        }
    }
    test.assert(was_caught, 'expected:' + expected_type + ' got nothing');
};

testcatch('TestError', () => { test.assert(false, 'let\'s fail this') });
testcatch('GSEVarNotDefined', () => { return non_existent_var });
testcatch('GSEVarNotDefined', () => { non_existent_var = 5 });
testcatch( 'GSEInvalidAssignment', () => { 1 = 5 });
testcatch( 'GSEInvalidAssignment', () => { 'asd' = 5 });
testcatch( 'GSEInvalidAssignment', () => { [1, 2] = 5 });
testcatch( 'GSEInvalidAssignment', () => { (a, b) => {} = 5 });
testcatch('GSEOperatorNotSupported', () => { 'asd' - 'qwe' });
testcatch('GSEOperationFailed', () => {{key: 'value1'} + {key: 'value2'}});


