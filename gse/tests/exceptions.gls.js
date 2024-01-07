// this should do nothing
test.assert(true, 'true is false');

try {

    // this should do nothing aswell
    test.assert(true, 'true is false');

    // this should fail but get caught later
    test.assert(false, 'assert failed!');

} catch {
    TestError: (e) => {
        return null; // test successful
    }
}
;

// check if assertion was caught (otherwise either assert or try..catch doesn't work correctly)
test.assert(false, 'exception was not caught');
