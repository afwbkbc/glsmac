// test script

let a = 5;
a++;
const a_const = 6;
test.assert(a == a_const);
let b = a + 2 * 4;
let c = (a + 2) * 4;
{
	a = 15;
	a += 10;
}
c = 123;
c -= 23;

let emptymethod = () => {};
test.assert(#typeof(emptymethod()) == 'Undefined');

let ret = 0;
let testmethod0 = () => {
	ret = 1;
	return;
	ret = 2;
};
test.assert(#typeof(testmethod0()) == 'Undefined');
test.assert(ret == 1);

let testmethod1 = (a, b, c) => {
	return a + b + c;
};

let testmethod2 = (a, b, c) => {
	/*
		this method is a bit different
	*/
	return
	a
	+
	b
	-
	c
	;
};

let testarr1 = [];
let testarr2 = [3, 'TEST', {
	key1: 'value1',
	key2: 'value2',
}];
testarr1 [] = 'first';
testarr1 [] = 'second';
testarr1 [] = 1 + 2 + 3;
testarr1 += testarr2;
testarr1 [] = testarr2;
let testarr3 = #clone(testarr1);
testarr3[1] = 'SECOND';
testarr3[testmethod2(a, b, c) + 61] = 'FIRST';
testarr3[2:5] = testarr1[0:1] + testarr2[0:1];
let testarr4 = testarr3[:3];
testarr4[ c + 1 - 100 : c - 100 + 2 ] = ['new first', 'new second'];

let testobj1 = {};
let testobj2 = {
	propertyString: 'STRING',
	propertyInt1: 111 + a + b,
	propertyInt2: 222,
};
let testobj3 = {
	child1: {
		child2: {
			value: 'CHILD VALUE'
		}
	},
};
testobj1.propertyInt = testobj2.propertyInt1 + testobj2.propertyInt2;

let testobj4 = {
	myvalue: 'my own value',
	myvalue2: this.myvalue + ' 2',
};
test.assert(testobj4.myvalue2 == 'my own value 2');
let testobj5 = {
	myvalue: 'some value',
	testfunc: () => {
		let internalobj = {
			myvalue: 'some internal value',
			testfunc: () => {
				return this.myvalue;
			}
		};
		return this.myvalue + ',' + internalobj.testfunc();
	}
};
test.assert(testobj5.testfunc() == 'some value,some internal value');

let d = null;
let x = a > b;

test.assert(d == null);
test.assert(x);
test.assert(x != b > c);
test.assert(a != b);
test.assert(b != c);
test.assert(c != a);
test.assert(a == a);
test.assert(a > b);
test.assert(!(b > c));
test.assert(!(b >= a));
test.assert(a >= 2);
test.assert(c <= 200);
test.assert(a <= 200);
test.assert(!(10 < 10));
test.assert(10 <= 10);
test.assert(!(a < a));
test.assert(a <= a);
test.assert(true && true);
test.assert(!(true && false));
test.assert(true || true);
test.assert(true || false);
test.assert(((5 > 10) && (2 > 1)) || ((5 <= 10) && !(5 > 35) && (100 >= 20)));
test.assert(testmethod1(11, b, 20) == 45);
test.assert(testmethod2(a, b, c) == -61);

let testmethod = testmethod1;
test.assert(testmethod(1, testmethod(2, testmethod(3, 3, 3), testmethod(4, 4, 4)), testmethod(5, 5, testmethod(6, 6, 6))) == 52);

test.assert(testarr1[0] == 'first');
test.assert(testarr1[1] == 'second');
test.assert(testarr1[2] == 6);
test.assert(testarr1[3] == 3);
test.assert(testarr1[4] == 'TEST');
test.assert(testarr1[5].key1 == 'value1');
test.assert(testarr1[5].key2 == 'value2');

let o = testarr1[5];
test.assert(o.key1 == 'value1');
test.assert(o.key2 == 'value2');

test.assert(testarr1[6] == testarr2);

test.assert(testarr2[0] == 3);
test.assert(testarr2[1] == 'TEST');

test.assert(testarr2[2] == o);

test.assert(testarr3[0] == 'FIRST');
test.assert(testarr3[1] == 'SECOND');
test.assert(testarr3[2] == 'first');
test.assert(testarr3[3] == 'second');
test.assert(testarr3[4] == 3);
test.assert(testarr3[5] == 'TEST');
test.assert(testarr3[6] == testarr2);

test.assert(testarr4[0] == 'FIRST');
test.assert(testarr4[1] == 'new first');
test.assert(testarr4[2] == 'new second');
test.assert(testarr4[3] == 'second');

test.assert(testarr1[0:1] == ['first', 'second']);

test.assert(testarr1[5:] == [testarr1[5], testarr1[6]]);
test.assert(testarr1[:3] == [testarr1[0], testarr1[1]] + testarr1[2:3] );
test.assert(testarr1[4:5] + testarr1[2:3] == [testarr1[4], testarr1[5], testarr1[2], testarr1[3]]);
test.assert(testobj3.child1.child2.value == 'CHILD VALUE');
test.assert(testobj1.propertyInt == 272 + c);
test.assert(testobj1 == {propertyInt: 372});
test.assert(testobj2.propertyInt1 == 150);
test.assert(testobj2.propertyInt2 == 222);
test.assert(testobj2.propertyString == 'STRING');

let yes_or_no = false;
if (a > b) {
	yes_or_no = true;
} else {
	yes_or_no = false;
}
;
test.assert(yes_or_no);

if (b > a) {
	yes_or_no = true;
} else {
	yes_or_no = false;
}
;
test.assert(!yes_or_no);

if (false) {
	test.assert(false);
}
if (false) {
	test.assert(false);
}
elseif(false)
{
	test.assert(false);
}
elseif(true)
{
	yes_or_no = true;
}
else
{
	test.assert(false);
}
test.assert(yes_or_no);

let arr = [];
let i = 0;
while (i++ < 5) {
	arr [] = i;
}
test.assert(arr == [1, 2, 3, 4, 5]);

const data = ['a', 'b', 'c'];

arr = [];
for (i in data) {
	arr []= #to_string(i) + '_' + data[i];
}
test.assert(arr == ['0_a', '1_b', '2_c']);
for (i in ['asd', 'qwe', 'zxc']) {
	arr []= i;
}
test.assert(arr == ['0_a', '1_b', '2_c', 0, 1, 2]);
for (i in {
	key1: 'value1',
	key2: 'value2',
}) {
	arr []= i;
}
test.assert(arr == ['0_a', '1_b', '2_c', 0, 1, 2, 'key1', 'key2']);

arr = [];
for (v of data) {
	arr []= 'of_' + v;
}
test.assert(arr == ['of_a', 'of_b', 'of_c']);
for (v of ['asd', 'qwe', 'zxc', (5 + 10), {k: 'v'}, ((x) => { return x * 2 })(5)]) {
	arr []= v;
}
test.assert(arr == [ 'of_a', 'of_b', 'of_c', 'asd', 'qwe', 'zxc', 15, {k: 'v'}, 10]);
for (i of {
	key1: 'value1',
	key2: 'value2',
}) {
	arr []= i;
}
test.assert(arr == [ 'of_a', 'of_b', 'of_c', 'asd', 'qwe', 'zxc', 15, {k: 'v'}, 10, 'value1', 'value2']);

arr = [];
for (let ii = 2 ; ii > 0 ; ii--) {
	arr []= 'i_' + data[ii];
}
test.assert(arr == ['i_c', 'i_b']);

for (i = 5 ; i <= 10 ; i++) {
	arr []= i;
}
test.assert(arr == ['i_c', 'i_b', 5, 6, 7, 8, 9, 10 ]);

const for_func = () => {
	for ( i = 0 ; i < 10 ; i++ ) {
		if ( i == 6 ) {
			return i;
		}
	}
};
test.assert(for_func() == 6);

arr = [];
for ( i = 0 ; i < 10 ; i++ ) {
	arr []= i;
	if (i >= 3) {
		arr []= 'x';
		for ( i = 5 ; i < 10 ; i++ ) {
			if ( i == 7 ) {
				break;
			}
			arr []= i;
		}
		arr []= 'y';
		break;
	}
}
test.assert(arr == [0, 1, 2, 3, 'x', 5, 6, 'y']);

arr = [];
i = 5;
while (i > 0) {
	if (i == 2 ) {
		break;
	}
	arr []= i;
	i--;
}
test.assert(arr == [5, 4, 3]);

arr = [];
for ( i of [4, 7, 1, 5] ) {
	if ( i == 1 ) {
		break;
	}
	arr []= i;
}
test.assert(arr == [4, 7]);

arr = [];
for ( i = 0 ; i < 10 ; i++ ) {
	if ( i < 5 || i > 8 ) {
		continue;
	}
	arr []= i;
}
test.assert(arr == [5, 6, 7, 8]);

arr = [];
i = 10;
while (i > 0) {
	i--;
	if (i > 7 || i < 3) {
		for ( ii of ['a', 'b', 'c', 'd', 'e', 'f'] ) {
			if ( ii == 'a' || ii == 'c' ) {
				continue;
			}
			elseif (ii == 'e') {
				break;
			}
			arr []= ii;
		}
		continue;
	}
	arr []= i;
}
test.assert(arr == ['b', 'd', 'b', 'd', 7, 6, 5, 4, 3, 'b', 'd', 'b', 'd', 'b', 'd']);

arr = [];
try {
	arr [] = 'BEFORE EXCEPTION'; // should be printed
	let realfailfunc = () => {
		arr [] = 'failfunc2';
		{
			throw TestError('something happened');
		}
	};
	let failfunc = () => {
		arr [] = 'failfunc';
		realfailfunc();
	};
	failfunc();
	arr [] = 'AFTER EXCEPTION'; // should not be printed
} catch {
	UnknownError: (e) => {
		arr [] = 'shouldnt catch this';
	},
	TestError: (e) => {
		arr [] = 'CAUGHT ' + e.type + ' : ' + e.reason;
		arr += e.backtrace;
	}
};

test.assert(arr == [
	'BEFORE EXCEPTION',
	'failfunc',
	'failfunc2',
	'CAUGHT TestError : something happened',
	'\tat ' + test.get_script_path() + ':343: throw TestError(\'something happened\');',
	'\tat ' + test.get_script_path() + ':348: realfailfunc();',
	'\tat ' + test.get_script_path() + ':350: failfunc();'
]);

test.assert(#to_string(2 + 3) + ' (five)' == '5 (five)');
test.assert(#to_string(#to_float(#to_string(#to_int('1' + '2') + 55) + '1')) == '671.000000');
test.assert(#to_int(#to_string(2 + 3) + '2') * 123 == 6396);

test.assert(#to_string('string') == 'string');
test.assert(#to_int(123) == 123);

test.assert(#typeof(null) == 'Null');
test.assert(#typeof(true) == 'Bool');
test.assert(#typeof(123) == 'Int');
test.assert(#typeof(123.45) == 'Float');
test.assert(#typeof(123.0) == 'Float');
test.assert(#typeof(123.) == 'Float');
test.assert(#typeof(0.123) == 'Float');
test.assert(#typeof('string') == 'String');
test.assert(#typeof([]) == 'Array');
test.assert(#typeof(2:3) == 'Range');
test.assert(#typeof({}) == 'Object');
test.assert(#typeof(() => {}) == 'Callable');

test.assert(15.0 != 15);
test.assert(#round(15.0) == 15);
test.assert(#round(15.4) == 15);
test.assert(#round(15.6) == 16);


let testobj = {
	key1: 'value1',
	key2: {
		k1: 'v1',
		k2: 'v2',
	}
};
let testref = testobj;
let testclone = #clone(testobj);
testref.key1 = 'VALUE1';
test.assert(testobj.key1 == 'VALUE1');
test.assert(testref.key1 == 'VALUE1');
test.assert(testclone.key1 == 'value1');
testclone.key1 = 'VaLuE1';
test.assert(testobj.key1 == 'VALUE1');
test.assert(testref.key1 == 'VALUE1');
test.assert(testclone.key1 == 'VaLuE1');
testref = testclone;
test.assert(testobj.key1 == 'VALUE1');
test.assert(testref.key1 == 'VaLuE1');
test.assert(testclone.key1 == 'VaLuE1');
testref.key1 = 'value1';
test.assert(testobj.key1 == 'VALUE1');
test.assert(testref.key1 == 'value1');
test.assert(testclone.key1 == 'value1');

let testint = 5;
let testcopy = testint;
test.assert(testint == 5);
test.assert(testcopy == 5);
testcopy = 4;
test.assert(testint == 5);
test.assert(testcopy == 4);
testint = 6;
test.assert(testint == 6);
test.assert(testcopy == 4);

let invalidclone = null;
let was_caught = false;
try {
	invalidclone = #clone(testint);
} catch {
	: (e) => {
		was_caught = true;
	}
}
test.assert(invalidclone == null);
test.assert(was_caught == true);

;
;
;
