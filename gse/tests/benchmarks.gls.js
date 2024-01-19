// measure duration of something in nanoseconds
let average_call_time_ns = 0;
const timeof = (label, f) => {
    console.global_mute();
    const begin = test.get_current_time_nano();
    f();
    const ns = test.get_current_time_nano() - begin - average_call_time_ns;
    console.global_unmute();
    if (label != '') {
        console.log(label, ':', ns, 'ns');
    }
    return ns;
};

// determine average time of function call and timeof internals, for more accuracy later
{
    const times = 10; // use average of 10 calls
    let total = 0;
    let i = 0;
    while (i++ < times) {
        total += timeof('', () => {
        });
    }
    ;
    average_call_time_ns = total / times;
}

test.assert(timeof('empty loop with 100000 iterations', () => {
    let i = 0;
    while (i++ < 100000) {
    }
}) < 1000000000);

let idx = 0;
let arr = [];
test.assert(timeof('appending 100000 elements to array', () => {
    idx = 0;
    while ( idx < 100000 ) {
        arr []= idx++;
    }
}) < 3000000000);

let sum = 0;
test.assert(timeof('reading and summing 100000 elements from array', () => {
    idx = 0;
    while (idx < 100000) {
        sum += arr[idx++];
    }
}) < 5000000000);
test.assert(sum == 4999950000);

test.assert(timeof('defining and calling function 100000 times', () => {
    idx = 0;
    while ( idx++ < 100000 ) {
        (() => {})();
    }
}) < 10000000000);

test.assert(timeof('creating scope and variable 100000 times', () => {
    idx = 0;
    while ( idx++ < 100000 ) {
        let v = 0;
    }
}) < 5000000000);

const recursivefunc = (x, times) => {
    if ( times == 0 ) {
        return x;
    }
    else {
        return recursivefunc( x + 1, times - 1 );
    }
};
test.assert(timeof('calling recursive function 100 times with depth 1000', () => {
    idx = 0;
    while ( idx++ < 100 ) {
        recursivefunc(0, 1000);
    }
}) < 15000000000);
