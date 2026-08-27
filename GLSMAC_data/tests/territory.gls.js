const define_territory = #include('../default/game/territory');

const callbacks = {};
const values = {};
let bases = [];
const distance = (a, b) => { return a.x < b.x ? b.x - a.x : a.x - b.x; };
const game = {
	get_bm: () => { return {get_bases: () => { return bases; }}; },
	get_tm: () => { return {get_distance: distance}; },
	on: (name, callback) => { callbacks[name] = callback; },
	set: (key, value) => { values[key] = value; },
};

const make_tile = (x, is_water) => { return {x: x, y: 0, is_water: is_water}; };
const player_one = {id: 1, name: 'One'};
const player_two = {id: 2, name: 'Two'};
const make_base = (id, owner, x, is_water) => {
	const tile = make_tile(x, is_water);
	return {
		id: id,
		get_owner: () => { return owner; },
		get_tile: () => { return tile; },
	};
};

define_territory(game);
callbacks.start({});

const oldest = make_base(1, player_one, 0, false);
const newer = make_base(2, player_two, 4, false);
bases = [newer, oldest];
test.assert(values.f_territory_get_base(make_tile(2, false)) == oldest);
test.assert(values.f_territory_get_owner(make_tile(3, false)) == player_two);
test.assert(values.f_territory_is_friendly(player_two, make_tile(3, false)));
test.assert(values.f_territory_get_owner(make_tile(13, false)) == null);
test.assert(values.f_territory_get_max_distance() == 8);

bases = [oldest];
test.assert(values.f_territory_get_owner(make_tile(2, true)) == player_one);
test.assert(values.f_territory_get_owner(make_tile(3, true)) == null);

const sea_base = make_base(3, player_two, 5, true);
bases = [sea_base];
test.assert(values.f_territory_get_owner(make_tile(8, true)) == player_two);
test.assert(values.f_territory_get_owner(make_tile(6, false)) == null);
