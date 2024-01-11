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

was_caught = false;
try {
    non_existent_var = 1;
} catch {
    ReferenceError: (e) => {
        was_caught = true;
    }
}
test.assert(was_caught, 'exception was not caught');
