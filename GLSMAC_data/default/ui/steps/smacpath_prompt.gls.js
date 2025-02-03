return (m) => {

	m.glsmac.on('smacpath_prompt', (e) => {

		let promptvalue = '';
		if (#is_defined(e.last_failed_path)) {
			promptvalue = e.last_failed_path;
		}

		let prompt = null;
		prompt = m.utils.system_popup.prompt(m.glsmac.ui, {
			text: 'Unable to find SMAC installation. Enter SMAC path:',
			oktext: 'OK',
			canceltext: 'Cancel',
			value: promptvalue,
			on_ok: (value) => {
				e.set_smacpath(value);
				prompt.remove();
			},
			on_cancel: () => {
				m.glsmac.exit();
			},
		});

		if (#is_defined(e.last_failed_path)) {
			let error = null;
			error = m.utils.system_popup.error(m.glsmac.ui, {
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

};
