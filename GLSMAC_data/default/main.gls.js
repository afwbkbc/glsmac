#main((glsmac) => {

	// backwards compatibility
	if (#classof(glsmac) != 'GLSMAC') {
		(#include('main_legacy'))(glsmac);
		return;
	}

	#print('before', glsmac.ui.root);

	let r = glsmac.ui.root; // TODO: copy as ref

	glsmac.ui.root.key1 = 'qwe';
	glsmac.ui.root.key2 = 'asd';
	glsmac.ui.root.key3 = 'zxczxczxc';
	r.key3 = 'fghfdghgfdhgdfh'; // TODO: make this work ( need to copy as ref )

	#print('after', glsmac.ui.root);
	#print('r', r);

});
