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

{
    let expected_type = '';
    let was_caught = false;
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

    testcatch('TestError', () => {
        test.assert(false);
    });
    testcatch('ReferenceError', () => {
        non_existent_var = 5;
    });

}
