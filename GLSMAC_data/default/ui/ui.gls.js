return (glsmac) => {

	#include('styles/system')(glsmac.ui);

	const m = {
		utils: #include('utils/utils'),
		glsmac: glsmac,
	};
	for (module of [
		'smacpath_prompt',
		'intro',
		'mainmenu',
	]) {
		#include('modules/' + module)(m);
	}

};
