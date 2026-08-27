const unity_pods = #include('../default/game/ai/unity_pods');

const make_tile = (x, pod) => {
	let neighbours = [];
	return {
		x: x,
		y: 0,
		features: {unity_pod: pod},
		get_surrounding_tiles: () => { return neighbours; },
		set_neighbours: (values) => { neighbours = values; },
	};
};

const source = make_tile(0, false);
const near = make_tile(2, true);
const detour = make_tile(4, false);
const far = make_tile(6, true);
source.set_neighbours([near, detour]);
near.set_neighbours([source]);
detour.set_neighbours([source, far]);
far.set_neighbours([detour]);
const tiles = [source, near, detour, far];
const tm = {
	get_tile: (x, y) => {
		for (tile of tiles) {
			if (tile.x == x) { return tile; }
		}
		return null;
	},
};
const game = {get_tm: () => { return tm; }};
const definition = {weapon: 'HandWeapons'};
let unit = {
	is_air: false,
	is_immovable: false,
	get_tile: () => { return source; },
	get_def: () => { return definition; },
};
const can_enter = (from, tile) => { return true; };
let destination = unity_pods.choose_destination(game, unit, can_enter);
test.assert(destination.target == near && destination.step == near && destination.distance == 1);

near.features.unity_pod = false;
destination = unity_pods.choose_destination(game, unit, can_enter);
test.assert(destination.target == far && destination.step == detour && destination.distance == 2);

unit.is_air = true;
test.assert(unity_pods.choose_destination(game, unit, can_enter) == null);
unit.is_air = false;
definition.weapon = 'AlienArtifact';
test.assert(unity_pods.choose_destination(game, unit, can_enter) == null);
