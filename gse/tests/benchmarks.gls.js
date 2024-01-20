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

test.assert(timeof('empty loop with 10000 iterations', () => {
    let i = 0;
    while (i++ < 10000) {
    }
}) < 100000000);

let idx = 0;
let arr = [];
test.assert(timeof('appending 10000 elements to array', () => {
    idx = 0;
    while ( idx < 10000 ) {
        arr []= idx++;
    }
}) < 300000000);

let sum = 0;
test.assert(timeof('reading and summing 10000 elements from array', () => {
    idx = 0;
    while (idx < 10000) {
        sum += arr[idx++];
    }
}) < 500000000);
console.log(sum);
test.assert(sum == 49995000);

test.assert(timeof('defining and calling function 10000 times', () => {
    idx = 0;
    while ( idx++ < 10000 ) {
        (() => {})();
    }
}) < 1000000000);

test.assert(timeof('creating scope and variable 10000 times', () => {
    idx = 0;
    while ( idx++ < 10000 ) {
        let v = 0;
    }
}) < 500000000);

const recursivefunc = (x, times) => {
    if ( times == 0 ) {
        return x;
    }
    else {
        return recursivefunc( x + 1, times - 1 );
    }
};
test.assert(timeof('calling recursive function 10 times with depth 1000', () => {
    idx = 0;
    while ( idx++ < 10 ) {
        recursivefunc(0, 1000);
    }
}) < 1500000000);
