const defs = #include('units/defs');
const movement = #include('units/movement');
const combat = #include('units/combat');
const turns = #include('units/turns');
const animations = #include('units/animations');

const result = {

	configure: (game) => {
		movement.configure(game);
		combat.configure(game, animations);
		turns.configure(game);
	},

	define: (game) => {
		defs.define(game.um);
		animations.define(game.am);
	},

};

return result;
