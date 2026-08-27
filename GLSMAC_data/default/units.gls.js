const defs = #include('units/defs');
const turns = #include('units/turns');
const animations = #include('units/animations');
const manifest = #include('content/base_units');

const result = {
	moralesets: defs.moralesets,
	definitions: defs.definitions,
	generated_count: defs.generated_count,
	manifest: manifest,

	configure: (game) => {
		turns.configure(game);
	},

	define: (game) => {
		defs.define(game);
		animations.define(game);
	},

};

return result;
