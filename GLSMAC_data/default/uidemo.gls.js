#main((glsmac) => {

	const root = glsmac.ui.root;

	let imgs = [];
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
		const x = a[0];
		imgs []= root.img({
			align: a[0],
			background: a[1],
			width: 50,
			height: 50,
		});
	}
	#print('images', imgs);
	imgs[4].background = '#ffffff'; // make center visible

});
