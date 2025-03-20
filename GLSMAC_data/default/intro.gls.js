return {

	run: (glsmac) => {

		let logo = glsmac.ui.root.img({
			id: 'firaxis-logo',
			src: 'logo.pcx',
			left: 100,
			top: 20,
			right: 100,
			bottom: 20,
		});

		logo.height = 500;

	},

};
