let var1 = 1;

console.log('here 1', var1);

let func = () => {
    console.log('here in 1', var1);
    test.assert(var1 == 1);
    var1 = 2;
    console.log('here in 2', var1);
};

console.log('here 2', var1);
test.assert(var1 == 1);
func();
console.log('here 3', var1);
test.assert(var1 == 2);

