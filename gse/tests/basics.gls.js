// test script

let a = 5;
a++;
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

console.log( d );
console.log( d == null );
console.log( x, x == b > c );

console.log( a != b, b != c, c != a, a != a );
console.log( a > b, b > c );
console.log( b >= a, a >= 2, c <= 200, a <= 200 );
console.log( 10 < 10, 10 <= 10, a < a, a <= a );
console.log( true && true, true && false, true || true, true || false );
console.log( (( 5 > 10 ) && ( 2 > 1 )) || (( 5 <= 10 ) && !( 5 > 35 ) && ( 100 >= 20 )) );
console.log(testmethod1(11, b, 20), testmethod2(a, b, c));
let testmethod = testmethod1;
console.log( testmethod( 1, testmethod( 2, testmethod( 3, 3, 3 ), testmethod( 4, 4, 4 ) ), testmethod( 5, 5, testmethod( 6, 6, 6 )) ), 10 );
console.log( testarr1 ); console.log( testarr2 ); console.log( testarr3 ); console.log( testarr4 );
console.log( testarr1[0] ); console.log( testarr1[1] ); console.log( testarr1[0:1] );
console.log( testarr1[5:] ); console.log( testarr1[:3] );
console.log( testarr1[4:5] + testarr1[2:3] );
console.log(testobj3.child1.child2.value);
console.log(testobj1.propertyInt == 272 + c); console.log(testobj1, testobj2);

if ( a > b ) {
  console.log( 'YES' );
}
else {
  console.log( 'NO' );
};
if ( b > a ) {
  console.log( 'YES' );
}
else {
  console.log( 'NO' );
};
if ( false ) { console.log( 'FALSE' ); };
if ( false ) {
  console.log('FAIL');
} elseif ( false ) {
  console.log( 'FAIL' );
} elseif ( true ) {
  console.log( 'OK' );
} else {
  console.log( 'FAIL' );
};

let i = 0;
while ( i++ < 5 ) {
  console.log(i);
};

try {
  console.log( 'BEFORE EXCEPTION' ); // should be printed
  let failfunc = () => {
    console.log('failfunc');
    throw TestError('something happened');
  };
  failfunc();
  console.log( 'AFTER EXCEPTION' ); // should not be printed
}
catch {
  UnknownError: (e) => {
    console.log('shouldnt catch this');
  },
  TestError: (e) => {
    console.log('CAUGHT ' + e.type + ' : ' + e.reason);
    console.log(e.backtrace);
  }
};

;;;
console.log('bye!');;
