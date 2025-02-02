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

	let smacpath_prompt = null;

	glsmac.on('init', (e) => {

		if (smacpath_prompt != null) {
			smacpath_prompt.remove();
			smacpath_prompt = null;
		}

		escape_handler_id = glsmac.ui.root.on('keydown', (e) => {
			if (e.code == 'ESCAPE') {
				glsmac.exit();
				return true;
			}
			return false;
		});

		#print('STARTED');

		//glsmac.ui.root.off('keydown', escape_handler_id);
		//const intro = #include('intro');
		//intro.run(glsmac);

	});

	try {
		glsmac.init();
	} catch {
		GSELoaderError: (e) => {
			let error = null;
			smacpath_prompt = ui_utils.system_popup.prompt(glsmac.ui, {
				text: 'Unable to find SMAC installation. Enter SMAC path:',
				oktext: 'OK',
				canceltext: 'Cancel',
				on_ok: (value) => {
					try {
						glsmac.init(value);
					} catch {
						GSELoaderError: (e) => {
							error = ui_utils.system_popup.error(glsmac.ui, {
								texts: [
									'Invalid SMAC path: ' + value,
									'  (check that it contains terranx.exe)'
								],
								on_close: () => {
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

});
