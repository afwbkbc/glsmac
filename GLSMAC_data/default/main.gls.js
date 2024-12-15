#main((glsmac) => {

	const is_new = (#to_string(glsmac))[0:7] == '#glsmac';

	if ( !is_new ) {
		(#include('main_legacy'))(glsmac);
		return;
	}

	#print('TODO');

});
