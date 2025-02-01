let escape_handler_id = 0;

/*const quit_on_escape_off = (ui) => {
	if (escape_handler_id) {
		ui.root.off('keydown', escape_handler_id);
		escape_handler_id = 0;
	}
}*/

#main((glsmac) => {

	// backwards compatibility
	if (#classof(glsmac) != 'GLSMAC') {
		(#include('main_legacy'))(glsmac);
		return;
	}

	#include('ui/styles/system')(glsmac.ui);
	const ui_utils = #include('ui/utils/utils');

	/*const f_quit_on_escape = (e) => {
		if (e.code == 'ESCAPE') {
			glsmac.exit();
			return true;
		}
		return false;
	};
	escape_handler_id = glsmac.ui.root.on('keydown', f_quit_on_escape);*/

	glsmac.on('init', () => {
		#print('STARTED');
	});

	//glsmac.ui.root.off('keydown', escape_handler_id);

	//const intro = #include('intro');

	//intro.run(glsmac);

	const tryinit = () => {
		try {
			glsmac.init();
		} catch {
			GSELoaderError: (e) => {
				let prompt = null;
				let error = null;
				prompt = ui_utils.system_popup.prompt(glsmac.ui, {
					text: 'Unable to find SMAC installation. Enter SMAC path:',
					oktext: 'OK',
					canceltext: 'Cancel',
					on_ok: (value) => {
						try {
							glsmac.init();
						} catch {
							GSELoaderError: (e) => {
								error = ui_utils.system_popup.error(glsmac.ui, {
									texts: [
										'Invalid SMAC path: ' + value,
										'  (check that it contains terranx.exe)'
									],
									on_close: () => {
										#print('REMOVE', error);
										error.remove();
										error = null;
									},
								});
							}
						}
					},
					on_cancel: () => {
						glsmac.exit();
					},
				});
			}
		}
	};
	tryinit();

});
