#print('main begin');

let value = 1;
let values = [];

// increment value every 150ms
#async(150, () => {
	value++;
	#print('async1 ' + #to_string(value));
	values [] = value;
	return value < 75;
});

// double value every 700ms
#async(700, () => {
	value *= 2;
	#print('async2 ' + #to_string(value));
	values []= value;
	return value < 50;
});

// check collected values
#async( 3400, () => {
	#print('check');
	#print(values);
	test.assert(values == [ 2, 3, 4, 5, 10, 11, 12, 13, 14, 15, 30, 31, 32, 33, 34, 68, 69, 70, 71, 72, 73, 74, 75 ]);

	return false;
});

#print('main end');
test.assert(values == []);
