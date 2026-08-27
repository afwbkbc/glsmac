const resources = #include('../default/resources');
const define_bases = #include('../default/game/bases');

let resource_callback = null;
const tm = {
	on: (name, callback) => {
		test.assert(name == 'get_tile_resources');
		resource_callback = callback;
	},
};
resources.configure({get_tm: () => { return tm; }});

const make_tile = (fungus) => {
	return {
		is_land: true,
		is_water: false,
		moisture: 2,
		rockiness: 1,
		elevation: 0,
		features: {
			xenofungus: fungus,
			monolith: false,
			jungle: false,
			river: false,
		},
		terraforming: {
			road: false,
			forest: false,
			farm: false,
			soil_enricher: false,
			mine: false,
			solar: false,
			condenser: false,
			mirror: false,
			borehole: false,
		},
		bonuses: {
			nutrient: false,
			minerals: false,
			energy: false,
		},
		get_surrounding_tiles: () => { return []; },
		get_base: () => { return null; },
	};
};

let known_technologies = {};
let faction_id = 'HIVE';
const player = {
	has_technology: (id) => {
		return #is_defined(known_technologies[id]);
	},
	get_faction: () => { return {id: faction_id}; },
};

let yields = resource_callback({tile: make_tile(true), player: player});
test.assert(yields == {NUTRIENTS: 0, MINERALS: 0, ENERGY: 0});
known_technologies.CentauriEcology = true;
yields = resource_callback({tile: make_tile(true), player: player});
test.assert(yields == {NUTRIENTS: 1, MINERALS: 0, ENERGY: 0});
faction_id = 'GAIANS';
yields = resource_callback({tile: make_tile(true), player: player});
test.assert(yields == {NUTRIENTS: 2, MINERALS: 0, ENERGY: 0});
faction_id = 'HIVE';
for (technology_id of [
	'CentauriPsi',
	'CentauriGenetics',
	'MatterTransmission',
	'ThresholdOfTranscendence',
	'CentauriMeditation',
	'SecretsOfAlphaCentauri',
	'TemporalMechanics',
	'GeneSplicing',
	'EcologicalEngineering',
	'EnvironmentalEconomics',
]) {
	known_technologies[technology_id] = true;
}
yields = resource_callback({tile: make_tile(true), player: player});
test.assert(yields == {NUTRIENTS: 2, MINERALS: 3, ENERGY: 3});

const forest_tile = make_tile(false);
forest_tile.terraforming.forest = true;
yields = resource_callback({tile: forest_tile, player: player});
test.assert(yields == {NUTRIENTS: 1, MINERALS: 2, ENERGY: 1});
forest_tile.features.river = true;
yields = resource_callback({tile: forest_tile, player: player});
test.assert(yields == {NUTRIENTS: 1, MINERALS: 2, ENERGY: 2});

known_technologies = {};
const rainy_farm = make_tile(false);
rainy_farm.moisture = 3;
rainy_farm.terraforming.farm = true;
test.assert(resources.get_tile_yields(rainy_farm, player).NUTRIENTS == 2);
known_technologies.GeneSplicing = true;
test.assert(resources.get_tile_yields(rainy_farm, player).NUTRIENTS == 3);
known_technologies = {};
rainy_farm.bonuses.nutrient = true;
test.assert(resources.get_tile_yields(rainy_farm, player).NUTRIENTS == 5);

const rocky_mine = make_tile(false);
rocky_mine.rockiness = 3;
rocky_mine.terraforming.mine = true;
rocky_mine.terraforming.road = true;
test.assert(resources.get_tile_yields(rocky_mine, player).MINERALS == 2);
known_technologies.EcologicalEngineering = true;
test.assert(resources.get_tile_yields(rocky_mine, player).MINERALS == 4);
rocky_mine.bonuses.minerals = true;
known_technologies = {};
test.assert(resources.get_tile_yields(rocky_mine, player).MINERALS == 7);

const high_solar = make_tile(false);
high_solar.elevation = 2000;
high_solar.terraforming.solar = true;
test.assert(resources.get_tile_yields(high_solar, player).ENERGY == 2);
known_technologies.EnvironmentalEconomics = true;
test.assert(resources.get_tile_yields(high_solar, player).ENERGY == 3);

const mirror = make_tile(false);
mirror.terraforming.mirror = true;
const mirrored_solar = make_tile(false);
mirrored_solar.terraforming.solar = true;
mirrored_solar.get_surrounding_tiles = () => { return [mirror, mirror]; };
test.assert(resources.get_tile_yields(mirrored_solar, player).ENERGY == 3);

const monolith = make_tile(false);
monolith.features.monolith = true;
test.assert(resources.get_tile_yields(monolith, player) == {
	NUTRIENTS: 2,
	MINERALS: 2,
	ENERGY: 2,
});

