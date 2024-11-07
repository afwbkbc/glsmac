const defs = #include('units/defs');
const movement = #include('units/movement');
const combat = #include('units/combat');
const turns = #include('units/turns');
const animations = #include('units/animations');

const result = {

	configure: (um) => {
		movement.configure(um);
		combat.configure(um, animations);
		turns.configure(um);
	},

	define: () => {
		defs.define();
		animations.define();
	},

};

return result;
