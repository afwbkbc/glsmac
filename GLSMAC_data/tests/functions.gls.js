let var1 = 1;

let func = () => {
	test.assert(var1 == 1);
	var1 = 2;
	test.assert(var1 == 2);
};

test.assert(var1 == 1);
func();
test.assert(var1 == 2);

let lambda1 = (a, b, c) => {
	return a * 100 + b * 10 + c;
};
let lambda2 = (f, a, b) => {
	return f(a + b, a - b, a * b);
};
let lambda3 = (f1, f2) => {
	return f1(f2, 1, 2);
};
test.assert(lambda3(lambda2, lambda1) == 292);

test.assert((a, b, c) => {
	return a + b + c;
}(1, 2, 3) == 6
)
;

let arr_of_func_objs = [
	{
		v: () => {
			return 'A';
		}
	},
	{
		v: () => {
			return 'B';
		}
	},
];
test.assert(arr_of_func_objs[0].v() == 'A');
test.assert(arr_of_func_objs[1].v() == 'B');

let f = () => {
	return 'Y'
};
test.assert('X' + f() == 'XY');

{
	{
		let aa = null;
		{
			let a = null;
			{
				let aa = 5;
				a = () => {
					return aa;
				};
				#print(a());
			}
			aa = a;
		}
		#print(aa());
	}

	let func1 = null;
	let func2 = null;
	{
		const by1 = 1;
		func1 = (x) => {
			#print('func1', x);
			if (x <= 0) {
				return x;
			} else {
				return func2(x - by1);
			}
		}
	}
	{
		const by2 = 2;
		func2 = (x) => {
			#print('func2', x);
			if (x <= 0) {
				return x;
			} else {
				return func1(x - by2);
			}
		}
	}
	func1(12);
	func2(34);
}

let called1 = false;
let called2 = false;
const testfunc = () => {
	called2 = true;
};

const testobjfunc = () => {
	return {
		key: 'value',
		func: () => {
			called1 = true;
			testfunc();
		}
	};
};

testobjfunc().func();
test.assert(called1);
test.assert(called2);
