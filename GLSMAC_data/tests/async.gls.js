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
#async( 4000, () => {
	#print('check');
	#print(values);
	test.assert(values == [ 2, 3, 4, 5, 10, 11, 12, 13, 14, 15, 30, 31, 32, 33, 34, 68, 69, 70, 71, 72, 73, 74, 75 ]);

	// next tests

	// async should respect scopes
	{
		#print('scope1 begin');

		const value = 1;
		#async(150, () => {
			#print('async1');
			test.assert(value == 1);
		});

		#print('scope1 end');
	}
	{
		#print('scope2 begin');

		const value = 2;
		#async(200, () => {
			#print('async2', value == 2);
		});

		#print('scope2 end');
	}

	// recursive async should work too
	let result = [];
	#async(500, () => {
		#print('async parent begin');
		let i = 0;
		result []= 'P';
		#async(200, () => {
			const value = i;
			result []= 'C' + #to_string(i);
			#print('async child begin', value);
			let ii = 0;
			#async(50, () => {
				result []= 'CC' + #to_string(value);
				#print('async child child', value);
				return ++ii < 8;
			});
			#print('async child end', value);
			return ++i < 5;
		});
		#print('async parent end');
	});

	// check result
	#async( 3000, () => {
		#print(result);
		test.assert(result == [ 'P', 'C0', 'CC0', 'CC0', 'CC0', 'C1', 'CC0', 'CC1', 'CC0', 'CC1', 'CC0', 'CC1', 'CC0', 'C2', 'CC1', 'CC0', 'CC2', 'CC1', 'CC2', 'CC1', 'CC2', 'CC1', 'C3', 'CC2', 'CC1', 'CC3', 'CC2', 'CC3', 'CC2', 'CC3', 'CC2', 'C4', 'CC3', 'CC2', 'CC4', 'CC3', 'CC4', 'CC3', 'CC4', 'CC3', 'CC4', 'CC3', 'CC4', 'CC4', 'CC4', 'CC4' ] );
	});


	return false;
});

#print('main end');
test.assert(values == []);
