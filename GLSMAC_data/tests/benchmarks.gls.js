const softassert = (condition) => {
	if (!condition) {
		#print('Too slow!');
	}
};

// measure duration of something in nanoseconds
let average_call_time_ns = 0;
const timeof = (label, f) => {
	#global_mute();
	const begin = test.get_current_time_nano();
	f();
	const ns = test.get_current_time_nano() - begin - average_call_time_ns;
	#global_unmute();
	if (label != '') {
		#print(label, ':', ns, 'ns');
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

softassert(timeof('empty loop with 10000 iterations', () => {
	let i = 0;
	while (i++ < 10000) {
	}
}) < 350000000);

let idx = 0;
let arr = [];
softassert(timeof('appending 10000 elements to array', () => {
	idx = 0;
	while (idx < 10000) {
		arr [] = idx++;
	}
}) < 350000000);

let sum = 0;
softassert(timeof('reading and summing 10000 elements from array', () => {
	idx = 0;
	while (idx < 10000) {
		sum += arr[idx++];
	}
}) < 500000000);
#print(sum);
test.assert(sum == 49995000);

softassert(timeof('defining and calling function 10000 times', () => {
	idx = 0;
	while (idx++ < 10000) {
		(() => {
		})();
	}
}) < 1000000000);
/*
softassert(timeof('creating scope and variable 10000 times', () => {
	idx = 0;
	while (idx++ < 10000) {
		let v = 0;
	}
}) < 500000000);

const recursivefunc = (x, times) => {
	if (times == 0) {
		return x;
	} else {
		return recursivefunc(x + 1, times - 1);
	}
};

// on linux this works with depth 1000, on windows there's stack overflow with depth 50
softassert(timeof('calling recursive function 400 times with depth 25', () => {
	idx = 0;
	while (idx++ < 400) {
		recursivefunc(0, 25);
	}
}) < 1500000000);
*/