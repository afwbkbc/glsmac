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
