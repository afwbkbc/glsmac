return (glsmac) => {

	#include('styles/system')(glsmac.ui);

	const m = {
		utils: #include('utils/utils'),
		glsmac: glsmac,
		root: glsmac.ui.root,
	};
	for (step of [
		'smacpath_prompt',
		'intro',
		'mainmenu',
		'loader',
		'game',
	]) {
		#include('parts/' + step)(m);
	}

};
