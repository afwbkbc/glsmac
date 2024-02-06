// this should do nothing

test.assert(true, 'true is false');

let was_caught = false;
try {

    // this should do nothing aswell
    test.assert(true, 'true is false');

    // this should fail but get caught later
    test.assert(false, 'assert failed!');

} catch {
    TestError: (e) => {
        was_caught = true;
    }
}
test.assert(was_caught, 'exception was not caught');

let expected_type = '';
was_caught = false;
let testcatch = (t, f) => {
    expected_type = t;
    was_caught = false;
    try {
        f();
    } catch {
    :
        (e) => { // default handler
            test.assert(e.type == expected_type, 'expected:' + expected_type + ' got:' + e.type);
            was_caught = true;
        }
    }
    test.assert(was_caught, 'expected:' + expected_type + ' got nothing');
};

testcatch('TestError', () => {
    test.assert(false, 'let\'s fail this')
});
testcatch('GSETypeError', () => {
    5 && true
});
testcatch('GSETypeError', () => {
    let a = 'qwe';
    a++
});
testcatch('GSEReferenceError', () => {
    return non_existent_var
});
testcatch('GSEReferenceError', () => {
    non_existent_var = 5
});
testcatch('GSEReferenceError', () => {
    'asd'++
});
testcatch('GSEInvalidAssignment', () => {
    1 = 5
});
testcatch('GSEInvalidAssignment', () => {
    'asd' = 5
});
testcatch('GSEInvalidAssignment', () => {
    [1, 2] = 5
});
testcatch('GSEInvalidAssignment', () => {
    (a, b) => {
    }
    = 5
});
testcatch( 'GSEInvalidAssignment', () => {
    let var = 1;
    let var = 2;
});
testcatch( 'GSEInvalidAssignment', () => {
    const a = 1;
    a++;
});
testcatch( 'GSEInvalidAssignment', () => {
    let #builtin = 5;
});
testcatch( 'GSEInvalidAssignment', () => {
    #builtin = 5;
});
testcatch('GSEOperationNotSupported', () => {
    'asd' - 'qwe'
});
testcatch('GSEOperationNotSupported', () => {
    8 + 'zxc'
});
testcatch('GSEOperationNotSupported', () => {
    let a = 'asd';
    a -= 'zxc';
});
testcatch('GSEOperationNotSupported', () => {
    5 [] = 1
});
testcatch('GSEOperationNotSupported', () => {
    let a = 5;
    5 [] = 1;
});
testcatch('GSEOperationFailed', () => {
    {
        key: 'value1'
    }
    +{key: 'value2'}
});
testcatch('GSEInvalidDereference', () => {
    'asd'.test
});
testcatch('GSEInvalidDereference', () => {
    5[1]
});
testcatch('GSEInvalidDereference', () => {
    [1, [3, 4], 3][1][2][1]
});
testcatch('GSEInvalidDereference', () => {
    [1, 2, 3, 4]['asd'
:
    2
]
});
testcatch('GSEInvalidDereference', () => {
    [1, 2, 3, 4][0
:
    'qwe'
]
});
testcatch('GSEInvalidDereference', () => {
    [1, 2, 3, 4][-1
:
    1
]
});
testcatch('GSEInvalidDereference', () => {
    [1, 2, 3, 4][2
:
    1
]
});
testcatch('GSEInvalidDereference', () => {
    [1, 2, 3].property
});
testcatch('GSEInvalidDereference', () => {
    let v = 'asd';
    v.property
});
testcatch('GSEInvalidCall', () => {
    6()
});
testcatch('GSEInvalidCall', () => {
    (2 + 2)();
});
testcatch( 'GSEInvalidCall', () => {
    const a = () => {};
    a(1, 2, 3);
});
testcatch( 'GSEInvalidCall', () => {
    const a = (a,b,c) => {};
    a(1);
});
testcatch( 'GSELoaderError', () => {
    include( './non_existent_include' );
});
testcatch( 'GSEConversionError', () => {
    #to_int('asd');
});
testcatch( 'GSEConversionError', () => {
    #to_float('asd');
});
