#main((glsmac) => {

	// backwards compatibility
	if (#classof(glsmac) != 'GLSMAC') {
		(#include('main_legacy'))(glsmac);
		return;
	}

	#include('ui/ui')(glsmac);

	glsmac.start();

});
