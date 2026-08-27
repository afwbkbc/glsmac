const colonization = #include('../default/game/ai/colonization');

const owner = {id: 1};

const make_tile = (x, y, nutrients, minerals, energy) => {
	let base = null;
	let units = [];
	let surrounding = [];
	return {
		x: x,
		y: y,
		is_water: false,
		is_locked: () => { return false; },
		get_base: () => { return base; },
		set_base: (value) => { base = value; },
		get_units: () => { return units; },
		set_units: (value) => { units = value; },
		get_resources: (player) => {
			test.assert(player == owner);
			return {NUTRIENTS: nutrients, MINERALS: minerals, ENERGY: energy};
		},
		get_surrounding_tiles: () => { return surrounding; },
		set_surrounding_tiles: (value) => { surrounding = value; },
	};
};

const make_base = (tile) => {
	const base = {get_tile: () => { return tile; }};
	tile.set_base(base);
	return base;
};

const tm = {
	get_distance: (first, second) => {
		return #abs(first.x - second.x) + #abs(first.y - second.y);
	},
};

const home_tile = make_tile(0, 0, 2, 1, 1);
const home = make_base(home_tile);
const too_close = make_tile(2, 0, 3, 3, 3);
const poor = make_tile(3, 0, 1, 0, 0);
const rich = make_tile(3, 1, 2, 2, 2);
const ideal = make_tile(4, 0, 1, 0, 0);
const rich_neighbour = make_tile(4, 1, 3, 2, 1);
const ocean = make_tile(4, 2, 2, 1, 2);
ocean.is_water = true;
poor.set_surrounding_tiles([]);
rich.set_surrounding_tiles([rich_neighbour]);
ideal.set_surrounding_tiles([]);
ocean.set_surrounding_tiles([]);

test.assert(!colonization.is_valid_site(tm, too_close, owner.id, [home]));
test.assert(colonization.is_valid_site(tm, poor, owner.id, [home]));

too_close.set_units([{owner: 2}]);
test.assert(!colonization.is_valid_site(tm, too_close, owner.id, []));
too_close.set_units([]);
too_close.is_water = true;
test.assert(!colonization.is_valid_site(tm, too_close, owner.id, []));
test.assert(colonization.is_valid_site(tm, too_close, owner.id, [], true));
test.assert(!colonization.is_valid_site(tm, poor, owner.id, [home], true));
test.assert(colonization.is_valid_site(tm, ocean, owner.id, [home], true));
test.assert(colonization.get_site_score(tm, ocean, owner, [home], true) != null);

test.assert(colonization.get_site_score(tm, rich, owner, [home]) > colonization.get_site_score(tm, poor, owner, [home]));
test.assert(colonization.get_site_score(tm, ideal, owner, [home]) > colonization.get_site_score(tm, poor, owner, [home]));
test.assert(colonization.get_destination_score(tm, rich, owner, [home], 1) > colonization.get_destination_score(tm, rich, owner, [home], 2));
