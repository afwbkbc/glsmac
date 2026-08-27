const unity_pods = #include('../default/game/unity_pods');

const make_flags = () => {
	return {
		road: false,
		mag_tube: false,
		forest: false,
		farm: false,
		soil_enricher: false,
		solar: false,
		mine: false,
		condenser: false,
		mirror: false,
		borehole: false,
		sensor: false,
		bunker: false,
		airbase: false,
		remove_fungus: false,
		plant_fungus: false,
	};
};

const run_case = (kind, verify_resolve) => {
	let feature_state = {
		river: false,
		monolith: false,
		xenofungus: kind == 'terraforming',
		unity_pod: true,
	};
	let bonus_state = {nutrient: false, energy: false, minerals: false};
	let terraforming_state = make_flags();
	terraforming_state.road = kind == 'earthquake';
	terraforming_state.mag_tube = kind == 'earthquake';
	terraforming_state.forest = kind == 'fungus';
	let elevation = 1000;

	const center = {
		x: 10,
		y: 10,
		is_water: false,
		is_land: true,
		elevation: 1000,
		rockiness: 1,
		features: {
			river: false,
			monolith: false,
			xenofungus: kind == 'terraforming',
			unity_pod: true,
		},
		bonuses: {nutrient: false, energy: false, minerals: false},
		terraforming: {
			road: kind == 'earthquake',
			mag_tube: kind == 'earthquake',
			forest: kind == 'fungus',
			farm: false,
			soil_enricher: false,
			solar: false,
			mine: false,
			condenser: false,
			mirror: false,
			borehole: false,
			sensor: false,
			bunker: false,
			airbase: false,
			remove_fungus: false,
			plant_fungus: false,
		},
		get_surrounding_tiles: () => { return []; },
		get_base: () => { return null; },
		get_units: (include_embarked) => { return []; },
		update_features: (changes) => {
			if (#is_defined(changes.river)) { feature_state.river = changes.river; }
			if (#is_defined(changes.monolith)) { feature_state.monolith = changes.monolith; }
			if (#is_defined(changes.xenofungus)) { feature_state.xenofungus = changes.xenofungus; }
			if (#is_defined(changes.unity_pod)) { feature_state.unity_pod = changes.unity_pod; }
		},
		update_terraforming: (changes) => {
			if (#is_defined(changes.road)) { terraforming_state.road = changes.road; }
			if (#is_defined(changes.mag_tube)) { terraforming_state.mag_tube = changes.mag_tube; }
			if (#is_defined(changes.forest)) { terraforming_state.forest = changes.forest; }
			if (#is_defined(changes.farm)) { terraforming_state.farm = changes.farm; }
			if (#is_defined(changes.soil_enricher)) { terraforming_state.soil_enricher = changes.soil_enricher; }
			if (#is_defined(changes.solar)) { terraforming_state.solar = changes.solar; }
			if (#is_defined(changes.mine)) { terraforming_state.mine = changes.mine; }
			if (#is_defined(changes.condenser)) { terraforming_state.condenser = changes.condenser; }
			if (#is_defined(changes.mirror)) { terraforming_state.mirror = changes.mirror; }
			if (#is_defined(changes.borehole)) { terraforming_state.borehole = changes.borehole; }
			if (#is_defined(changes.sensor)) { terraforming_state.sensor = changes.sensor; }
			if (#is_defined(changes.bunker)) { terraforming_state.bunker = changes.bunker; }
			if (#is_defined(changes.airbase)) { terraforming_state.airbase = changes.airbase; }
			if (#is_defined(changes.remove_fungus)) { terraforming_state.remove_fungus = changes.remove_fungus; }
			if (#is_defined(changes.plant_fungus)) { terraforming_state.plant_fungus = changes.plant_fungus; }
		},
		set_bonus: (name) => {
			bonus_state.nutrient = name == 'nutrient';
			bonus_state.energy = name == 'energy';
			bonus_state.minerals = name == 'minerals';
		},
	};

	let energy_credits = 100;
	let research = {technologies: [], target: 'Alpha', progress: 4};
	const clone_research = () => {
		let technologies = [];
		for (id of research.technologies) {
			technologies :+id;
		}
		return {technologies: technologies, target: research.target, progress: research.progress};
	};
	const player = {
		id: 1,
		name: 'Gaia',
		get_energy_credits: () => { return energy_credits; },
		set_energy_credits: (value) => { energy_credits = value; },
		has_technology: (id) => { return id == 'SyntheticFossilFuels'; },
		get_research_state: () => { return clone_research(); },
		set_research_state: (value) => {
			let technologies = [];
			for (id of value.technologies) {
				technologies :+id;
			}
			research = {technologies: technologies, target: value.target, progress: value.progress};
		},
	};

	let base_minerals = 10;
	const production = {name: 'Recycling Tanks', mineral_cost: 100};
	const base_tile = {x: 30, y: 30};
	const production_base = {
		id: 1,
		name: 'Test Base',
		get_owner: () => { return player; },
		get_tile: () => { return base_tile; },
		get_production: () => { return production; },
		get_accumulated_minerals: () => { return base_minerals; },
		set_accumulated_minerals: (value) => { base_minerals = value; },
	};

	const definitions = [
		{id: 'ScoutPatrol', name: 'Scout Patrol', offense: 1, weapon: 'HandWeapons', cargo_capacity: 0},
		{id: 'AlienArtifact', name: 'Alien Artifact', offense: 0, weapon: 'AlienArtifact', cargo_capacity: 0},
		{id: 'UnityRover', name: 'Unity Rover', offense: 1, weapon: 'HandWeapons', cargo_capacity: 0},
		{id: 'UnityScoutChopper', name: 'Unity Scout Chopper', offense: 1, weapon: 'HandWeapons', cargo_capacity: 0},
		{id: 'UnityFoil', name: 'Unity Foil', offense: 0, weapon: 'TroopTransport', cargo_capacity: 2},
		{id: 'MindWorms', name: 'Mind Worms', offense: 1, weapon: 'PsiAttack', cargo_capacity: 0},
		{id: 'IsleOfTheDeep', name: 'Isle of the Deep', offense: 1, weapon: 'PsiAttack', cargo_capacity: 4},
	];
	const find_definition = (id) => {
		for (definition of definitions) {
			if (definition.id == id) { return definition; }
		}
		return null;
	};
	let unit_health = 0.5;
	let unit_morale = 2;
	const unit = {
		id: 1,
		owner: player.id,
		movement: 1.0,
		moved_this_turn: false,
		health: unit_health,
		morale: unit_morale,
		get_def: () => { return find_definition('ScoutPatrol'); },
		get_tile: () => { return center; },
		get_cargo: () => { return []; },
	};

	let next_unit_id = 100;
	let spawned = {};
	let rolls = [0, 2];
	let roll_index = 0;
	let messages = 0;
	let opened_events = 0;
	let earthquake_applies = 0;
	let earthquake_restores = 0;
	const native_player = {id: 8, name: 'Planet'};
	const game = {
		random: {
			get_int: (low, high) => {
				const value = rolls[roll_index];
				roll_index++;
				test.assert(value >= low && value <= high);
				return value;
			},
		},
		get_turn: () => { return 20; },
		get_player: (id) => { return player; },
		get_native_player: () => { return native_player; },
		get: (key) => {
			if (key == 'f_base_get_production_cost') {
				return (base, item) => { return item.mineral_cost; };
			}
			if (key == 'f_technology_get_next_target') {
				return (known, owner) => {
					for (candidate of ['Alpha', 'Beta']) {
						let found = false;
						for (id of known) {
							if (id == candidate) { found = true; }
						}
						if (!found) { return candidate; }
					}
					return '';
				};
			}
			if (key == 'f_technology_get_definition') {
				return (id) => { return {id: id, name: id}; };
			}
			return #undefined;
		},
		bm: {get_bases: () => { return [production_base]; }},
		tm: {
			get_distance: (first, second) => { return #abs(first.x - second.x) + #abs(first.y - second.y); },
			apply_earthquake: (tile, steps) => {
				test.assert(steps == 2);
				earthquake_applies++;
				elevation += 2000;
				return 'earthquake-snapshot';
			},
			restore_terrain: (snapshot) => {
				test.assert(snapshot == 'earthquake-snapshot');
				earthquake_restores++;
				elevation = 1000;
				terraforming_state.road = true;
				terraforming_state.mag_tube = true;
			},
		},
		um: {
			get_unit_defs: () => { return definitions; },
			spawn_unit: (data) => {
				const id = next_unit_id;
				next_unit_id++;
				const key = 'u' + #to_string(id);
				spawned[key] = {
					id: id,
					present: true,
					def: data.def,
					owner: data.owner,
					transport_id: data.transport_id,
				};
				return {
					id: id,
					movement: 1.0,
					moved_this_turn: false,
					get_def: () => { return find_definition(data.def); },
				};
			},
			has_unit: (id) => {
				const key = 'u' + #to_string(id);
				return #is_defined(spawned[key]) && spawned[key].present;
			},
			get_unit: (id) => {
				const key = 'u' + #to_string(id);
				return spawned[key];
			},
			despawn_unit: (removed) => {
				const key = 'u' + #to_string(removed.id);
				spawned[key].present = false;
			},
		},
		message: (text) => { messages++; },
		trigger: (name, data) => {
			if (name == 'unity_pod_opened') { opened_events++; }
		},
	};

	if (verify_resolve) {
		const resolved = unity_pods.resolve(game, unit, center);
		test.assert(resolved.kind == 'energy' && resolved.amount == 50);
	}

	let resolution = {kind: kind};
	if (kind == 'energy') {
		resolution.amount = 75;
	} else if (kind == 'earthquake') {
		resolution.elevation_steps = 2;
		resolution.broken_roads = [center];
	} else if (kind == 'production') {
		resolution.base = production_base;
		resolution.production_name = production.name;
		resolution.cost = production.mineral_cost;
	} else if (kind == 'artifact') {
		resolution.transport_id = 0;
	} else if (kind == 'fungus') {
		resolution.tiles = [center];
	} else if (kind == 'vehicle') {
		resolution.unit_def = 'UnityRover';
	} else if (kind == 'terraforming') {
		resolution.tiles = [center];
		resolution.improvement = 'mine';
	} else if (kind == 'clone') {
		resolution.unit_def = 'ScoutPatrol';
	} else if (kind == 'native') {
		resolution.outbreak = {
			spawns: [{
				def: 'MindWorms',
				tile: center,
				morale: 1,
				transport_index: 0 - 1,
			}],
		};
	} else if (kind == 'resource') {
		resolution.bonus = 'minerals';
	}

	const applied = unity_pods.apply(game, unit, center, resolution);
	test.assert(!feature_state.unity_pod);
	test.assert(messages == 1 && opened_events == 1);
	if (kind == 'energy') {
		test.assert(energy_credits == 175);
	} else if (kind == 'river') {
		test.assert(feature_state.river);
	} else if (kind == 'earthquake') {
		test.assert(elevation == 3000 && !terraforming_state.road && !terraforming_state.mag_tube);
	} else if (kind == 'production') {
		test.assert(base_minerals == 100);
	} else if (kind == 'artifact') {
		test.assert(game.um.get_unit(applied.spawned_unit_id).def == 'AlienArtifact');
	} else if (kind == 'fungus') {
		test.assert(feature_state.xenofungus && !terraforming_state.forest);
	} else if (kind == 'monolith') {
		test.assert(feature_state.monolith && unit.health == 1.0 && unit.morale == 3);
	} else if (kind == 'vehicle') {
		test.assert(game.um.get_unit(applied.spawned_unit_id).def == 'UnityRover');
	} else if (kind == 'technology') {
		test.assert(research.technologies == ['Alpha']);
	} else if (kind == 'terraforming') {
		test.assert(!feature_state.xenofungus && terraforming_state.mine && terraforming_state.road);
	} else if (kind == 'clone') {
		test.assert(game.um.get_unit(applied.spawned_unit_id).def == 'ScoutPatrol');
	} else if (kind == 'native') {
		test.assert(#sizeof(applied.native_outbreak.unit_ids) == 1);
		const spawned_native = game.um.get_unit(applied.native_outbreak.unit_ids[0]);
		test.assert(spawned_native.def == 'MindWorms');
		test.assert(spawned_native.owner == native_player);
	} else if (kind == 'resource') {
		test.assert(bonus_state.minerals);
	}

	unity_pods.rollback(game, applied);
	test.assert(feature_state.unity_pod);
	if (kind == 'energy') {
		test.assert(energy_credits == 100);
	} else if (kind == 'river') {
		test.assert(!feature_state.river);
	} else if (kind == 'earthquake') {
		test.assert(
			elevation == 1000 && terraforming_state.road && terraforming_state.mag_tube &&
			earthquake_applies == 1 && earthquake_restores == 1
		);
	} else if (kind == 'production') {
		test.assert(base_minerals == 10);
	} else if (kind == 'artifact' || kind == 'vehicle' || kind == 'clone') {
		test.assert(!game.um.has_unit(applied.spawned_unit_id));
	} else if (kind == 'fungus') {
		test.assert(!feature_state.xenofungus && terraforming_state.forest);
	} else if (kind == 'monolith') {
		test.assert(
			!feature_state.monolith && applied.unit_state.unit.health == unit_health &&
			applied.unit_state.unit.morale == unit_morale
		);
	} else if (kind == 'technology') {
		test.assert(research == {technologies: [], target: 'Alpha', progress: 4});
	} else if (kind == 'terraforming') {
		test.assert(feature_state.xenofungus && !terraforming_state.mine && !terraforming_state.road);
	} else if (kind == 'native') {
		test.assert(!game.um.has_unit(applied.native_outbreak.unit_ids[0]));
	} else if (kind == 'resource') {
		test.assert(!bonus_state.minerals);
	}
};

run_case('energy', true);
for (kind of [
	'river', 'earthquake', 'production', 'artifact', 'fungus', 'monolith',
	'vehicle', 'technology', 'terraforming', 'clone', 'native', 'resource'
]) {
	run_case(kind, false);
}
