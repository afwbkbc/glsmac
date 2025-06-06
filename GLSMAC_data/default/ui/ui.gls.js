return (glsmac) => {

	for (style of ['system', 'popup']) {
		#include('styles/' + style)(glsmac.ui);
	}

	const m = {
		utils: #include('utils/utils'),
		glsmac: glsmac,
		root: glsmac.ui.root,
	};
	for (step of [
		'smacpath_prompt',
		'intro',
		'mainmenu/mainmenu',
		'loader',
		'game',
	]) {
		#include('parts/' + step)(m);
	}

};
