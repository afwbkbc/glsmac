return {

	process_message: (text) => {
		this.page.text({
			class: 'bottombar-messages-line',
			text: text,
		});
	},

	init: (p) => {

		p.ui.class('bottombar-messages-line').set({
			color: 'rgb(184, 192, 96)',
			font: 'arialnb.ttf:16',
		});

		this.page = p.frame.listview({
			class: 'bottombar-frame-page',
			itemsize: 17,
			max_items: 50,
			padding: 3,
			vscroll_class: 'default-scroll-v',
		});

		this.button = p.frame.button({
			class: 'bottombar-frame-page-button',
			align: 'top right',
			top: 5,
			right: 5,
			text: 'M',
			group: 'pages',
		});

	},

};
