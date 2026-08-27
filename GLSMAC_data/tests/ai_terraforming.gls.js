const terraforming = #include('../default/game/ai/terraforming');

let known_technologies = {};
const owner = {
	id: 1,
	has_technology: (id) => {
		return #is_defined(known_technologies[id]);
	},
};

const make_tile = (moisture, rockiness, nutrients, minerals, energy) => {
	return {
		is_water: false,
		moisture: moisture,
		rockiness: rockiness,
		features: {monolith: false, xenofungus: false},
		terraforming: {
			road: false,
			mag_tube: false,
			forest: false,
			farm: false,
			soil_enricher: false,
			mine: false,
			solar: false,
			condenser: false,
			mirror: false,
			borehole: false,
			sensor: false,
			bunker: false,
			airbase: false,
			remove_fungus: false,
			plant_fungus: false,
		},
		get_base: () => { return null; },
		get_surrounding_tiles: () => { return []; },
		get_resources: (player) => {
			test.assert(player == owner);
			return {NUTRIENTS: nutrients, MINERALS: minerals, ENERGY: energy};
		},
	};
};

const moist = make_tile(2, 1, 2, 1, 1);
test.assert(terraforming.get_order(moist, true, owner) == 'farm');
test.assert(terraforming.get_order(moist, false, owner) == 'farm');

const arid = make_tile(0, 1, 1, 1, 1);
test.assert(terraforming.get_order(arid, true, owner) == 'forest');

moist.terraforming.farm = true;
test.assert(terraforming.get_order(moist, true, owner) == 'road');
moist.terraforming.road = true;
test.assert(terraforming.get_order(moist, true, owner) == 'solar');
moist.terraforming.solar = true;
test.assert(terraforming.get_order(moist, true, owner) == null);

const fungus = make_tile(2, 1, 3, 3, 3);
fungus.features.xenofungus = true;
test.assert(terraforming.get_order(fungus, true, owner) == 'remove_fungus');

const rocky = make_tile(1, 3, 0, 2, 0);
test.assert(terraforming.get_order(rocky, true, owner) == 'mine');
rocky.terraforming.mine = true;
test.assert(terraforming.get_order(rocky, true, owner) == 'road');

const advanced = make_tile(2, 1, 2, 1, 1);
advanced.terraforming.farm = true;
advanced.terraforming.road = true;
known_technologies.AdvancedEcologicalEngineering = true;
test.assert(terraforming.get_order(advanced, false, owner) == 'soil_enricher');
known_technologies.EcologicalEngineering = true;
test.assert(terraforming.get_order(advanced, true, owner) == 'condenser');

const connected = make_tile(1, 1, 1, 1, 1);
connected.terraforming.forest = true;
connected.terraforming.road = true;
known_technologies = {MonopoleMagnets: true};
test.assert(terraforming.get_order(connected, false, owner) == 'mag_tube');
known_technologies = {};

const food = make_tile(2, 1, 3, 0, 0);
const minerals = make_tile(1, 2, 0, 3, 0);
test.assert(terraforming.get_target_score(food, owner, 0, 2) > terraforming.get_target_score(minerals, owner, 1, 1));
test.assert(terraforming.get_target_score(food, owner, 1, 1) > terraforming.get_target_score(food, owner, 1, 2));
test.assert(terraforming.get_target_score(food, owner, 1, 2, true) > terraforming.get_target_score(food, owner, 1, 1, false));
