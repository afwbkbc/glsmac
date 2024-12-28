#main((glsmac) => {

	const root = glsmac.ui.root;
	/*
		let surfaces = [];
		for (a of [
			['top left', '#60a06080'],
			['top center', '#00ffff'],
			['top right', '#ff00ff80'],
			['center left', '#ffff00'],
			['center', '#00000000'], // invisible at first
			['center right', '#00ff00'],
			['bottom left', '#ff000080'],
			['bottom center', '#0000ff'],
			['bottom right', '#a060a080'],
		]) {
			surfaces []= root.surface({
				align: a[0],
				background: a[1],
				width: 20,
				height: 20,
			});
		}
		surfaces[4].background = '#ffffff'; // make center visible
	*/
	// recursive panels
	const parent = root.panel({
		//align: 'top right',
		align: 'top left',
		/*		right: 40,
				top: 40,
				width: 200,
				height: 200,*/
		left: 100,
		top: 100,
		width: 300,
		height: 300,
		background: '#004400',
	});
	/*const child1 = parent.panel({
		left: 0,
		top: 0,
		width: 100,
		height: 100,
		background: '#006600',
	});
	const child1child1 = child1.panel({
		align: 'bottom right',
		right: 0,
		bottom: 0,
		width: 40,
		height: 40,
		background: '#008800',
	});
	const child1child2 = child1.panel({
		align: 'top right',
		right: 0,
		bottom: 0,
		width: 40,
		height: 40,
		background: '#008800',
	});
	const child1child3 = child1.panel({
		align: 'top left',
		right: 0,
		bottom: 0,
		width: 40,
		height: 40,
		background: '#008800',
	});
	const child1child4 = child1.panel({
		align: 'bottom left',
		right: 0,
		bottom: 0,
		width: 40,
		height: 40,
		background: '#008800',
	});
	const child2 = parent.panel({
		align: 'bottom right',
		right: 10,
		bottom: 10,
		width: 60,
		height: 60,
		background: '#00aa00',
	});*/
	/*const child3 = parent.panel({
		align: 'top right',
		right: -10,
		top: -10,
		width: 60,
		height: 60,
		background: '#00aa00',
	});*/
	/*	child1child2.background = '#00cc00';
		child1child4.background = '#00ee00';

		const texts = root.panel({
			left: 40,
			top: 40,
			right: 270,
			height: 120,
			background: '#000044',
		});
		texts.text({
			align: 'top left',
			text: 'TOP LEFT',
			color: '#ffff00',
			font: ':16',
		});
		texts.text({
			align: 'top right',
			text: 'TOP RIGHT',
			color: '#ffff00',
			font: ':16',
		});
		texts.text({
			align: 'bottom left',
			text: 'BOTTOM LEFT',
			color: '#ffff00',
			font: ':16',
		});
		texts.text({
			align: 'bottom right',
			text: 'BOTTOM RIGHT',
			color: '#ffff00',
			font: ':16',
		});
		texts.text({
			align: 'top center',
			text: 'TOP',
			top: 10,
			color: '#ffffff',
			font: ':24',
		});
		texts.text({
			align: 'bottom center',
			text: 'BOTTOM',
			bottom: 10,
			color: '#ffffff',
			font: ':24',
		});
		texts.text({
			align: 'left center',
			text: 'LEFT',
			left: 10,
			color: '#ffffff',
			font: ':32',
		});
		texts.text({
			align: 'right center',
			text: 'RIGHT',
			right: 10,
			color: '#ffffff',
			font: ':32',
		});
		texts.bottom = 50;
	*/
});
