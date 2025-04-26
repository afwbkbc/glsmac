const defs = #include('units/defs');
const combat = #include('units/combat');
const turns = #include('units/turns');
const animations = #include('units/animations');

const result = {

	configure: (game) => {
		combat.configure(game, animations);
		turns.configure(game);
	},

	define: (game) => {
		defs.define(game);
		animations.define(game);
	},

};

return result;
