return (i) => {
	const next = () => {
		if (i.settings.global.map.type == 'random') {
			i.steps.select_difficulty_level(i);
		} else { // custom
			i.steps.customize_ocean_coverage(i);
		}
	};
	const custom = () => {
		let width_input = null;
		let height_input = null;
		i.popup.show({
			title: 'Custom Planet Size',
			width: 430,
			height: 130,
			generator: (body) => {
				body.text({
					class: 'popup-text',
					text: 'Width:',
					align: 'left',
					top: 13,
					left: 15,
				});
				width_input = body.input({
					class: 'popup-input',
					top: 12,
					left: 150,
					width: 260,
					value: #to_string(i.settings.global.map.size_x),
				});
				body.text({
					class: 'popup-text',
					text: 'Height:',
					align: 'left',
					top: 40,
					left: 15,
				});
				height_input = body.input({
					class: 'popup-input',
					top: 39,
					left: 150,
					width: 260,
					value: #to_string(i.settings.global.map.size_y),
				});
			},
			buttons: [
				{
					style: {
						text: 'OK',
						align: 'left',
						is_ok: true,
					},
					onclick: (e) => {
						let width = 0;
						let height = 0;
						let parse_error = false;
						try {
							width = #to_int(#trim(width_input.value));
							height = #to_int(#trim(height_input.value));
						} catch {
						:
							(e) => {
								parse_error = true;
							}
						}
						if (parse_error) {
							i.popup.error('Width and height must be whole numbers.');
							return true;
						}
						if (width < 4 || height < 4 || width % 2 != 0 || height % 2 != 0) {
							i.popup.error('Width and height must be even numbers of at least 4.');
							return true;
						}
						if (width * height > 180 * 90) {
							i.popup.error('Custom map area cannot exceed Huge Planet (180x90).');
							return true;
						}
						i.settings.global.map.size_x = width;
						i.settings.global.map.size_y = height;
						i.popup.hide();
						next();
						return true;
					},
				},
				{
					style: {
						text: 'Cancel',
						align: 'right',
						is_cancel: true,
					},
					onclick: (e) => {
						i.popup.back();
						return true;
					},
				},
			],
		});
	};
	i.sliding.show({
		title: 'Select size of planet',
		entries: [
			['Tiny Planet', () => {
				i.settings.global.map.size_x = 68;
				i.settings.global.map.size_y = 34;
				next();
			}],
			['Small Planet', () => {
				i.settings.global.map.size_x = 88;
				i.settings.global.map.size_y = 44;
				next();
			}],
			['Standard Planet', () => {
				i.settings.global.map.size_x = 112;
				i.settings.global.map.size_y = 56;
				next();
			}],
			['Large Planet', () => {
				i.settings.global.map.size_x = 140;
				i.settings.global.map.size_y = 70;
				next();
			}],
			['Huge Planet', () => {
				i.settings.global.map.size_x = 180;
				i.settings.global.map.size_y = 90;
				next();
			}],
			['Custom Size', custom],
		],
	});
};
