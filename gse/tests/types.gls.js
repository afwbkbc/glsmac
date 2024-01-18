// test script

let a = 5;
a++;
const a_const = 6;
test.assert(a == a_const);
let b = a + 2 * 4;
let c=(a+2)*4;
{
	a = 15;
	a += 10;
};
c = 123;
c -= 23;

let testmethod1 = (a, b, c) => { return a + b + c; };

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
let testarr2 = [ 3, 'TEST', {
  key1: 'value1',
  key2: 'value2',
} ];
testarr1 []= 'first';
testarr1 []= 'second';
testarr1 []= 1 + 2 + 3;
testarr1 += testarr2;
testarr1 []= testarr2;
let testarr3 = testarr1;
testarr3[1] = 'SECOND';
testarr3[ testmethod2(a, b, c) + 61 ] = 'FIRST';
testarr3[2:5] = testarr1[0:1] + testarr2[0:1];
let testarr4 = testarr3[:3];
testarr4[ c + 1 - 100 : c - 100 + 2 ] = [ 'new first', 'new second' ];

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

let d = null;
let x = a > b;

test.assert(d == null);
test.assert( x );
test.assert( x != b > c );
test.assert( a != b );
test.assert( b != c );
test.assert( c != a );
test.assert( a == a );
test.assert( a > b );
test.assert( !(b > c) );
test.assert( !(b >= a) );
test.assert( a >= 2 );
test.assert( c <= 200 );
test.assert( a <= 200 );
test.assert( !(10 < 10) );
test.assert( 10 <= 10 );
test.assert( !(a < a) );
test.assert( a <= a );
test.assert( true && true );
test.assert( !(true && false) );
test.assert( true || true );
test.assert( true || false );
test.assert( (( 5 > 10 ) && ( 2 > 1 )) || (( 5 <= 10 ) && !( 5 > 35 ) && ( 100 >= 20 )) );
test.assert( testmethod1(11, b, 20) == 45 );
test.assert( testmethod2(a, b, c) == -61 );

let testmethod = testmethod1;
test.assert( testmethod( 1, testmethod( 2, testmethod( 3, 3, 3 ), testmethod( 4, 4, 4 ) ), testmethod( 5, 5, testmethod( 6, 6, 6 )) ) == 52 );

test.assert( testarr1[0] == 'first' );
test.assert( testarr1[1] == 'second' );
test.assert( testarr1[2] == 6 );
test.assert( testarr1[3] == 3 );
test.assert( testarr1[4] == 'TEST' );
test.assert( testarr1[5].key1 == 'value1' );
test.assert( testarr1[5].key2 == 'value2' );

let o = testarr1[5];
test.assert( o.key1 == 'value1' );
test.assert( o.key2 == 'value2' );

test.assert( testarr1[6] == testarr2 );

test.assert( testarr2[0] == 3 );
test.assert( testarr2[1] == 'TEST' );

test.assert( testarr2[2] == o );

test.assert( testarr3[0] == 'FIRST' );
test.assert( testarr3[1] == 'SECOND' );
test.assert( testarr3[2] == 'first' );
test.assert( testarr3[3] == 'second' );
test.assert( testarr3[4] == 3 );
test.assert( testarr3[5] == 'TEST' );
test.assert( testarr3[6] == testarr2 );

test.assert( testarr4[0] == 'FIRST' );
test.assert( testarr4[1] == 'new first' );
test.assert( testarr4[2] == 'new second' );
test.assert( testarr4[3] == 'second' );

test.assert( testarr1[0:1] == [ 'first', 'second' ] );

test.assert( testarr1[5:] == [ testarr1[5], testarr1[6] ] );
test.assert( testarr1[:3] == [ testarr1[0], testarr1[1] ] + testarr1[2:3] );
test.assert( testarr1[4:5] + testarr1[2:3] == [ testarr1[4], testarr1[5], testarr1[2], testarr1[3] ] );
test.assert(testobj3.child1.child2.value == 'CHILD VALUE');
test.assert(testobj1.propertyInt == 272 + c);
test.assert(testobj1 == { propertyInt: 372 } );
test.assert(testobj2.propertyInt1 == 150 );
test.assert(testobj2.propertyInt2 == 222 );
test.assert(testobj2.propertyString == 'STRING' );

let yes_or_no = false;
if ( a > b ) {
  yes_or_no = true;
}
else {
  yes_or_no = false;
};
test.assert( yes_or_no );

if ( b > a ) {
  yes_or_no = true;
}
else {
  yes_or_no = false;
};
test.assert( !yes_or_no );

if ( false ) { test.assert(false); };
if ( false ) {
  test.assert(false);
} elseif ( false ) {
  test.assert(false);
} elseif ( true ) {
  yes_or_no = true;
} else {
  test.assert(false);
};
test.assert( yes_or_no );

let arr = [];
let i = 0;
while ( i++ < 5 ) {
  arr []= i;
};
test.assert( arr == [1, 2, 3, 4, 5] );

arr = [];
try {
  arr []= 'BEFORE EXCEPTION'; // should be printed
  let failfunc = () => {
    arr []= 'failfunc';
    throw TestError('something happened');
  };
  failfunc();
  arr []= 'AFTER EXCEPTION'; // should not be printed
}
catch {
  UnknownError: (e) => {
    arr []= 'shouldnt catch this';
  },
  TestError: (e) => {
    arr []= 'CAUGHT ' + e.type + ' : ' + e.reason;
    arr += e.backtrace;
  }
};

test.assert( arr == [
    'BEFORE EXCEPTION',
    'failfunc',
    'CAUGHT TestError : something happened',
    '\tat ' + test.get_script_path() + ':179: throw TestError(\'something happened\');',
    '\tat ' + test.get_script_path() + ':181: failfunc();'
] );

;;;