const borehole = make_tile(false);
borehole.terraforming.borehole = true;
known_technologies = {EcologicalEngineering: true};
test.assert(resources.get_tile_yields(borehole, player) == {
	NUTRIENTS: 0,
	MINERALS: 6,
	ENERGY: 2,
});
known_technologies.EnvironmentalEconomics = true;
test.assert(resources.get_tile_yields(borehole, player) == {
	NUTRIENTS: 0,
	MINERALS: 6,
	ENERGY: 6,
});

const sea = make_tile(false);
sea.is_land = false;
sea.is_water = true;
known_technologies = {};
test.assert(resources.get_tile_yields(sea, player) == {
	NUTRIENTS: 1,
	MINERALS: 0,
	ENERGY: 1,
});
sea.terraforming.farm = true;
test.assert(resources.get_tile_yields(sea, player).NUTRIENTS == 2);
known_technologies.GeneSplicing = true;
test.assert(resources.get_tile_yields(sea, player).NUTRIENTS == 3);
sea.terraforming.farm = false;
sea.terraforming.solar = true;
test.assert(resources.get_tile_yields(sea, player).ENERGY == 2);
known_technologies.EnvironmentalEconomics = true;
test.assert(resources.get_tile_yields(sea, player).ENERGY == 3);

const bm_callbacks = {};
const bm = {
	on: (name, callback) => {
		bm_callbacks[name] = callback;
	},
};
const game_callbacks = {};
let effective_facilities = #undefined;
let global_mineral_bonus = 0;
let orbital_bonus = {NUTRIENTS: 0, MINERALS: 0, ENERGY: 0};
const game = {
	get_bm: () => { return bm; },
	event: (name, data) => {},
	on: (name, callback) => {
		game_callbacks[name] = callback;
	},
	get: (key) => {
		if (key == 'f_base_get_effective_facilities' && #is_defined(effective_facilities)) {
			return (base) => { return effective_facilities; };
		}
		if (key == 'f_project_get_effects') {
			return (base) => { return {
				mineral_bonus: global_mineral_bonus,
				support_bonus: 0,
				maintenance_multiplier: 1.0,
				growth_rating_bonus: 0,
				population_limit_bonus: 0,
				talent_bonus: 0,
				network_node_drone_modifier: 0,
				prevent_riots: false,
			}; };
		}
		if (key == 'f_orbital_get_base_resource_bonus') {
			return (base, resource) => { return orbital_bonus[resource]; };
		}
		return #undefined;
	},
};
define_bases(game);

let queried_players = [];
const base_owner = {id: 7};
const make_base_tile = () => {
	return {
		is_land: true,
		terraforming: {forest: false},
		get_resources: (value_player) => {
			queried_players :+value_player;
			return {NUTRIENTS: 1, MINERALS: 1, ENERGY: 1};
		},
	};
};
const center_tile = make_base_tile();
const worked_tile = make_base_tile();
const base = {
	get_owner: () => { return base_owner; },
	get_tile: () => { return center_tile; },
	get_worked_tiles: () => { return [worked_tile]; },
	get_facilities: () => { return []; },
};

const intake = bm_callbacks.get_base_intake({base: base});
test.assert(intake == {NUTRIENTS: 2, MINERALS: 2, ENERGY: 2});
test.assert(queried_players == [base_owner, base_owner]);

base.get_facilities = () => { return [
	{nutrient_bonus: 1, mineral_bonus: 1, energy_bonus: 1, mineral_multiplier: 0.5},
	{nutrient_bonus: 0, mineral_bonus: 0, energy_bonus: 0, mineral_multiplier: 0.5},
]; };
const multiplied_intake = bm_callbacks.get_base_intake({base: base});
test.assert(multiplied_intake == {NUTRIENTS: 3, MINERALS: 6, ENERGY: 3});

effective_facilities = base.get_facilities() + [{
	nutrient_bonus: 0,
	mineral_bonus: 0,
	energy_bonus: 0,
	mineral_multiplier: 0.0,
	worked_tile_energy_bonus: 1,
}];
global_mineral_bonus = 2;
const project_intake = bm_callbacks.get_base_intake({base: base});
test.assert(project_intake == {NUTRIENTS: 3, MINERALS: 10, ENERGY: 5});

base.get_facilities = () => { return []; };
effective_facilities = [];
global_mineral_bonus = 0;
orbital_bonus = {NUTRIENTS: 3, MINERALS: 2, ENERGY: 1};
const orbital_intake = bm_callbacks.get_base_intake({base: base});
test.assert(orbital_intake == {NUTRIENTS: 5, MINERALS: 4, ENERGY: 3});
orbital_bonus = {NUTRIENTS: 0, MINERALS: 0, ENERGY: 0};

worked_tile.terraforming.forest = true;
effective_facilities = [{
	nutrient_bonus: 0,
	mineral_bonus: 0,
	energy_bonus: 0,
	mineral_multiplier: 0.0,
	forest_nutrient_bonus: 2,
	forest_mineral_bonus: 1,
	forest_energy_bonus: 1,
}];
global_mineral_bonus = 0;
const forest_intake = bm_callbacks.get_base_intake({base: base});
test.assert(forest_intake == {NUTRIENTS: 4, MINERALS: 3, ENERGY: 3});
