#main((glsmac) => {

	const root = glsmac.ui.root;

	root.on('keydown', (e) => {
		if (e.code == 'ESCAPE') {
			glsmac.exit();
			return true;
		}
		return false;
	});

	glsmac.ui.class('side-square').set({
		width: 20,
		height: 20,
	});
	glsmac.ui.class('side-square-alt').set({
		width: 30,
		height: 30,
	});

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
		const surface = root.surface({
			class: 'side-square',
			align: a[0],
			background: a[1],
		});
		const orig_color = a[1];
		surface.on('mouseover', (e) => {
			surface.background = '#ffffff';
			surface.width = 30;
			surface.height = 30;
			return true;
		});
		surface.on('mouseout', (e) => {
			surface.background = orig_color;
			surface.width = #undefined;
			surface.height = #undefined;
			return true;
		});
		surfaces :+surface;
	}
	surfaces[4].background = '#ffffff'; // make center visible

	const tex = 'textures/demotex.png';

	// recursive panels
	const parent = root.panel({
		align: 'top right',
		right: 40,
		top: 40,
		width: 200,
		height: 200,
		background: tex + ':tint(#00ff00,0.08):stretch',
	});
	const child1 = parent.panel({
		left: 0,
		top: 0,
		width: 100,
		height: 100,
		background: tex + ':tint(#006600,0.5)',
	});
	const child1child1 = child1.panel({
		align: 'bottom right',
		right: 0,
		bottom: 0,
		width: 40,
		height: 40,
		background: tex + ':tint(#008800,0.6)',
	});
	const child1child2 = child1.panel({
		align: 'top right',
		right: 0,
		bottom: 0,
		width: 40,
		height: 40,
		background: tex + ':tint(#008800,0.6)',
	});
	const child1child3 = child1.panel({
		align: 'top left',
		right: 0,
		bottom: 0,
		width: 40,
		height: 40,
		background: tex + ':tint(#008800,0.6)',
	});
	const child1child4 = child1.panel({
		align: 'bottom left',
		right: 0,
		bottom: 0,
		width: 40,
		height: 40,
		background: tex + ':tint(#008800,0.6)',
	});
	const child2 = parent.panel({
		align: 'bottom right',
		right: 10,
		bottom: 10,
		width: 60,
		height: 60,
		background: tex + ':tint(#aa0000,0.2)',
	});
	const child3 = parent.panel({
		align: 'top right',
		right: -10,
		top: -10,
		width: 60,
		height: 60,
		background: tex + ':tint(#0000ff,0.2):crop(5,5,8,8)',
	});
	child1child2.background = '#00cc00';
	child1child4.background = '#00ee00';

	const textcls1 = glsmac.ui.class('text1');
	const textcls2 = glsmac.ui.class('text2');
	const textcls3 = glsmac.ui.class('text3');

	const texts = root.panel({
		left: 40,
		top: 40,
		right: 270,
		height: 120,
		background: 'textures/demotex.png',
	});
	texts.text({
		align: 'top left',
		text: 'TOP LEFT',
		class: 'text1',
	});
	texts.text({
		align: 'top right',
		text: 'TOP RIGHT',
		class: 'text1',
	});
	texts.text({
		align: 'bottom left',
		text: 'BOTTOM LEFT',
		class: 'text1',
	});
	texts.text({
		align: 'bottom right',
		text: 'BOTTOM RIGHT',
		class: 'text1',
	});
	texts.text({
		align: 'top center',
		text: 'TOP',
		top: 10,
		class: 'text2',
	});
	texts.text({
		align: 'bottom center',
		text: 'BOTTOM',
		bottom: 10,
		class: 'text2',
	});
	texts.text({
		align: 'left center',
		text: 'LEFT',
		left: 10,
		class: 'text3',
	});
	texts.text({
		align: 'right center',
		text: 'RIGHT',
		right: 10,
		class: 'text3',
	});
	texts.bottom = 50;

	// change color and font in text1 class, should update all related objects
	textcls1.set({
		font: ':16',
	});
	textcls2.set({
		color: '#ffffff',
		font: ':24',
	});
	textcls3.color = textcls2.color; // this should work too
	textcls3.set({
		font: ':8', // will be unset later
	});
	textcls3.unset(['font']); // unset so that default font size is used

	// blink corner texts
	let visible = false;
	#async(1000, () => {
		visible = !visible;
		if (visible) {
			textcls1.color = '#ffff00ff';
		} else {
			textcls1.color = '#00000000';
		}
		return true;
	});

	// 'bouncing ball' animation

	let ballarea = [180, 420];
	let ballareaobj = root.panel({
		align: 'bottom right',
		right: 40,
		bottom: 40,
		width: ballarea[0],
		height: ballarea[1],
		background: '#ffffff44',
	});

	glsmac.ui.class('balltext').set({
		align: 'center',
		text: 'ball',
		color: '#ffffff',
	});

	let balls = [];

	const f_move_balls = () => {
		for (ball of balls) {
			const ballobj = ball.obj;
			const maxleft = ballarea[0] - ballobj.width - 1;
			const maxtop = ballarea[1] - ballobj.height - 1;
			if (ballobj.left <= 0 || ballobj.left >= maxleft) {
				if (ballobj.left < 0) {
					ballobj.left = 0;
				}
				if (ballobj.left > maxleft) {
					ballobj.left = maxleft;
				}
				ball.speed[0] = ball.speed[0] * -1;
			}
			if (ballobj.top <= 0 || ballobj.top >= maxtop) {
				if (ballobj.top < 0) {
					ballobj.top = 0;
				}
				if (ballobj.top > maxtop) {
					ballobj.top = maxtop;
				}
				ball.speed[1] = ball.speed[1] * -1;
			}
			ballobj.left = ballobj.left + ball.speed[0];
			ballobj.top = ballobj.top + ball.speed[1];
		}
		return true;
	};
	let balls_timer = null;

	const addball = (ball) => {
		let ballobj = ballareaobj.panel({
			class: ball.class,
			left: ball.pos[0],
			top: ball.pos[1],
		});
		ballobj.text({
			class: ball.class + 'text',
		});

		ballobj.on('mouseover', (e) => {
			ballobj.background = '#ffffff';
			return true;
		});
		ballobj.on('mouseout', (e) => {
			ballobj.background = #undefined;
			return true;
		});
		ball.obj = ballobj;
		balls :+ball;
	};

	glsmac.ui.class('ball1', { // set in constructor is possible too
		width: 20,
		height: 20,
		background: 'blue',
	});
	glsmac.ui.class('ball1text', 'balltext', { // extend in constructor
		font: ':16',
	});
	glsmac.ui.class('ball2', {
		width: 30,
		height: 30,
		background: 'rgb(0, 255, 0)',
	});
	glsmac.ui.class('ball2text').extend('balltext').set({ // extend after constructor is possible too
		font: ':20',
	});
	glsmac.ui.class('ball3', {
		width: 40,
		height: 40,
		background: '#0000ff',
	});
	glsmac.ui.class('ball3text', {
		font: ':32',
	}).extend('balltext'); // extend can be done later too

	for (y of [0, 30, 40, 70, 80, 120, 150, 160, 170]) {
		addball({
			class: 'ball1',
			pos: [60 + y, 100 + y * 2],
			speed: [(-3) - (y / 40), (-2) - (y / 40)],
		});

		addball({
			class: 'ball2',
			pos: [60 + y, 100 + y * 3],
			speed: [(-1) - (y / 40), (-3) - (y / 40)],
		});

		addball({
			class: 'ball3',
			pos: [30 + y, 0 + y],
			speed: [(-2) - (y / 40), (-1) - (y / 40)],
		});
	}

	glsmac.ui.class('button1')
		.set({
			background: '#112211',
			border: '#666666,3',
			color: '#aaffaa',
			_active: {
				background: '#446644',
			},
		})
	;
	glsmac.ui.class('button2')
		.set({
			font: ':20',
			_hover: {
				color: 'white',
				border: 'white,3',
				background: '#224411',
			},
			_active: {
				color: '#00ff00',
			},
		})
	;
	glsmac.ui.class('button-active')
		.set({
			background: '#221111',
			color: 'red',
			_hover: {
				background: '#442211',
			},
			_active: {
				color: '#ff0000',
				background: '#664444',
			},
		})
	;
	let button = root.button({
		class: 'button1 button2',
		align: 'bottom right',
		width: 200,
		height: 24,
		bottom: 480,
		right: 40,
		text: 'START',
	});
	button.on('mouseover', (e) => {
		for (surface of surfaces) {
			surface.class = 'side-square-alt';
		}
		return true;
	});
	button.on('mouseout', (e) => {
		for (surface of surfaces) {
			surface.class = 'side-square';
		}
		return true;
	});
	button.on('click', (e) => {
		if (balls_timer != null) {
			button.removeclass('button-active');
			balls_timer.stop();
			balls_timer = null;
			button.text = 'START';
		} else {
			button.text = 'STOP';
			balls_timer = #async(10, f_move_balls);
			button.addclass('button-active');
		}
		return true;
	});

	// count async timer iterations
	let iterations = 0;
	const counterbox = texts.panel({
		align: 'center',
		width: 200,
		height: 60,
	});
	const counter = counterbox.text({
		align: 'center',
		color: '#ffffff',
		font: ':20',
	});
	#async(10, () => {
		counter.text = 'ITERATIONS: ' + #to_string(++iterations);
		return true;
	});
	// cycle colors a bit
	let is_blue = false;
	#async(200, () => {
		is_blue = !is_blue;
		if (is_blue) {
			counterbox.background = '#0000ff77';
		} else {
			counterbox.background = 'rgba(255, 0, 0, 127)';
		}
		return true;
	});

	glsmac.ui.class('ot').set({
		width: 100,
		height: 25,
	});
	const overflowtest = root.panel({
		class: 'ot',
		align: 'bottom left',
		bottom: 100,
		left: 100,
		height: 100,
		background: 'rgba(128, 255, 128, 128)',
		overflow: 'hidden',
	});
	overflowtest.surface({
		class: 'ot',
		align: 'top left',
		left: -50,
		background: 'red',
	});
	overflowtest.surface({
		class: 'ot',
		align: 'top right',
		right: -50,
		background: 'green',
		top: 25,
	});
	overflowtest.surface({
		class: 'ot',
		align: 'top left',
		left: -50,
		background: 'blue',
		top: 50,
	});
	overflowtest.surface({
		class: 'ot',
		align: 'top right',
		right: -50,
		background: 'gray',
		top: 75,
	});
	#async(1000, () => {
		if (overflowtest.overflow == 'hidden') {
			overflowtest.overflow = 'visible';
		} else {
			overflowtest.overflow = 'hidden';
		}
		return true;
	});

	const input = texts.panel({
		align: 'center',
		width: 300,
		height: 120,
	}).input({
		align: 'bottom',
		width: 300,
		height: 20,
		background: '#111116',
		color: 'white',
		font: ':16',
		border: 'silver,2',
	});

	input.on('select', (e) => {
		#print('SELECT', e);
		return true;
	});

	const crpanel = texts.panel({
		align: 'top center',
		top: 100,
		height: 140,
		width: 280,
	});
	const create = crpanel.button({
		class: 'button1 button2',
		align: 'top left',
		width: 130,
		height: 30,
		text: 'CREATE',
	});
	const remove = crpanel.button({
		class: 'button1 button2 button-active',
		align: 'top right',
		width: 130,
		height: 30,
		text: 'REMOVE',
	});

	let crobjects = [];
	glsmac.ui.class('crclass').set({
		background: 'rgba( 0, 255, 0, 40 );',
		width: 100,
		height: 100,
		align: 'bottom center',
	});

	create.on('click', (e) => {
		crobjects :+crpanel.surface({
			class: 'crclass',
		});
		return true;
	});
	remove.on('click', (e) => {
		const lastobj = crobjects:~;
		if ( #is_defined( lastobj ) ) {
			lastobj.remove();
		}
		return true;
	});

	let window = null;
	const windowbtn = texts.button({
		class: 'button1 button2',
		align: 'bottom right',
		bottom: 60,
		right: 100,
		height: 40,
		width: 160,
		text: 'Show Window',
	});
	windowbtn.on('click', (e) => {
		if (window == null) {
			window = root.window({
				width: 300,
				height: 200,
				background: '#333333',
				border: 'gray',
				title: 'Test Window',
				header_border: 'white',
				header_height: 30,
				header_color: 'white',
				header_font: ':24',
				header_background: 'navy',
				align: 'center',
			});
			window.text({
				align: 'center',
				text: 'WINDOW',
				class: 'balltext',
			});
			const closebtn = window.button({
				class: 'button1 button2 button-active',
				align: 'bottom right',
				height: 30,
				width: 100,
				right: 10,
				bottom: 10,
				text: 'CLOSE',
			});
			closebtn.on('click', (e) => {
					window.remove();
					window = null;
					return true;
				})
			;
			const blockbtn = window.button({
				class: 'button1 button2',
				align: 'bottom left',
				height: 30,
				width: 100,
				left: 10,
				bottom: 10,
				text: 'BLOCK',
			});
			blockbtn.on('click', (e) => {
				if (window.blocker == #undefined) {
					window.blocker = 'rgba(0, 0, 0, 120)';
					blockbtn.text = 'UNBLOCK';
				}
				else {
					window.blocker = #undefined;
					blockbtn.text = 'BLOCK';
				}
				return true;
			});
		}
		return true;
	});

});
