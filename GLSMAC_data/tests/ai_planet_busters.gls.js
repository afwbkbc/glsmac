const planet_busters = #include('../default/game/ai/planet_busters');

const make_player = (id) => {
	let player = {
		id: id,
		relations: {},
	};
	player.get_diplomatic_relation = (other) => {
		const key = 'p' + #to_string(other.id);
		return player.relations[key];
	};
	return player;
};

const actor = make_player(1);
const enemy = make_player(2);
const neutral = make_player(3);
actor.relations.p2 = 'vendetta';
actor.relations.p3 = 'neutral';

const make_tile = (x, y) => {
	let tile = {
		x: x,
		y: y,
		base: null,
		units: [],
		neighbours: [],
	};
	tile.get_base = () => { return tile.base; };
	tile.get_units = (include_embarked) => { return tile.units; };
	tile.get_surrounding_tiles = () => { return tile.neighbours; };
	return tile;
};

const small = make_tile(2, 2);
const large = make_tile(4, 4);
const neutral_tile = make_tile(6, 6);
const friendly_ring = make_tile(8, 8);
small.neighbours = [];
large.neighbours = [friendly_ring];
neutral_tile.neighbours = [];
friendly_ring.neighbours = [];
small.base = {get_owner: () => { return enemy; }, get_size: () => { return 3; }};
large.base = {get_owner: () => { return enemy; }, get_size: () => { return 8; }};
neutral_tile.base = {get_owner: () => { return neutral; }, get_size: () => { return 12; }};

const definition = {
	weapon: 'PlanetBuster',
	is_missile: true,
	reactor_power: 1,
};
const missile = {get_def: () => { return definition; }};
test.assert(
	planet_busters.choose_target(missile, actor, [small, neutral_tile], (id) => { return false; }) ==
	null
);
test.assert(
	planet_busters.choose_target(missile, actor, [small, large, neutral_tile], (id) => { return false; }) ==
	large
);
test.assert(
	planet_busters.choose_target(
		missile, actor, [large], (id) => { return false; }, 9
	) == null
);
test.assert(
	planet_busters.choose_target(
		missile, actor, [large], (id) => { return false; }, 8
	) == large
);

friendly_ring.units = [{owner: actor.id}];
test.assert(planet_busters.has_friendly_collateral(large, 1, actor.id));
test.assert(
	planet_busters.choose_target(missile, actor, [large], (id) => { return false; }) == null
);
friendly_ring.units = [];
friendly_ring.base = {get_owner: () => { return actor; }};
test.assert(planet_busters.has_friendly_collateral(large, 1, actor.id));
friendly_ring.base = null;

friendly_ring.units = [{owner: neutral.id}];
test.assert(
	planet_busters.choose_target(
		missile,
		actor,
		[large],
		(id) => { return id == neutral.id; }
	) == null
);
friendly_ring.units = [];

test.assert(
	planet_busters.choose_target(missile, actor, [large], (id) => { return id == enemy.id; }) == null
);
definition.weapon = 'ConventionalPayload';
test.assert(
	planet_busters.choose_target(missile, actor, [large], (id) => { return false; }) == null
);
