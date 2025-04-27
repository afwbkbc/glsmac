const defs = #include('units/defs');
const turns = #include('units/turns');
const animations = #include('units/animations');

const result = {

	configure: (game) => {
		turns.configure(game);
	},

	define: (game) => {
		defs.define(game);
		animations.define(game);
	},

};

return result;
