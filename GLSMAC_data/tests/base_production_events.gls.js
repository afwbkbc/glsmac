const set_base_production = #include('../default/game/event/set_base_production');
const queue_base_production = #include('../default/game/event/queue_base_production');
const remove_base_production = #include('../default/game/event/remove_base_production');
const process_base_production = #include('../default/game/event/process_base_production');

let prototyped_components = ['ColonyModule', 'HandWeapons', 'Infantry', 'NoArmor'];
let orbital_counts = {};
const owner = {
	id: 1,
	name: 'University',
	get_faction: () => { return {id: 'UNIVERSITY'}; },
	get_prototyped_components: () => { return #clone(prototyped_components); },
	has_prototyped_component: (id) => {
		for (component of prototyped_components) {
			if (component == id) {
				return true;
			}
		}
		return false;
	},
	set_prototyped_components: (value) => {
		prototyped_components = #clone(value);
	},
	get_orbital_facility_count: (id) => {
		return #is_defined(orbital_counts[id]) ? orbital_counts[id] : 0;
	},
	set_orbital_facility_count: (id, count) => {
		orbital_counts[id] = count == 0 ? #undefined : count;
	},
};
const tile = {id: 'base-tile'};
const mind_worms = {
	id: 'MindWorms',
	name: 'Mind Worms',
	production_kind: 'unit',
	mineral_cost: 30,
	can_found_base: false,
	is_native: true,
	morale_set: 'NATIVE',
};
const spore_launcher = {
	id: 'SporeLauncher',
	name: 'Spore Launcher',
	production_kind: 'unit',
	mineral_cost: 50,
	can_found_base: false,
	is_native: true,
	morale_set: 'NATIVE',
};
const colony_pod = {
	id: 'ColonyPod',
	name: 'Colony Pod',
	production_kind: 'unit',
	mineral_cost: 30,
	can_found_base: true,
	is_native: false,
	offense: 0,
	morale_set: 'STANDARD',
	is_land: true,
	is_water: false,
	is_air: false,
};
const land_patrol = {
	id: 'LandPatrol',
	name: 'Land Patrol',
	production_kind: 'unit',
	mineral_cost: 20,
	can_found_base: false,
	is_native: false,
	offense: 1,
	morale_set: 'STANDARD',
	is_land: true,
	is_water: false,
	is_air: false,
	abilities: [],
};
const trained_land_patrol = {
	id: 'TrainedLandPatrol',
	name: 'Trained Land Patrol',
	production_kind: 'unit',
	mineral_cost: 20,
	can_found_base: false,
	is_native: false,
	offense: 1,
	morale_set: 'STANDARD',
	is_land: true,
	is_water: false,
	is_air: false,
	abilities: ['HighMorale'],
};
const prototype_patrol = {
	id: 'PrototypePatrol',
	name: 'Prototype Patrol',
	production_kind: 'unit',
	mineral_cost: 20,
	can_found_base: false,
	is_native: false,
	offense: 2,
	morale_set: 'STANDARD',
	is_land: true,
	is_water: false,
	is_air: false,
	chassis: 'Speeder',
	weapon: 'Laser',
	armor: 'NoArmor',
	abilities: [],
};
const sea_patrol = {
	id: 'SeaPatrol',
	name: 'Sea Patrol',
	production_kind: 'unit',
	mineral_cost: 20,
	can_found_base: false,
	is_native: false,
	offense: 1,
	morale_set: 'STANDARD',
	is_land: false,
	is_water: true,
	is_air: false,
};
const air_patrol = {
	id: 'AirPatrol',
	name: 'Air Patrol',
	production_kind: 'unit',
	mineral_cost: 20,
	can_found_base: false,
	is_native: false,
	offense: 1,
	morale_set: 'STANDARD',
	is_land: false,
	is_water: false,
	is_air: true,
};
const recycling_tanks = {
	id: 'RecyclingTanks',
	name: 'Recycling Tanks',
	production_kind: 'facility',
	mineral_cost: 40,
	unit_morale_bonus: 0,
};
const headquarters = {
	id: 'Headquarters',
	name: 'Headquarters',
	production_kind: 'facility',
	mineral_cost: 50,
	unit_morale_bonus: 0,
};
const recreation_commons = {
	id: 'RecreationCommons',
	name: 'Recreation Commons',
	production_kind: 'facility',
	mineral_cost: 40,
	unit_morale_bonus: 0,
};
const human_genome_project = {
	id: 'TheHumanGenomeProject',
	name: 'The Human Genome Project',
	production_kind: 'project',
	mineral_cost: 200,
	unit_morale_bonus: 0,
};
const planetary_datalinks = {
	id: 'ThePlanetaryDatalinks',
	name: 'The Planetary Datalinks',
	production_kind: 'project',
	mineral_cost: 300,
	unit_morale_bonus: 0,
};
const universal_translator = {
	id: 'TheUniversalTranslator',
	name: 'The Universal Translator',
	production_kind: 'project',
	mineral_cost: 300,
	unit_morale_bonus: 0,
};
const command_center = {
	id: 'CommandCenter',
	name: 'Command Center',
	production_kind: 'facility',
	mineral_cost: 40,
	unit_morale_bonus: 0,
	unit_morale_land_bonus: 2,
};
const naval_yard = {
	id: 'NavalYard',
	name: 'Naval Yard',
	production_kind: 'facility',
	mineral_cost: 80,
	unit_morale_bonus: 0,
	unit_morale_water_bonus: 2,
};
const aerospace_complex = {
	id: 'AerospaceComplex',
	name: 'Aerospace Complex',
	production_kind: 'facility',
	mineral_cost: 80,
	unit_morale_bonus: 0,
	unit_morale_air_bonus: 2,
};
const bioenhancement_center = {
	id: 'BioenhancementCenter',
	name: 'Bioenhancement Center',
	production_kind: 'facility',
	mineral_cost: 100,
	unit_morale_bonus: 2,
	native_lifecycle_bonus: 1,
};
const biology_lab = {
	id: 'BiologyLab',
	name: 'Biology Lab',
	production_kind: 'facility',
	mineral_cost: 60,
	unit_morale_bonus: 0,
	native_lifecycle_bonus: 1,
};
const centauri_preserve = {
	id: 'CentauriPreserve',
	name: 'Centauri Preserve',
	production_kind: 'facility',
	mineral_cost: 100,
	unit_morale_bonus: 0,
	native_lifecycle_bonus: 1,
};
const temple_of_planet = {
	id: 'TempleOfPlanet',
	name: 'Temple of Planet',
	production_kind: 'facility',
	mineral_cost: 200,
	unit_morale_bonus: 0,
	native_lifecycle_bonus: 1,
};
const stockpile_energy = {
	id: 'StockpileEnergy',
	name: 'Stockpile Energy',
	production_kind: 'facility',
	mineral_cost: 0,
	mineral_to_energy_divisor: 2,
	unit_morale_bonus: 0,
};
const sky_hydroponics = {
	id: 'SkyHydroponicsLab',
	name: 'Sky Hydroponics Lab',
	production_kind: 'facility',
	mineral_cost: 120,
	orbital_resource: 'NUTRIENTS',
	orbital_defense: false,
	unit_morale_bonus: 0,
};
const definitions = [
	mind_worms,
	spore_launcher,
	colony_pod,
	land_patrol,
	trained_land_patrol,
	prototype_patrol,
	sea_patrol,
	air_patrol,
	recycling_tanks,
	headquarters,
	recreation_commons,
	human_genome_project,
	planetary_datalinks,
	universal_translator,
	command_center,
	naval_yard,
	aerospace_complex,
	bioenhancement_center,
	biology_lab,
	centauri_preserve,
	temple_of_planet,
	stockpile_energy,
	sky_hydroponics,
];

let production_queue = [];
let built_facilities = [];
let accumulated_minerals = 0;
let pending_production = 7;
let social_morale = 0;
let spawned_unit = #undefined;
let spawn_data = #undefined;
let despawned_unit = #undefined;
let base_pops = [];
let processed_psych = [];
let completed_project_base = #undefined;
let competing_production_queue = [];
let competing_has_headquarters = false;
let datalinks_queues = 0;
let project_completion_applications = [];
let project_completion_rollbacks = [];
let base_custom = {};
let competing_custom = {};

const make_pop = (type, worked_tile) => {
	let tile = worked_tile;
	return {
		get_type: () => { return type; },
		set_type: (value) => { type = value; },
		get: (key) => { return key == 'worked_tile' ? tile : #undefined; },
		has: (key) => { return key == 'worked_tile' && #is_defined(tile); },
		clear_tile: () => { tile = #undefined; },
		set_tile: (value) => { tile = value; },
	};
};

const find_definition = (kind, id) => {
	for (definition of definitions) {
		if (definition.production_kind == kind && definition.id == id) {
			return definition;
		}
	}
	return #undefined;
};

const has_facility = (id) => {
	for (facility_id of built_facilities) {
		if (facility_id == id) {
			return true;
		}
	}
	return false;
};

const queue_is_valid = (candidate_queue) => {
	if (#sizeof(candidate_queue) > 8) {
		return false;
	}
	let queued_facilities = [];
	for (item of candidate_queue) {
		const definition = find_definition(item.production_kind, item.id);
		if (!#is_defined(definition)) {
			return false;
		}
		if (item.production_kind == 'facility' || item.production_kind == 'project') {
			const is_repeatable = item.production_kind == 'facility' && (
				(
					#is_defined(definition.mineral_to_energy_divisor) &&
					definition.mineral_to_energy_divisor > 0
				) || (
					#is_defined(definition.orbital_resource) &&
					definition.orbital_resource != ''
				) || (
					#is_defined(definition.orbital_defense) &&
					definition.orbital_defense
				)
			);
			if (!is_repeatable && has_facility(item.id)) {
				return false;
			}
			if (item.production_kind == 'project' && #is_defined(completed_project_base)) {
				return false;
			}
			if (!is_repeatable) {
				for (facility_id of queued_facilities) {
					if (facility_id == item.id) {
						return false;
					}
				}
				queued_facilities :+item.id;
			}
		}
	}
	return true;
};

const get_queue_state = () => {
	let result = [];
	for (item of production_queue) {
		result :+item.production_kind + ':' + item.id;
	}
	return result;
};

const base = {
	id: 11,
	get_owner: () => {
		return owner;
	},
	get_tile: () => {
		return tile;
	},
	get_size: () => {
		return #sizeof(base_pops);
	},
	get_pops: () => {
		return base_pops;
	},
	get_production: () => {
		return #sizeof(production_queue) > 0
			? production_queue[0]
			: #undefined;
	},
	get_production_queue: () => {
		return production_queue;
	},
	get_facilities: () => {
		let result = [];
		for (id of built_facilities) {
			const facility = find_definition('facility', id);
			const definition = #is_defined(facility)
				? facility
				: find_definition('project', id);
			result :+definition;
		}
		return result;
	},
	can_set_production: (kind, id) => {
		const definition = find_definition(kind, id);
		if (!#is_defined(definition)) {
			return false;
		}
		let candidate_queue = [];
		for (item of production_queue) {
			candidate_queue :+item;
		}
		if (#sizeof(candidate_queue) == 0) {
			candidate_queue :+definition;
		} else {
			candidate_queue[0] = definition;
		}
		return queue_is_valid(candidate_queue);
	},
	can_queue_production: (kind, id) => {
		const definition = find_definition(kind, id);
		if (
			!#is_defined(definition) ||
			(
				#is_defined(definition.mineral_to_energy_divisor) &&
				definition.mineral_to_energy_divisor > 0
			)
		) {
			return false;
		}
		let candidate_queue = [];
		for (item of production_queue) {
			candidate_queue :+item;
		}
		candidate_queue :+definition;
		return queue_is_valid(candidate_queue);
	},
	set_production: (kind, id) => {
		const definition = find_definition(kind, id);
		if (#sizeof(production_queue) == 0) {
			production_queue :+definition;
		} else {
			production_queue[0] = definition;
		}
	},
	queue_production: (kind, id) => {
		production_queue :+find_definition(kind, id);
	},
	remove_production: (index) => {
		let updated_queue = [];
		let i = 0;
		for (item of production_queue) {
			if (i != index) {
				updated_queue :+item;
			}
			i++;
		}
		production_queue = updated_queue;
	},
	set_production_queue: (specs) => {
		let restored_queue = [];
		for (spec of specs) {
			restored_queue :+find_definition(spec.kind, spec.id);
		}
		test.assert(queue_is_valid(restored_queue));
		production_queue = restored_queue;
	},
	clear_production: () => {
		production_queue = [];
	},
	has_facility: (id) => {
		return has_facility(id);
	},
	add_facility: (id) => {
		test.assert(id != 'SkyHydroponicsLab');
		test.assert(!has_facility(id));
		built_facilities :+id;
		if (#is_defined(find_definition('project', id))) {
			test.assert(!#is_defined(completed_project_base));
			completed_project_base = owner;
		}
	},
	remove_facility: (id) => {
		let updated_facilities = [];
		let removed = false;
		for (facility_id of built_facilities) {
			if (facility_id == id) {
				removed = true;
			} else {
				updated_facilities :+facility_id;
			}
		}
		test.assert(removed);
		built_facilities = updated_facilities;
		if (#is_defined(find_definition('project', id))) {
			completed_project_base = #undefined;
		}
	},
	get_accumulated_minerals: () => {
		return accumulated_minerals;
	},
	set_accumulated_minerals: (minerals) => {
		accumulated_minerals = minerals;
	},
	unwork_pop_tile: (pop, worked_tile) => {
		test.assert(pop.get('worked_tile') == worked_tile);
		pop.clear_tile();
	},
	work_pop_tile: (pop, worked_tile) => {
		test.assert(!pop.has('worked_tile'));
		pop.set_tile(worked_tile);
	},
	destroy_pop: (pop) => {
		let remaining = [];
		for (candidate of base_pops) {
			if (candidate != pop) {
				remaining :+candidate;
			}
		}
		base_pops = remaining;
	},
	create_pop: (data) => {
		const pop = make_pop(data.type, #undefined);
		base_pops :+pop;
		return pop;
	},
	has: (key) => { return #is_defined(base_custom[key]); },
	get: (key) => { return base_custom[key]; },
	set: (key, value) => { base_custom[key] = value; },
	unset: (key) => { base_custom[key] = #undefined; },
};

const competing_base = {
	id: 12,
	get_owner: () => { return owner; },
	get_production_queue: () => { return competing_production_queue; },
	has_facility: (id) => { return id == 'Headquarters' && competing_has_headquarters; },
	remove_facility: (id) => {
		test.assert(id == 'Headquarters' && competing_has_headquarters);
		competing_has_headquarters = false;
	},
	add_facility: (id) => {
		test.assert(id == 'Headquarters' && !competing_has_headquarters);
		competing_has_headquarters = true;
	},
	remove_production: (index) => {
		let updated = [];
		for (let i = 0; i < #sizeof(competing_production_queue); i++) {
			if (i != index) {
				updated :+competing_production_queue[i];
			}
		}
		competing_production_queue = updated;
	},
	set_production_queue: (specs) => {
		let restored = [];
		for (spec of specs) {
			restored :+find_definition(spec.kind, spec.id);
		}
		competing_production_queue = restored;
	},
	has: (key) => { return #is_defined(competing_custom[key]); },
	get: (key) => { return competing_custom[key]; },
	set: (key, value) => { competing_custom[key] = value; },
	unset: (key) => { competing_custom[key] = #undefined; },
};

let turn_complete = false;
let game = null;
game = {
	trigger: (name, data) => {},
	get_bm: () => {
		return {
			get_bases: () => { return [base, competing_base]; },
			get_project_base: (id) => { return completed_project_base; },
		};
	},
	is_turn_complete: (player_id) => {
		test.assert(player_id == owner.id);
		return turn_complete;
	},
	get: (key) => {
		if (key == 'f_base_get_effective_facilities' || key == 'f_project_get_effects') {
			return #undefined;
		}
		if (key == 'f_social_get_unit_training_morale_bonus') {
			return (player, bonus) => {
				test.assert(player == owner);
				return social_morale <= 0 - 2
					? #floor(#to_float(bonus) / 2.0)
					: bonus;
			};
		}
		if (key == 'f_base_get_pending_production') {
			return (target_base) => {
				test.assert(target_base == base);
				return pending_production;
			};
		}
		if (key == 'f_base_get_production_cost') {
			return (target_base, production) => {
				test.assert(target_base == base);
				return production.mineral_cost;
			};
		}
		if (key == 'f_base_select_population_for_reduction') {
			return (target_base) => {
				test.assert(target_base == base);
				for (pop of base_pops) {
					if (!pop.has('worked_tile')) {
						return pop;
					}
				}
				return #sizeof(base_pops) > 0 ? base_pops[0] : null;
			};
		}
		if (key == 'f_economy_get_base_psych') {
			return (target_game, target_base) => {
				test.assert(target_game == game && target_base == base);
				return has_facility('RecreationCommons') ? 4 : 0;
			};
		}
		if (key == 'f_base_process_psych') {
			return (target_game, target_base, psych) => {
				test.assert(target_game == game && target_base == base);
				processed_psych :+psych;
				let laborer_count = 0;
				for (pop of base_pops) {
					if (pop.has('worked_tile')) {
						pop.set_type(laborer_count < 3 ? 'WORKER' : 'DRONE');
						laborer_count++;
					}
				}
				for (pop of base_pops) {
					if (psych < 2) {
						break;
					}
					if (pop.has('worked_tile') && pop.get_type() == 'DRONE') {
						pop.set_type('WORKER');
						psych -= 2;
					}
				}
				for (pop of base_pops) {
					if (psych < 2) {
						break;
					}
					if (pop.has('worked_tile') && pop.get_type() == 'WORKER') {
						pop.set_type('TALENT');
						psych -= 2;
					}
				}
			};
		}
		if (key == 'f_project_queue_planetary_datalinks') {
			return () => { datalinks_queues++; };
		}
		if (key == 'f_project_apply_completion_effects') {
			return (target_base, project_id) => {
				test.assert(target_base == base);
				project_completion_applications :+project_id;
				return project_id == 'TheUniversalTranslator'
					? {project_id: project_id}
					: #undefined;
			};
		}
		if (key == 'f_project_rollback_completion_effects') {
			return (applied) => {
				project_completion_rollbacks :+applied.project_id;
			};
		}
		if (key == 'f_orbital_apply_launch') {
			return (target_base, definition) => {
				test.assert(target_base == base && definition == sky_hydroponics);
				const count = owner.get_orbital_facility_count(definition.id);
				owner.set_orbital_facility_count(definition.id, count + 1);
				return {player: owner, id: definition.id, count: count};
			};
		}
		if (key == 'f_orbital_rollback_launch') {
			return (applied) => {
				applied.player.set_orbital_facility_count(applied.id, applied.count);
			};
		}
		throw Error('Unexpected game callback: ' + key);
	},
	um: {
		get_moraleset: (id) => { return [0, 1, 2, 3, 4, 5, 6]; },
		spawn_unit: (data) => {
			spawn_data = data;
			spawned_unit = {id: 17};
			return spawned_unit;
		},
		despawn_unit: (unit) => {
			despawned_unit = unit;
			spawned_unit = #undefined;
		},
	},
};

production_queue = [mind_worms, spore_launcher];
let event = {
	caller: owner.id,
	game: game,
	data: {
		base: base,
		kind: 'facility',
		id: 'RecyclingTanks',
	},
};
test.assert(!#is_defined(set_base_production.validate(event)));
event.caller = 2;
test.assert(#is_defined(set_base_production.validate(event)));
event.caller = owner.id;
turn_complete = true;
test.assert(#is_defined(set_base_production.validate(event)));
turn_complete = false;
event.data.kind = 1;
test.assert(#is_defined(set_base_production.validate(event)));
event.data.kind = 'facility';
event.data.id = 1;
test.assert(#is_defined(set_base_production.validate(event)));
event.data.id = 'RecyclingTanks';
event.data.kind = 'unit';
event.data.id = 'FungalTower';
test.assert(#is_defined(set_base_production.validate(event)));
event.data.kind = 'facility';
event.data.id = 'RecyclingTanks';

event.applied = set_base_production.apply(event);
test.assert(get_queue_state() == ['facility:RecyclingTanks', 'unit:SporeLauncher']);
set_base_production.rollback(event);
test.assert(get_queue_state() == ['unit:MindWorms', 'unit:SporeLauncher']);

production_queue = [];
event.applied = set_base_production.apply(event);
test.assert(get_queue_state() == ['facility:RecyclingTanks']);
set_base_production.rollback(event);
test.assert(get_queue_state() == []);

production_queue = [mind_worms];
event = {
	caller: owner.id,
	game: game,
	data: {
		base: base,
		kind: 'unit',
		id: 'SporeLauncher',
	},
};
test.assert(!#is_defined(queue_base_production.validate(event)));
event.caller = 2;
test.assert(#is_defined(queue_base_production.validate(event)));
event.caller = owner.id;
turn_complete = true;
test.assert(#is_defined(queue_base_production.validate(event)));
turn_complete = false;
event.data.kind = 1;
test.assert(#is_defined(queue_base_production.validate(event)));
event.data.kind = 'unit';
event.data.id = 1;
test.assert(#is_defined(queue_base_production.validate(event)));
event.data.id = 'SporeLauncher';

event.applied = queue_base_production.apply(event);
test.assert(get_queue_state() == ['unit:MindWorms', 'unit:SporeLauncher']);
queue_base_production.rollback(event);
test.assert(get_queue_state() == ['unit:MindWorms']);

production_queue = [mind_worms, recycling_tanks];
event.data.kind = 'facility';
event.data.id = 'RecyclingTanks';
test.assert(#is_defined(queue_base_production.validate(event)));
production_queue = [mind_worms, mind_worms, mind_worms, mind_worms, mind_worms, mind_worms, mind_worms, mind_worms];
event.data.kind = 'unit';
event.data.id = 'SporeLauncher';
test.assert(#is_defined(queue_base_production.validate(event)));

production_queue = [mind_worms, recycling_tanks, spore_launcher];
event = {
	caller: owner.id,
	game: game,
	data: {
		base: base,
		index: 1,
	},
};
test.assert(!#is_defined(remove_base_production.validate(event)));
event.caller = 2;
test.assert(#is_defined(remove_base_production.validate(event)));
event.caller = owner.id;
turn_complete = true;
test.assert(#is_defined(remove_base_production.validate(event)));
turn_complete = false;
event.data.index = -1;
test.assert(#is_defined(remove_base_production.validate(event)));
event.data.index = 3;
test.assert(#is_defined(remove_base_production.validate(event)));
event.data.index = 1.0;
test.assert(#is_defined(remove_base_production.validate(event)));
event.data.index = '1';
test.assert(#is_defined(remove_base_production.validate(event)));
event.data.index = 1;

event.applied = remove_base_production.apply(event);
test.assert(get_queue_state() == ['unit:MindWorms', 'unit:SporeLauncher']);
remove_base_production.rollback(event);
test.assert(get_queue_state() == ['unit:MindWorms', 'facility:RecyclingTanks', 'unit:SporeLauncher']);

production_queue = [mind_worms, spore_launcher];
built_facilities = ['CommandCenter'];
accumulated_minerals = 25;
spawned_unit = #undefined;
spawn_data = #undefined;
despawned_unit = #undefined;
event = {
	caller: 0,
	game: game,
	data: {
		base: base,
	},
};
test.assert(!#is_defined(process_base_production.validate(event)));
event.caller = owner.id;
test.assert(#is_defined(process_base_production.validate(event)));
event.caller = 0;

event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 2);
test.assert(get_queue_state() == ['unit:SporeLauncher']);
test.assert(#is_defined(spawned_unit));
test.assert(spawn_data.def == mind_worms.id);
test.assert(spawn_data.owner == owner);
test.assert(spawn_data.tile == tile);
test.assert(spawn_data.morale == 1);
test.assert(spawn_data.health == 1.0);
test.assert(spawn_data.home_base_id == base.id);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 25);
test.assert(get_queue_state() == ['unit:MindWorms', 'unit:SporeLauncher']);
test.assert(!#is_defined(spawned_unit));
test.assert(despawned_unit.id == 17);
built_facilities = [];

production_queue = [stockpile_energy];
accumulated_minerals = 13;
pending_production = 7;
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 13);
test.assert(get_queue_state() == ['facility:StockpileEnergy']);
test.assert(!has_facility('StockpileEnergy'));
process_base_production.rollback(event);
test.assert(accumulated_minerals == 13);
test.assert(get_queue_state() == ['facility:StockpileEnergy']);
pending_production = 7;

production_queue = [sky_hydroponics, sky_hydroponics];
test.assert(queue_is_valid(production_queue));
accumulated_minerals = 115;
orbital_counts = {};
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 2);
test.assert(get_queue_state() == ['facility:SkyHydroponicsLab']);
test.assert(owner.get_orbital_facility_count('SkyHydroponicsLab') == 1);
test.assert(!has_facility('SkyHydroponicsLab'));
process_base_production.rollback(event);
test.assert(accumulated_minerals == 115);
test.assert(get_queue_state() == [
	'facility:SkyHydroponicsLab', 'facility:SkyHydroponicsLab',
]);
test.assert(owner.get_orbital_facility_count('SkyHydroponicsLab') == 0);

production_queue = [mind_worms];
built_facilities = [
	'BiologyLab',
	'BioenhancementCenter',
	'CentauriPreserve',
	'TempleOfPlanet',
];
accumulated_minerals = 25;
spawned_unit = #undefined;
spawn_data = #undefined;
event.applied = process_base_production.apply(event);
test.assert(#is_defined(spawned_unit));
test.assert(spawn_data.def == mind_worms.id);
test.assert(spawn_data.morale == 5);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 25);
built_facilities = [];

production_queue = [mind_worms];
accumulated_minerals = 25;
spawned_unit = #undefined;
despawned_unit = #undefined;
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 2);
test.assert(get_queue_state() == ['unit:MindWorms']);
test.assert(#is_defined(spawned_unit));
process_base_production.rollback(event);
test.assert(accumulated_minerals == 25);
test.assert(get_queue_state() == ['unit:MindWorms']);

production_queue = [mind_worms];
accumulated_minerals = 10;
spawn_data = #undefined;
despawned_unit = #undefined;
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 17);
test.assert(get_queue_state() == ['unit:MindWorms']);
test.assert(!#is_defined(spawn_data));
process_base_production.rollback(event);
test.assert(accumulated_minerals == 10);
test.assert(get_queue_state() == ['unit:MindWorms']);
test.assert(!#is_defined(despawned_unit));

production_queue = [recycling_tanks, spore_launcher];
built_facilities = [];
accumulated_minerals = 35;
spawn_data = #undefined;
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 2);
test.assert(get_queue_state() == ['unit:SporeLauncher']);
test.assert(has_facility('RecyclingTanks'));
test.assert(!#is_defined(spawn_data));
process_base_production.rollback(event);
test.assert(accumulated_minerals == 35);
test.assert(get_queue_state() == ['facility:RecyclingTanks', 'unit:SporeLauncher']);
test.assert(!has_facility('RecyclingTanks'));

production_queue = [human_genome_project, spore_launcher];
competing_production_queue = [land_patrol, human_genome_project, sea_patrol];
built_facilities = [];
completed_project_base = #undefined;
accumulated_minerals = 195;
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 2);
test.assert(get_queue_state() == ['unit:SporeLauncher']);
test.assert(has_facility('TheHumanGenomeProject'));
test.assert(completed_project_base == owner);
test.assert(#sizeof(event.applied.cancelled_project_queues) == 1);
test.assert(competing_production_queue == [land_patrol, sea_patrol]);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 195);
test.assert(get_queue_state() == ['project:TheHumanGenomeProject', 'unit:SporeLauncher']);
test.assert(competing_production_queue == [land_patrol, human_genome_project, sea_patrol]);
test.assert(!has_facility('TheHumanGenomeProject'));
test.assert(!#is_defined(completed_project_base));

production_queue = [planetary_datalinks];
competing_production_queue = [];
built_facilities = [];
accumulated_minerals = 295;
event.applied = process_base_production.apply(event);
test.assert(has_facility('ThePlanetaryDatalinks'));
test.assert(datalinks_queues == 1);
process_base_production.rollback(event);
test.assert(!has_facility('ThePlanetaryDatalinks'));

production_queue = [universal_translator];
competing_production_queue = [];
built_facilities = [];
completed_project_base = #undefined;
accumulated_minerals = 295;
project_completion_applications = [];
project_completion_rollbacks = [];
event.applied = process_base_production.apply(event);
test.assert(has_facility('TheUniversalTranslator'));
test.assert(project_completion_applications == ['TheUniversalTranslator']);
test.assert(event.applied.project_completion_effects == {
	project_id: 'TheUniversalTranslator',
});
process_base_production.rollback(event);
test.assert(!has_facility('TheUniversalTranslator'));
test.assert(project_completion_rollbacks == ['TheUniversalTranslator']);

production_queue = [];
accumulated_minerals = 9;
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 9);
test.assert(!#is_defined(event.applied.produced_unit));
test.assert(!#is_defined(event.applied.completed_facility));
process_base_production.rollback(event);
test.assert(accumulated_minerals == 9);
test.assert(get_queue_state() == []);

const worked_tile = {id: 'worked-tile'};
const worker_pop = make_pop('WORKER', worked_tile);
production_queue = [colony_pod];
base_pops = [worker_pop];
accumulated_minerals = 25;
spawned_unit = #undefined;
spawn_data = #undefined;
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 32);
test.assert(!#is_defined(spawned_unit));
test.assert(#sizeof(base_pops) == 1);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 25);
test.assert(#sizeof(base_pops) == 1);

const doctor_pop = make_pop('DOCTOR', #undefined);
base_pops = [worker_pop, doctor_pop];
built_facilities = ['CommandCenter', 'BioenhancementCenter'];
accumulated_minerals = 25;
spawned_unit = #undefined;
spawn_data = #undefined;
event.applied = process_base_production.apply(event);
test.assert(accumulated_minerals == 2);
test.assert(#is_defined(spawned_unit));
test.assert(spawn_data.def == colony_pod.id);
test.assert(spawn_data.morale == 3);
test.assert(#sizeof(base_pops) == 1);
test.assert(base_pops[0] == worker_pop);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 25);
test.assert(!#is_defined(spawned_unit));
test.assert(#sizeof(base_pops) == 2);
test.assert(base_pops[1].get_type() == 'DOCTOR');
built_facilities = [];

for (triad_unit of [land_patrol, sea_patrol, air_patrol]) {
	production_queue = [triad_unit];
	built_facilities = [
		'CommandCenter',
		'NavalYard',
		'AerospaceComplex',
		'BioenhancementCenter',
	];
	accumulated_minerals = 13;
	spawned_unit = #undefined;
	spawn_data = #undefined;
	event.applied = process_base_production.apply(event);
	test.assert(#is_defined(spawned_unit));
	test.assert(spawn_data.def == triad_unit.id);
	test.assert(spawn_data.morale == 5);
	process_base_production.rollback(event);
	test.assert(accumulated_minerals == 13);
	test.assert(!#is_defined(spawned_unit));
}
built_facilities = [];

social_morale = 0 - 2;
production_queue = [mind_worms];
built_facilities = [
	'BiologyLab',
	'BioenhancementCenter',
	'CentauriPreserve',
	'TempleOfPlanet',
];
accumulated_minerals = 25;
spawned_unit = #undefined;
spawn_data = #undefined;
event.applied = process_base_production.apply(event);
test.assert(#is_defined(spawned_unit));
test.assert(spawn_data.def == mind_worms.id);
test.assert(spawn_data.morale == 5);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 25);

for (triad_unit of [land_patrol, sea_patrol, air_patrol]) {
	production_queue = [triad_unit];
	built_facilities = [
		'CommandCenter',
		'NavalYard',
		'AerospaceComplex',
		'BioenhancementCenter',
	];
	accumulated_minerals = 13;
	spawned_unit = #undefined;
	spawn_data = #undefined;
	event.applied = process_base_production.apply(event);
	test.assert(#is_defined(spawned_unit));
	test.assert(spawn_data.def == triad_unit.id);
	test.assert(spawn_data.morale == 3);
	process_base_production.rollback(event);
	test.assert(accumulated_minerals == 13);
	test.assert(!#is_defined(spawned_unit));
}
built_facilities = [];

production_queue = [trained_land_patrol];
accumulated_minerals = 13;
spawned_unit = #undefined;
spawn_data = #undefined;
event.applied = process_base_production.apply(event);
test.assert(#is_defined(spawned_unit));
test.assert(spawn_data.def == trained_land_patrol.id);
test.assert(spawn_data.morale == 2);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 13);
test.assert(!#is_defined(spawned_unit));

production_queue = [trained_land_patrol];
built_facilities = ['CommandCenter'];
accumulated_minerals = 13;
spawned_unit = #undefined;
spawn_data = #undefined;
event.applied = process_base_production.apply(event);
test.assert(#is_defined(spawned_unit));
test.assert(spawn_data.def == trained_land_patrol.id);
test.assert(spawn_data.morale == 3);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 13);
test.assert(!#is_defined(spawned_unit));
built_facilities = [];
social_morale = 0;

prototyped_components = ['ColonyModule', 'HandWeapons', 'Infantry', 'NoArmor'];
production_queue = [prototype_patrol];
accumulated_minerals = 13;
spawned_unit = #undefined;
spawn_data = #undefined;
event.applied = process_base_production.apply(event);
test.assert(#is_defined(spawned_unit));
test.assert(spawn_data.def == prototype_patrol.id);
test.assert(spawn_data.morale == 2);
test.assert(prototyped_components == [
	'ColonyModule', 'HandWeapons', 'Infantry', 'NoArmor', 'Speeder', 'Laser',
]);
process_base_production.rollback(event);
test.assert(accumulated_minerals == 13);
test.assert(!#is_defined(spawned_unit));
test.assert(prototyped_components == [
	'ColonyModule', 'HandWeapons', 'Infantry', 'NoArmor',
]);

const worker_a = make_pop('WORKER', {id: 'worker-a'});
const worker_b = make_pop('WORKER', {id: 'worker-b'});
const worker_c = make_pop('WORKER', {id: 'worker-c'});
const drone_d = make_pop('DRONE', {id: 'drone-d'});
production_queue = [colony_pod];
base_pops = [worker_a, worker_b, worker_c, drone_d];
accumulated_minerals = 25;
spawned_unit = #undefined;
spawn_data = #undefined;
processed_psych = [];
event.applied = process_base_production.apply(event);
test.assert(processed_psych == [0]);
test.assert(#sizeof(base_pops) == 3);
test.assert(base_pops[0].get_type() == 'WORKER');
test.assert(base_pops[1].get_type() == 'WORKER');
test.assert(base_pops[2].get_type() == 'WORKER');
process_base_production.rollback(event);
test.assert(#sizeof(base_pops) == 4);
test.assert(base_pops[0].get_type() == 'WORKER');
test.assert(base_pops[1].get_type() == 'WORKER');
test.assert(base_pops[2].get_type() == 'DRONE');
test.assert(base_pops[3].get_type() == 'WORKER');

const commons_worker_a = make_pop('WORKER', {id: 'commons-a'});
const commons_worker_b = make_pop('WORKER', {id: 'commons-b'});
const commons_worker_c = make_pop('WORKER', {id: 'commons-c'});
const commons_drone_d = make_pop('DRONE', {id: 'commons-d'});
production_queue = [recreation_commons];
base_pops = [commons_worker_a, commons_worker_b, commons_worker_c, commons_drone_d];
built_facilities = [];
accumulated_minerals = 35;
processed_psych = [];
event.applied = process_base_production.apply(event);
test.assert(has_facility('RecreationCommons'));
test.assert(processed_psych == [4]);
test.assert(base_pops[0].get_type() == 'TALENT');
test.assert(base_pops[1].get_type() == 'WORKER');
test.assert(base_pops[2].get_type() == 'WORKER');
test.assert(base_pops[3].get_type() == 'WORKER');
process_base_production.rollback(event);
test.assert(!has_facility('RecreationCommons'));
test.assert(base_pops[0].get_type() == 'WORKER');
test.assert(base_pops[1].get_type() == 'WORKER');
test.assert(base_pops[2].get_type() == 'WORKER');
test.assert(base_pops[3].get_type() == 'DRONE');

production_queue = [headquarters];
built_facilities = [];
competing_has_headquarters = true;
competing_custom = {
	economic_victory_turn: 42,
	economic_victory_cost: 1800,
};
base_custom = {};
accumulated_minerals = 45;
pending_production = 7;
processed_psych = [];
event.applied = process_base_production.apply(event);
test.assert(has_facility('Headquarters'));
test.assert(!competing_has_headquarters);
test.assert(#sizeof(event.applied.previous_headquarters) == 1);
test.assert(event.applied.previous_headquarters[0] == competing_base);
test.assert(base_custom.economic_victory_turn == 42);
test.assert(base_custom.economic_victory_cost == 1800);
test.assert(!#is_defined(competing_custom.economic_victory_turn));
process_base_production.rollback(event);
test.assert(!has_facility('Headquarters'));
test.assert(competing_has_headquarters);
test.assert(!#is_defined(base_custom.economic_victory_turn));
test.assert(competing_custom.economic_victory_turn == 42);
test.assert(competing_custom.economic_victory_cost == 1800);
competing_custom = {};
