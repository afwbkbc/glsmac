let a = 1;
{
	{
		{
			a++;
		}
		let a = 5;
		{
			test.assert(a == 5);
			a = 10;
			test.assert(a == 10);
		}
		test.assert(a == 10);
	}
	test.assert(a++ == 2);
}
test.assert(a == 3);
{
	;
}
{
	;
}
{
	;
}
