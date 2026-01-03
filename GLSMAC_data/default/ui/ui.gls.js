return (glsmac) => {

	for (style of ['system', 'popup', 'lobby']) {
		#include('styles/' + style)(glsmac.ui);
	}

	const m = {
		utils: #include('utils/utils'),
		glsmac: glsmac,
		ui: glsmac.ui,
		root: glsmac.ui.root,
	};
	for (step of [
		'smacpath_prompt',
		'intro',
		'mainmenu/mainmenu',
		'error',
		'loader',
		'game/game',
	]) {
		#include('parts/' + step)(m);
	}

};
