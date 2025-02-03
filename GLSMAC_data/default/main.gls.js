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

	glsmac.on('smacpath_prompt', (e) => {

		let promptvalue = '';
		if (#is_defined(e.last_failed_path)) {
			promptvalue = e.last_failed_path;
		}

		let prompt = null;
		prompt = ui_utils.system_popup.prompt(glsmac.ui, {
			text: 'Unable to find SMAC installation. Enter SMAC path:',
			oktext: 'OK',
			canceltext: 'Cancel',
			value: promptvalue,
			on_ok: (value) => {
				e.set_smacpath(value);
				prompt.remove();
			},
			on_cancel: () => {
				glsmac.exit();
			},
		});

		if (#is_defined(e.last_failed_path)) {
			let error = null;
			error = ui_utils.system_popup.error(glsmac.ui, {
				texts: [
					'Invalid SMAC path: ' + e.last_failed_path,
					'  (check that it contains terranx.exe)'
				],
				on_close: () => {
					error.remove();
					error = null;
				},
			});
		}

	});

	glsmac.on('init', (e) => {

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

	glsmac.start();

});
