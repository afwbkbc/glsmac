const MAX_DEFINITION_VALUE = 1000000;

const technology_fields = {
	id: true,
	name: true,
	cost: true,
	commerce_bonus: true,
	prerequisites: true,
};

const facility_fields = {
	name: true,
	mineral_cost: true,
	nutrient_bonus: true,
	mineral_bonus: true,
	energy_bonus: true,
	energy_maintenance: true,
	required_technology: true,
	psych_bonus: true,
	research_multiplier: true,
	defense_multiplier: true,
	economy_multiplier: true,
	unit_morale_bonus: true,
	research_bonus: true,
	mineral_multiplier: true,
	psych_multiplier: true,
	population_limit: true,
	required_facility: true,
	required_project: true,
	drone_modifier: true,
	talent_bonus: true,
	suppress_psych: true,
	unit_morale_land_bonus: true,
	unit_morale_water_bonus: true,
	unit_morale_air_bonus: true,
	water_defense_multiplier: true,
	air_defense_multiplier: true,
	growth_rating_bonus: true,
	native_lifecycle_bonus: true,
	is_project: true,
	granted_facility: true,
	global_talent_bonus: true,
	global_growth_rating_bonus: true,
	global_population_limit_bonus: true,
	global_mineral_bonus: true,
	global_support_bonus: true,
	global_maintenance_multiplier: true,
	global_native_lifecycle_bonus: true,
	network_node_drone_modifier: true,
	network_node_research_bonus: true,
	worked_tile_energy_bonus: true,
	forest_nutrient_bonus: true,
	forest_mineral_bonus: true,
	forest_energy_bonus: true,
	full_repair_land: true,
	full_repair_water: true,
	full_repair_air: true,
	full_repair_native: true,
	defender_morale_bonus: true,
	global_prevent_riots: true,
	global_terraforming_rate_multiplier: true,
	new_base_population: true,
	small_base_drone_modifier: true,
	global_psi_attack_multiplier: true,
	global_psi_defense_multiplier: true,
	global_naval_movement_bonus: true,
	global_full_repair: true,
	global_police_rating_bonus: true,
	global_extra_police_units: true,
	efficiency_rating_bonus: true,
	defender_morale_minimum: true,
	prototype_cost_waiver: true,
	mineral_to_energy_divisor: true,
	orbital_resource: true,
	orbital_defense: true,
	psi_gate: true,
};

const facility_manifest_fields = {
	id: true,
	name: true,
	kind: true,
	mineral_cost: true,
	energy_maintenance: true,
	required_technology: true,
	required_project: true,
	obsolete_technology: true,
	effect: true,
};

const unit_fields = {
	name: true,
	mineral_cost: true,
	is_native: true,
	offense: true,
	defense: true,
	can_found_base: true,
	can_terraform: true,
	buildable: true,
	required_technology: true,
	chassis: true,
	weapon: true,
	armor: true,
	reactor: true,
	reactor_power: true,
	abilities: true,
	morale: true,
	type: true,
	movement_type: true,
	movement_per_turn: true,
	operational_range: true,
	is_missile: true,
	cargo_capacity: true,
	render: true,
};

const unit_render_fields = {
	type: true,
	file: true,
	x: true,
	y: true,
	w: true,
	h: true,
	cx: true,
	cy: true,
	morale_based_xshift: true,
};

const chassis_fields = {
	id: true,
	name: true,
	speed: true,
	triad: true,
	range: true,
	missile: true,
	cargo: true,
	cost: true,
	availability: true,
	required_technology: true,
};

const reactor_fields = {
	id: true,
	name: true,
	power: true,
	availability: true,
	required_technology: true,
};

const weapon_fields = {
	id: true,
	name: true,
	short_name: true,
	offense: true,
	mode: true,
	cost: true,
	icon: true,
	availability: true,
	required_technology: true,
};

const armor_fields = {
	id: true,
	name: true,
	short_name: true,
	defense: true,
	mode: true,
	cost: true,
	availability: true,
	required_technology: true,
};

const ability_fields = {
	id: true,
	name: true,
	cost: true,
	abbreviation: true,
	flags: true,
	effect: true,
	availability: true,
	required_technology: true,
};

const predefined_unit_fields = {
	id: true,
	name: true,
	chassis: true,
	weapon: true,
	armor: true,
	plan: true,
	mineral_cost: true,
	cargo: true,
	icon: true,
	ability_flags: true,
	availability: true,
	required_technology: true,
};

const faction_fields = {
	starting_technologies: true,
	is_naval: true,
	is_progenitor: true,
	is_native: true,
};

const add_error = (errors, path, message) => {
	errors :+path + ': ' + message;
};

const is_number = (value) => {
	return #typeof(value) == 'Int' || #typeof(value) == 'Float';
};

const validate_string = (object, key, path, errors, required) => {
	if (!#is_defined(object[key])) {
		if (required) {
			add_error(errors, path + '.' + key, 'is required');
		}
		return;
	}
	if (#typeof(object[key]) != 'String' || object[key] == '') {
		add_error(errors, path + '.' + key, 'must be a non-empty string');
	}
};

const validate_optional_string = (object, key, path, errors) => {
	if (#is_defined(object[key]) && #typeof(object[key]) != 'String') {
		add_error(errors, path + '.' + key, 'must be a string');
	}
};

const validate_int = (object, key, path, errors, required, minimum, maximum) => {
	if (!#is_defined(object[key])) {
		if (required) {
			add_error(errors, path + '.' + key, 'is required');
		}
		return;
	}
	if (
		#typeof(object[key]) != 'Int' ||
		object[key] < minimum ||
		object[key] > maximum
	) {
		add_error(
			errors,
			path + '.' + key,
			'must be an integer from ' + #to_string(minimum) + ' through ' + #to_string(maximum)
		);
	}
};

const validate_number = (object, key, path, errors, required, minimum, maximum) => {
	if (!#is_defined(object[key])) {
		if (required) {
			add_error(errors, path + '.' + key, 'is required');
		}
		return;
	}
	if (!is_number(object[key])) {
		add_error(errors, path + '.' + key, 'must be a number');
		return;
	}
	const value = #to_float(object[key]);
	if (value < minimum || value > maximum) {
		add_error(
			errors,
			path + '.' + key,
			'must be a number from ' + #to_string(minimum) + ' through ' + #to_string(maximum)
		);
	}
};

const validate_bool = (object, key, path, errors, required) => {
	if (!#is_defined(object[key])) {
		if (required) {
			add_error(errors, path + '.' + key, 'is required');
		}
		return;
	}
	if (#typeof(object[key]) != 'Bool') {
		add_error(errors, path + '.' + key, 'must be a boolean');
	}
};

const validate_availability = (entry, path, technologies, errors) => {
	validate_string(entry, 'availability', path, errors, true);
	validate_optional_string(entry, 'required_technology', path, errors);
	if (!#is_defined(entry.availability)) {
		return;
	}
	if (
		entry.availability != 'always' &&
		entry.availability != 'technology' &&
		entry.availability != 'disabled'
	) {
		add_error(errors, path + '.availability', 'must be always, technology, or disabled');
		return;
	}
	if (entry.availability == 'technology') {
		if (!#is_defined(entry.required_technology) || entry.required_technology == '') {
			add_error(errors, path + '.required_technology', 'is required for technology availability');
		} else if (!#is_defined(technologies[entry.required_technology])) {
			add_error(
				errors,
				path + '.required_technology',
				'references missing technology ' + entry.required_technology
			);
		}
	} else if (#is_defined(entry.required_technology) && entry.required_technology != '') {
		add_error(errors, path + '.required_technology', 'must be empty unless availability is technology');
	}
};

const validate_known_fields = (object, allowed, path, errors) => {
	for (key in object) {
		if (!#is_defined(allowed[key])) {
			add_error(errors, path + '.' + key, 'is not a supported field');
		}
	}
};

const validate_technologies = (definitions, order, errors) => {
	let count = 0;
	if (#typeof(definitions) != 'Object') {
		add_error(errors, 'technologies.definitions', 'must be an object');
		return count;
	}
	if (#typeof(order) != 'Array') {
		add_error(errors, 'technologies.order', 'must be an array');
		return count;
	}

	let ordered = {};
	let graph_is_valid = true;
	for (let i = 0; i < #sizeof(order); i++) {
		const id = order[i];
		const path = 'technologies.order[' + #to_string(i) + ']';
		if (#typeof(id) != 'String' || id == '') {
			add_error(errors, path, 'must contain a non-empty technology id');
			graph_is_valid = false;
			continue;
		}
		if (#is_defined(ordered[id])) {
			add_error(errors, path, 'duplicates technology ' + id);
			graph_is_valid = false;
			continue;
		}
		ordered[id] = true;
		if (!#is_defined(definitions[id])) {
			add_error(errors, path, 'references missing technology ' + id);
			graph_is_valid = false;
		}
	}

	for (id in definitions) {
		count++;
		const definition = definitions[id];
		const path = 'technologies.' + id;
		if (!#is_defined(ordered[id])) {
			add_error(errors, path, 'is not listed in technologies.order');
			graph_is_valid = false;
		}
		if (#typeof(definition) != 'Object') {
			add_error(errors, path, 'must be an object');
			graph_is_valid = false;
			continue;
		}
		validate_known_fields(definition, technology_fields, path, errors);
		validate_string(definition, 'id', path, errors, true);
		if (#is_defined(definition.id) && definition.id != id) {
			add_error(errors, path + '.id', 'must match catalog key ' + id);
		}
		validate_string(definition, 'name', path, errors, true);
		validate_int(definition, 'cost', path, errors, true, 1, MAX_DEFINITION_VALUE);
		validate_int(definition, 'commerce_bonus', path, errors, true, 0, MAX_DEFINITION_VALUE);
		if (#typeof(definition.prerequisites) != 'Array') {
			add_error(errors, path + '.prerequisites', 'must be an array');
			graph_is_valid = false;
			continue;
		}
		let seen_prerequisites = {};
		for (let i = 0; i < #sizeof(definition.prerequisites); i++) {
			const prerequisite = definition.prerequisites[i];
			const prerequisite_path = path + '.prerequisites[' + #to_string(i) + ']';
			if (#typeof(prerequisite) != 'String' || prerequisite == '') {
				add_error(errors, prerequisite_path, 'must contain a non-empty technology id');
				graph_is_valid = false;
				continue;
			}
			if (#is_defined(seen_prerequisites[prerequisite])) {
				add_error(errors, prerequisite_path, 'duplicates prerequisite ' + prerequisite);
				graph_is_valid = false;
			}
			seen_prerequisites[prerequisite] = true;
			if (prerequisite == id) {
				add_error(errors, prerequisite_path, 'cannot reference itself');
				graph_is_valid = false;
			} else if (!#is_defined(definitions[prerequisite])) {
				add_error(errors, prerequisite_path, 'references missing technology ' + prerequisite);
				graph_is_valid = false;
			}
		}
	}

	if (graph_is_valid) {
		let resolved = {};
		let resolved_count = 0;
		let changed = true;
		while (changed) {
			changed = false;
			for (id of order) {
				if (#is_defined(resolved[id])) {
					continue;
				}
				let ready = true;
				for (prerequisite of definitions[id].prerequisites) {
					if (!#is_defined(resolved[prerequisite])) {
						ready = false;
						break;
					}
				}
				if (ready) {
					resolved[id] = true;
					resolved_count++;
					changed = true;
				}
			}
		}
		if (resolved_count != #sizeof(order)) {
			let blocked = '';
			for (id of order) {
				if (!#is_defined(resolved[id])) {
					blocked = blocked == '' ? id : blocked + ', ' + id;
				}
			}
			add_error(errors, 'technologies', 'dependency cycle prevents resolution of: ' + blocked);
		}
	}

	return count;
};

const validate_facilities = (facilities, technologies, errors) => {
	let count = 0;
	let project_count = 0;
	if (#typeof(facilities) != 'Array') {
		add_error(errors, 'facilities', 'must be an array');
		return {facility_count: count, project_count: project_count};
	}
	let seen = {};
	for (let i = 0; i < #sizeof(facilities); i++) {
		const entry = facilities[i];
		const index_path = 'facilities[' + #to_string(i) + ']';
		if (#typeof(entry) != 'Object') {
			add_error(errors, index_path, 'must be an object');
			continue;
		}
		if (#typeof(entry.id) != 'String' || entry.id == '') {
			add_error(errors, index_path + '.id', 'must be a non-empty string');
			continue;
		}
		const path = 'facilities.' + entry.id;
		if (#is_defined(seen[entry.id])) {
			add_error(errors, path, 'duplicates facility id ' + entry.id);
			continue;
		}
		seen[entry.id] = true;
		if (#typeof(entry.data) != 'Object') {
			add_error(errors, path + '.data', 'must be an object');
			continue;
		}
		const data = entry.data;
		if (#is_defined(data.is_project) && data.is_project) {
			project_count++;
		} else {
			count++;
		}
		validate_known_fields(data, facility_fields, path, errors);
		validate_bool(data, 'is_project', path, errors, false);
		validate_optional_string(data, 'granted_facility', path, errors);
		validate_int(data, 'global_talent_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'global_growth_rating_bonus', path, errors, false, 0, 10);
		validate_int(data, 'global_population_limit_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'global_mineral_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'global_support_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_number(data, 'global_maintenance_multiplier', path, errors, false, 0.0, 1.0);
		validate_int(data, 'global_native_lifecycle_bonus', path, errors, false, 0, 10);
		validate_int(
			data,
			'network_node_drone_modifier',
			path,
			errors,
			false,
			0 - MAX_DEFINITION_VALUE,
			MAX_DEFINITION_VALUE
		);
		validate_int(data, 'network_node_research_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'worked_tile_energy_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'forest_nutrient_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'forest_mineral_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'forest_energy_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_bool(data, 'full_repair_land', path, errors, false);
		validate_bool(data, 'full_repair_water', path, errors, false);
		validate_bool(data, 'full_repair_air', path, errors, false);
		validate_bool(data, 'full_repair_native', path, errors, false);
		validate_int(data, 'defender_morale_bonus', path, errors, false, 0, 10);
		validate_bool(data, 'global_prevent_riots', path, errors, false);
		validate_number(data, 'global_terraforming_rate_multiplier', path, errors, false, 1.0, 10.0);
		validate_int(data, 'new_base_population', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_int(
			data,
			'small_base_drone_modifier',
			path,
			errors,
			false,
			0 - MAX_DEFINITION_VALUE,
			MAX_DEFINITION_VALUE
		);
		validate_number(data, 'global_psi_attack_multiplier', path, errors, false, 1.0, 10.0);
		validate_number(data, 'global_psi_defense_multiplier', path, errors, false, 1.0, 10.0);
		validate_number(data, 'global_naval_movement_bonus', path, errors, false, 0.0, 10.0);
		validate_bool(data, 'global_full_repair', path, errors, false);
		validate_int(data, 'global_police_rating_bonus', path, errors, false, 0, 10);
		validate_int(data, 'global_extra_police_units', path, errors, false, 0, 10);
		validate_int(data, 'efficiency_rating_bonus', path, errors, false, 0, 10);
		validate_int(data, 'defender_morale_minimum', path, errors, false, 0, 10);
		validate_int(
			data,
			'mineral_to_energy_divisor',
			path,
			errors,
			false,
			1,
			MAX_DEFINITION_VALUE
		);
		validate_optional_string(data, 'orbital_resource', path, errors);
		validate_bool(data, 'orbital_defense', path, errors, false);
		validate_string(data, 'name', path, errors, true);
		const is_mineral_conversion =
			#is_defined(data.mineral_to_energy_divisor) &&
			data.mineral_to_energy_divisor > 0;
		validate_int(
			data,
			'mineral_cost',
			path,
			errors,
			true,
			is_mineral_conversion ? 0 : 1,
			MAX_DEFINITION_VALUE
		);
		if (is_mineral_conversion && (data.mineral_cost != 0 || data.is_project)) {
			add_error(
				errors,
				path + '.mineral_to_energy_divisor',
				'requires a zero-cost non-project definition'
			);
		}
		const orbital_resource = #is_defined(data.orbital_resource)
			? data.orbital_resource
			: '';
		const is_orbital_defense = #is_defined(data.orbital_defense) &&
			data.orbital_defense;
		const is_orbital = orbital_resource != '' || is_orbital_defense;
		if (
			orbital_resource != '' && orbital_resource != 'NUTRIENTS' &&
			orbital_resource != 'MINERALS' && orbital_resource != 'ENERGY'
		) {
			add_error(
				errors,
				path + '.orbital_resource',
				'must be NUTRIENTS, MINERALS, or ENERGY'
			);
		}
		if (orbital_resource != '' && is_orbital_defense) {
			add_error(errors, path, 'cannot be both an orbital resource and defense facility');
		}
		if (is_orbital && data.is_project) {
			add_error(errors, path, 'orbital facilities cannot be Secret Projects');
		}
		validate_int(data, 'nutrient_bonus', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'mineral_bonus', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'energy_bonus', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'energy_maintenance', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'psych_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_number(data, 'research_multiplier', path, errors, false, 0.0 - 1.0, 10.0);
		validate_number(data, 'defense_multiplier', path, errors, false, 1.0, 10.0);
		validate_number(data, 'economy_multiplier', path, errors, false, 0.0, 10.0);
		validate_int(data, 'unit_morale_bonus', path, errors, false, 0, 10);
		validate_int(data, 'research_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_number(data, 'mineral_multiplier', path, errors, false, 0.0, 10.0);
		validate_number(data, 'psych_multiplier', path, errors, false, 0.0, 10.0);
		validate_int(data, 'population_limit', path, errors, false, 1, MAX_DEFINITION_VALUE);
		validate_optional_string(data, 'required_facility', path, errors);
		validate_optional_string(data, 'required_project', path, errors);
		validate_int(
			data,
			'drone_modifier',
			path,
			errors,
			false,
			0 - MAX_DEFINITION_VALUE,
			MAX_DEFINITION_VALUE
		);
		validate_int(data, 'talent_bonus', path, errors, false, 0, MAX_DEFINITION_VALUE);
		validate_bool(data, 'suppress_psych', path, errors, false);
		validate_int(data, 'unit_morale_land_bonus', path, errors, false, 0, 10);
		validate_int(data, 'unit_morale_water_bonus', path, errors, false, 0, 10);
		validate_int(data, 'unit_morale_air_bonus', path, errors, false, 0, 10);
		validate_number(data, 'water_defense_multiplier', path, errors, false, 1.0, 10.0);
		validate_number(data, 'air_defense_multiplier', path, errors, false, 1.0, 10.0);
		validate_int(data, 'growth_rating_bonus', path, errors, false, 0, 10);
		validate_int(data, 'native_lifecycle_bonus', path, errors, false, 0, 10);
		validate_bool(data, 'prototype_cost_waiver', path, errors, false);
		validate_bool(data, 'psi_gate', path, errors, false);
		validate_optional_string(data, 'required_technology', path, errors);
		if (
			#is_defined(data.required_technology) &&
			data.required_technology != '' &&
			!#is_defined(technologies[data.required_technology])
		) {
			add_error(
				errors,
				path + '.required_technology',
				'references missing technology ' + data.required_technology
			);
		}
		const has_effect =
			data.nutrient_bonus > 0 ||
			data.mineral_bonus > 0 ||
			data.energy_bonus > 0 ||
			(#is_defined(data.psych_bonus) && data.psych_bonus > 0) ||
			(#is_defined(data.research_multiplier) && data.research_multiplier != 0.0) ||
			(#is_defined(data.defense_multiplier) && data.defense_multiplier > 1.0) ||
			(#is_defined(data.economy_multiplier) && data.economy_multiplier > 0.0) ||
			(#is_defined(data.unit_morale_bonus) && data.unit_morale_bonus > 0) ||
			(#is_defined(data.research_bonus) && data.research_bonus > 0) ||
			(#is_defined(data.mineral_multiplier) && data.mineral_multiplier > 0.0) ||
			(#is_defined(data.psych_multiplier) && data.psych_multiplier > 0.0) ||
			(#is_defined(data.population_limit) && data.population_limit > 0) ||
			(#is_defined(data.drone_modifier) && data.drone_modifier != 0) ||
			(#is_defined(data.talent_bonus) && data.talent_bonus > 0) ||
			(#is_defined(data.suppress_psych) && data.suppress_psych) ||
			(#is_defined(data.unit_morale_land_bonus) && data.unit_morale_land_bonus > 0) ||
			(#is_defined(data.unit_morale_water_bonus) && data.unit_morale_water_bonus > 0) ||
			(#is_defined(data.unit_morale_air_bonus) && data.unit_morale_air_bonus > 0) ||
			(#is_defined(data.water_defense_multiplier) && data.water_defense_multiplier > 1.0) ||
			(#is_defined(data.air_defense_multiplier) && data.air_defense_multiplier > 1.0) ||
			(#is_defined(data.growth_rating_bonus) && data.growth_rating_bonus > 0) ||
			(#is_defined(data.native_lifecycle_bonus) && data.native_lifecycle_bonus > 0) ||
			(#is_defined(data.prototype_cost_waiver) && data.prototype_cost_waiver) ||
			(#is_defined(data.psi_gate) && data.psi_gate) ||
			is_mineral_conversion ||
			is_orbital ||
			(#is_defined(data.granted_facility) && data.granted_facility != '') ||
			(#is_defined(data.global_talent_bonus) && data.global_talent_bonus > 0) ||
			(#is_defined(data.global_growth_rating_bonus) && data.global_growth_rating_bonus > 0) ||
			(#is_defined(data.global_population_limit_bonus) && data.global_population_limit_bonus > 0) ||
			(#is_defined(data.global_mineral_bonus) && data.global_mineral_bonus > 0) ||
			(#is_defined(data.global_support_bonus) && data.global_support_bonus > 0) ||
			(
				#is_defined(data.global_maintenance_multiplier) &&
				data.global_maintenance_multiplier < 1.0
			) ||
			(
				#is_defined(data.global_native_lifecycle_bonus) &&
				data.global_native_lifecycle_bonus > 0
			) ||
			(
				#is_defined(data.network_node_drone_modifier) &&
				data.network_node_drone_modifier != 0
			) ||
			(
				#is_defined(data.network_node_research_bonus) &&
				data.network_node_research_bonus > 0
			) ||
			(#is_defined(data.worked_tile_energy_bonus) && data.worked_tile_energy_bonus > 0) ||
			(#is_defined(data.forest_nutrient_bonus) && data.forest_nutrient_bonus > 0) ||
			(#is_defined(data.forest_mineral_bonus) && data.forest_mineral_bonus > 0) ||
			(#is_defined(data.forest_energy_bonus) && data.forest_energy_bonus > 0) ||
			(#is_defined(data.full_repair_land) && data.full_repair_land) ||
			(#is_defined(data.full_repair_water) && data.full_repair_water) ||
			(#is_defined(data.full_repair_air) && data.full_repair_air) ||
			(#is_defined(data.full_repair_native) && data.full_repair_native) ||
			(#is_defined(data.defender_morale_bonus) && data.defender_morale_bonus > 0) ||
			(#is_defined(data.efficiency_rating_bonus) && data.efficiency_rating_bonus > 0) ||
			(#is_defined(data.defender_morale_minimum) && data.defender_morale_minimum > 0) ||
			(#is_defined(data.global_prevent_riots) && data.global_prevent_riots) ||
			(
				#is_defined(data.global_terraforming_rate_multiplier) &&
				data.global_terraforming_rate_multiplier > 1.0
			) ||
			(#is_defined(data.new_base_population) && data.new_base_population > 0) ||
			(
				#is_defined(data.small_base_drone_modifier) &&
				data.small_base_drone_modifier != 0
			) ||
			(
				#is_defined(data.global_psi_attack_multiplier) &&
				data.global_psi_attack_multiplier > 1.0
			) ||
			(
				#is_defined(data.global_psi_defense_multiplier) &&
				data.global_psi_defense_multiplier > 1.0
			) ||
			(
				#is_defined(data.global_naval_movement_bonus) &&
				data.global_naval_movement_bonus > 0.0
			) ||
			(#is_defined(data.global_full_repair) && data.global_full_repair) ||
			(
				#is_defined(data.global_police_rating_bonus) &&
				data.global_police_rating_bonus > 0
			) ||
			(
				#is_defined(data.global_extra_police_units) &&
				data.global_extra_police_units > 0
			);
		if (!has_effect && !(#is_defined(data.is_project) && data.is_project)) {
			add_error(errors, path, 'has no implemented gameplay effect');
		}
	}
	let project_ids = {};
	for (entry of facilities) {
		if (
			#typeof(entry) == 'Object' && #typeof(entry.id) == 'String' &&
			#typeof(entry.data) == 'Object' &&
			#is_defined(entry.data.is_project) && entry.data.is_project
		) {
			project_ids[entry.id] = true;
		}
	}
	for (entry of facilities) {
		if (
			#typeof(entry) != 'Object' || #typeof(entry.id) != 'String' ||
			#typeof(entry.data) != 'Object' ||
			!#is_defined(entry.data.required_facility) || entry.data.required_facility == ''
		) {
			continue;
		}
		if (entry.data.required_facility == entry.id) {
			add_error(
				errors,
				'facilities.' + entry.id + '.required_facility',
				'cannot reference itself'
			);
		} else if (!#is_defined(seen[entry.data.required_facility])) {
			add_error(
				errors,
				'facilities.' + entry.id + '.required_facility',
				'references missing facility ' + entry.data.required_facility
			);
		}
	}
	for (entry of facilities) {
		if (
			#typeof(entry) != 'Object' || #typeof(entry.id) != 'String' ||
			#typeof(entry.data) != 'Object' ||
			!#is_defined(entry.data.required_project) || entry.data.required_project == ''
		) {
			continue;
		}
		if (entry.data.required_project == entry.id) {
			add_error(
				errors,
				'facilities.' + entry.id + '.required_project',
				'cannot reference itself'
			);
		} else if (!#is_defined(seen[entry.data.required_project])) {
			add_error(
				errors,
				'facilities.' + entry.id + '.required_project',
				'references missing project ' + entry.data.required_project
			);
		} else if (!#is_defined(project_ids[entry.data.required_project])) {
			add_error(
				errors,
				'facilities.' + entry.id + '.required_project',
				'must reference a project'
			);
		}
	}
	for (entry of facilities) {
		if (
			#typeof(entry) != 'Object' || #typeof(entry.data) != 'Object' ||
			!#is_defined(entry.data.granted_facility) || entry.data.granted_facility == ''
		) {
			continue;
		}
		if (!#is_defined(seen[entry.data.granted_facility])) {
			add_error(
				errors,
				'facilities.' + entry.id + '.granted_facility',
				'references missing facility ' + entry.data.granted_facility
			);
		}
	}
	return {facility_count: count, project_count: project_count};
};

const validate_facility_manifest = (manifest, technologies, errors) => {
	let facility_count = 0;
	let project_count = 0;
	let definition_count = 0;
	let definitions = {};
	if (#typeof(manifest) != 'Array') {
		add_error(errors, 'facility_manifest', 'must be an array');
		return {
			facility_count: facility_count,
			project_count: project_count,
			definitions: definitions,
		};
	}
	for (let i = 0; i < #sizeof(manifest); i++) {
		const entry = manifest[i];
		const index_path = 'facility_manifest[' + #to_string(i) + ']';
		if (#typeof(entry) != 'Object' || #typeof(entry.id) != 'String' || entry.id == '') {
			add_error(errors, index_path, 'must have a non-empty id');
			continue;
		}
		const path = 'facility_manifest.' + entry.id;
		if (#is_defined(definitions[entry.id])) {
			add_error(errors, path, 'duplicates catalog id ' + entry.id);
			continue;
		}
		definitions[entry.id] = entry;
		definition_count++;
		validate_known_fields(entry, facility_manifest_fields, path, errors);
		validate_string(entry, 'name', path, errors, true);
		validate_string(entry, 'kind', path, errors, true);
		if (#is_defined(entry.kind) && entry.kind == 'facility') {
			facility_count++;
		} else if (#is_defined(entry.kind) && entry.kind == 'project') {
			project_count++;
		} else if (#is_defined(entry.kind)) {
			add_error(errors, path + '.kind', 'must be facility or project');
		}
		validate_int(entry, 'mineral_cost', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_int(entry, 'energy_maintenance', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_optional_string(entry, 'required_technology', path, errors);
		validate_optional_string(entry, 'required_project', path, errors);
		validate_optional_string(entry, 'obsolete_technology', path, errors);
		validate_string(entry, 'effect', path, errors, true);
		for (technology_field of ['required_technology', 'obsolete_technology']) {
			if (
				#is_defined(entry[technology_field]) &&
				entry[technology_field] != '' &&
				!#is_defined(technologies[entry[technology_field]])
			) {
				add_error(
					errors,
					path + '.' + technology_field,
					'references missing technology ' + entry[technology_field]
				);
			}
		}
	}
	let project_graph_is_valid = true;
	for (entry of manifest) {
		if (
			#typeof(entry) != 'Object' || #typeof(entry.id) != 'String' ||
			!#is_defined(entry.required_project) || entry.required_project == ''
		) {
			continue;
		}
		const path = 'facility_manifest.' + entry.id + '.required_project';
		if (entry.required_project == entry.id) {
			add_error(errors, path, 'cannot reference itself');
			project_graph_is_valid = false;
		} else if (!#is_defined(definitions[entry.required_project])) {
			add_error(errors, path, 'references missing project ' + entry.required_project);
			project_graph_is_valid = false;
		} else if (definitions[entry.required_project].kind != 'project') {
			add_error(errors, path, 'must reference a project');
			project_graph_is_valid = false;
		}
	}
	if (project_graph_is_valid) {
		let resolved = {};
		let resolved_count = 0;
		let changed = true;
		while (changed) {
			changed = false;
			for (entry of manifest) {
				if (#typeof(entry) != 'Object' || #typeof(entry.id) != 'String') {
					continue;
				}
				if (
					#is_defined(resolved[entry.id]) ||
					(
						#is_defined(entry.required_project) && entry.required_project != '' &&
						!#is_defined(resolved[entry.required_project])
					)
				) {
					continue;
				}
				resolved[entry.id] = true;
				resolved_count++;
				changed = true;
			}
		}
		if (resolved_count != definition_count) {
			let blocked = '';
			for (entry of manifest) {
				if (
					#typeof(entry) == 'Object' && #typeof(entry.id) == 'String' &&
					!#is_defined(resolved[entry.id])
				) {
					blocked = blocked == '' ? entry.id : blocked + ', ' + entry.id;
				}
			}
			add_error(
				errors,
				'facility_manifest',
				'project dependency cycle prevents resolution of: ' + blocked
			);
		}
	}
	return {
		facility_count: facility_count,
		project_count: project_count,
		definitions: definitions,
	};
};

const validate_facility_implementations = (facilities, manifest, errors) => {
	if (#typeof(facilities) != 'Array') {
		return;
	}
	for (entry of facilities) {
		if (#typeof(entry) != 'Object' || #typeof(entry.id) != 'String') {
			continue;
		}
		const path = 'facilities.' + entry.id;
		if (!#is_defined(manifest[entry.id])) {
			add_error(errors, path, 'is not present in the base-game facility manifest');
			continue;
		}
		if (#typeof(entry.data) != 'Object') {
			continue;
		}
		const data = entry.data;
		const is_project = #is_defined(data.is_project) && data.is_project;
		if ((manifest[entry.id].kind == 'project') != is_project) {
			add_error(
				errors,
				path + '.is_project',
				'must match base-game manifest kind ' + manifest[entry.id].kind
			);
		}
		for (field of ['name', 'mineral_cost', 'energy_maintenance', 'required_technology']) {
			if (data[field] != manifest[entry.id][field]) {
				add_error(
					errors,
					path + '.' + field,
					'does not match base-game manifest value ' + #to_string(manifest[entry.id][field])
				);
			}
		}
		const manifest_required_project = #is_defined(manifest[entry.id].required_project)
			? manifest[entry.id].required_project
			: '';
		if (data.required_project != manifest_required_project) {
			add_error(
				errors,
				path + '.required_project',
				'does not match base-game manifest value ' + manifest_required_project
			);
		}
	}
};

const validate_facility_coverage = (coverage, facilities, errors, projects) => {
	const coverage_path = projects ? 'project_coverage' : 'facility_coverage';
	let result = {complete: 0, partial: 0};
	if (#typeof(coverage) != 'Object') {
		add_error(errors, coverage_path, 'must be an object');
		return result;
	}
	validate_int(coverage, 'complete', coverage_path, errors, true, 0, MAX_DEFINITION_VALUE);
	validate_int(coverage, 'partial', coverage_path, errors, true, 0, MAX_DEFINITION_VALUE);
	if (#typeof(coverage.status) != 'Object') {
		add_error(errors, coverage_path + '.status', 'must be an object');
		return result;
	}
	let implemented = {};
	if (#typeof(facilities) == 'Array') {
		for (entry of facilities) {
			if (#typeof(entry) == 'Object' && #typeof(entry.id) == 'String') {
				const is_project = #typeof(entry.data) == 'Object' &&
					#is_defined(entry.data.is_project) && entry.data.is_project;
				if (is_project == projects) {
					implemented[entry.id] = true;
				}
			}
		}
	}
	for (id in coverage.status) {
		const path = coverage_path + '.' + id;
		if (!#is_defined(implemented[id])) {
			add_error(errors, path, 'references a facility that is not implemented');
		}
		const status = coverage.status[id];
		if (status == 'complete') {
			result.complete = result.complete + 1;
		} else if (status == 'partial') {
			result.partial = result.partial + 1;
		} else {
			add_error(errors, path, 'must be complete or partial');
		}
	}
	for (id in implemented) {
		if (!#is_defined(coverage.status[id])) {
			add_error(errors, coverage_path + '.' + id, 'is missing an implementation status');
		}
	}
	if (#typeof(coverage.complete) == 'Int' && coverage.complete != result.complete) {
		add_error(
			errors,
			coverage_path + '.complete',
			'reports ' + #to_string(coverage.complete) +
				' but contains ' + #to_string(result.complete)
		);
	}
	if (#typeof(coverage.partial) == 'Int' && coverage.partial != result.partial) {
		add_error(
			errors,
			coverage_path + '.partial',
			'reports ' + #to_string(coverage.partial) +
				' but contains ' + #to_string(result.partial)
		);
	}
	return result;
};

const validate_moralesets = (moralesets, errors) => {
	let count = 0;
	let ids = {};
	if (#typeof(moralesets) != 'Array') {
		add_error(errors, 'moralesets', 'must be an array');
		return {count: count, ids: ids};
	}
	for (let i = 0; i < #sizeof(moralesets); i++) {
		const entry = moralesets[i];
		const index_path = 'moralesets[' + #to_string(i) + ']';
		if (#typeof(entry) != 'Object' || #typeof(entry.id) != 'String' || entry.id == '') {
			add_error(errors, index_path, 'must have a non-empty id');
			continue;
		}
		const path = 'moralesets.' + entry.id;
		if (#is_defined(ids[entry.id])) {
			add_error(errors, path, 'duplicates morale set id ' + entry.id);
			continue;
		}
		ids[entry.id] = true;
		count++;
		if (#typeof(entry.data) != 'Array' || #sizeof(entry.data) != 7) {
			add_error(errors, path + '.data', 'must contain exactly seven morale levels');
			continue;
		}
		for (let level = 0; level < #sizeof(entry.data); level++) {
			const level_path = path + '.data[' + #to_string(level) + ']';
			if (#typeof(entry.data[level]) != 'Object') {
				add_error(errors, level_path, 'must be an object');
				continue;
			}
			validate_string(entry.data[level], 'name', level_path, errors, true);
		}
	}
	return {count: count, ids: ids};
};

const validate_unit_render = (render, path, errors) => {
	if (#typeof(render) != 'Object') {
		add_error(errors, path, 'must be an object');
		return;
	}
	validate_known_fields(render, unit_render_fields, path, errors);
	validate_string(render, 'type', path, errors, true);
	if (#is_defined(render.type) && render.type != 'sprite') {
		add_error(errors, path + '.type', 'must be sprite');
	}
	validate_string(render, 'file', path, errors, true);
	for (field of ['x', 'y', 'w', 'h', 'cx', 'cy']) {
		validate_int(render, field, path, errors, true, 0, MAX_DEFINITION_VALUE);
	}
	validate_int(render, 'morale_based_xshift', path, errors, false, 0, MAX_DEFINITION_VALUE);
};

const validate_units = (units, technologies, morale_ids, unit_manifest, errors) => {
	let count = 0;
	if (#typeof(units) != 'Array') {
		add_error(errors, 'units', 'must be an array');
		return count;
	}
	let seen = {};
	const movement_types = {immovable: true, land: true, water: true, air: true};
	for (let i = 0; i < #sizeof(units); i++) {
		const entry = units[i];
		const index_path = 'units[' + #to_string(i) + ']';
		if (#typeof(entry) != 'Object' || #typeof(entry.id) != 'String' || entry.id == '') {
			add_error(errors, index_path, 'must have a non-empty id');
			continue;
		}
		const path = 'units.' + entry.id;
		if (#is_defined(seen[entry.id])) {
			add_error(errors, path, 'duplicates unit id ' + entry.id);
			continue;
		}
		seen[entry.id] = true;
		count++;
		if (#typeof(entry.data) != 'Object') {
			add_error(errors, path + '.data', 'must be an object');
			continue;
		}
		const data = entry.data;
		validate_known_fields(data, unit_fields, path, errors);
		validate_string(data, 'name', path, errors, true);
		validate_int(data, 'mineral_cost', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_bool(data, 'is_native', path, errors, true);
		validate_int(data, 'offense', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_int(data, 'defense', path, errors, true, 0, MAX_DEFINITION_VALUE);
		validate_bool(data, 'can_found_base', path, errors, false);
		validate_bool(data, 'can_terraform', path, errors, false);
		validate_bool(data, 'buildable', path, errors, false);
		for (component of ['chassis', 'weapon', 'armor', 'reactor']) {
			validate_string(data, component, path, errors, true);
		}
		if (#is_defined(data.chassis) && !#is_defined(unit_manifest.chassis.definitions[data.chassis])) {
			add_error(errors, path + '.chassis', 'references missing chassis ' + data.chassis);
		}
		if (#is_defined(data.weapon) && !#is_defined(unit_manifest.weapons.definitions[data.weapon])) {
			add_error(errors, path + '.weapon', 'references missing weapon ' + data.weapon);
		}
		if (#is_defined(data.armor) && !#is_defined(unit_manifest.armors.definitions[data.armor])) {
			add_error(errors, path + '.armor', 'references missing armor ' + data.armor);
		}
		if (#is_defined(data.reactor) && !#is_defined(unit_manifest.reactors.definitions[data.reactor])) {
			add_error(errors, path + '.reactor', 'references missing reactor ' + data.reactor);
		}
		validate_int(data, 'reactor_power', path, errors, true, 1, 4);
		if (#typeof(data.abilities) != 'Array') {
			add_error(errors, path + '.abilities', 'must be an array');
		} else {
			let seen_abilities = {};
			for (let ability_index = 0; ability_index < #sizeof(data.abilities); ability_index++) {
				const ability = data.abilities[ability_index];
				const ability_path = path + '.abilities[' + #to_string(ability_index) + ']';
				if (#typeof(ability) != 'String' || ability == '') {
					add_error(errors, ability_path, 'must be a non-empty string');
				} else if (#is_defined(seen_abilities[ability])) {
					add_error(errors, ability_path, 'duplicates unit ability ' + ability);
				} else if (!#is_defined(unit_manifest.abilities.definitions[ability])) {
					add_error(errors, ability_path, 'references missing ability ' + ability);
				}
				seen_abilities[ability] = true;
			}
		}
		validate_optional_string(data, 'required_technology', path, errors);
		if (
			#is_defined(data.required_technology) &&
			data.required_technology != '' &&
			!#is_defined(technologies[data.required_technology])
		) {
			add_error(
				errors,
				path + '.required_technology',
				'references missing technology ' + data.required_technology
			);
		}
		validate_string(data, 'morale', path, errors, true);
		if (#is_defined(data.morale) && !#is_defined(morale_ids[data.morale])) {
			add_error(errors, path + '.morale', 'references missing morale set ' + data.morale);
		}
		validate_string(data, 'type', path, errors, true);
		if (#is_defined(data.type) && data.type != 'static') {
			add_error(errors, path + '.type', 'must be static');
		}
		validate_string(data, 'movement_type', path, errors, true);
		if (#is_defined(data.movement_type) && !#is_defined(movement_types[data.movement_type])) {
			add_error(errors, path + '.movement_type', 'is not supported');
		}
		validate_int(data, 'movement_per_turn', path, errors, true, 0, 1000);
		validate_int(data, 'operational_range', path, errors, true, 0, 1000);
		validate_bool(data, 'is_missile', path, errors, true);
		validate_int(data, 'cargo_capacity', path, errors, true, 0, 100);
		if (
			#is_defined(data.movement_type) && data.movement_type != 'air' &&
			(
				(#is_defined(data.operational_range) && data.operational_range > 0) ||
				(#is_defined(data.is_missile) && data.is_missile)
			)
		) {
			add_error(errors, path + '.operational_range', 'is only supported for air units');
		}
		if (
			#is_defined(data.is_missile) && data.is_missile &&
			#is_defined(data.operational_range) && data.operational_range == 0
		) {
			add_error(errors, path + '.operational_range', 'must be positive for missiles');
		}
		if (#is_defined(data.chassis) && #is_defined(unit_manifest.chassis.definitions[data.chassis])) {
			const chassis = unit_manifest.chassis.definitions[data.chassis];
			if (#is_defined(data.operational_range) && data.operational_range != chassis.range) {
				add_error(
					errors,
					path + '.operational_range',
					'does not match chassis range ' + #to_string(chassis.range)
				);
			}
			if (#is_defined(data.is_missile) && data.is_missile != chassis.missile) {
				add_error(errors, path + '.is_missile', 'does not match chassis missile flag');
			}
			if (
				#is_defined(data.cargo_capacity) && data.weapon == 'TroopTransport' &&
				data.cargo_capacity != chassis.cargo * data.reactor_power
			) {
				add_error(
					errors,
					path + '.cargo_capacity',
					'does not match chassis cargo multiplied by reactor power'
				);
			}
			if (
				#is_defined(data.cargo_capacity) && data.cargo_capacity > 0 &&
				data.weapon != 'TroopTransport' && !data.is_native
			) {
				add_error(errors, path + '.cargo_capacity', 'requires a transport weapon');
			}
		}
		validate_unit_render(data.render, path + '.render', errors);
	}
	return count;
};

const validate_unit_manifest_entries = (
	entries,
	collection,
	fields,
	technologies,
	errors,
	validate_entry
) => {
	let count = 0;
	let definitions = {};
	const collection_path = 'unit_manifest.' + collection;
	if (#typeof(entries) != 'Array') {
		add_error(errors, collection_path, 'must be an array');
		return {count: count, definitions: definitions};
	}
	for (let i = 0; i < #sizeof(entries); i++) {
		const entry = entries[i];
		const index_path = collection_path + '[' + #to_string(i) + ']';
		if (#typeof(entry) != 'Object' || #typeof(entry.id) != 'String' || entry.id == '') {
			add_error(errors, index_path, 'must have a non-empty id');
			continue;
		}
		const path = collection_path + '.' + entry.id;
		if (#is_defined(definitions[entry.id])) {
			add_error(errors, path, 'duplicates catalog id ' + entry.id);
			continue;
		}
		definitions[entry.id] = entry;
		count++;
		validate_known_fields(entry, fields, path, errors);
		validate_string(entry, 'name', path, errors, true);
		validate_availability(entry, path, technologies, errors);
		validate_entry(entry, path);
	}
	return {count: count, definitions: definitions};
};

const validate_unit_manifest = (manifest, technologies, errors) => {
	if (#typeof(manifest) != 'Object') {
		add_error(errors, 'unit_manifest', 'must be an object');
		return {
			chassis: {count: 0, definitions: {}},
			reactors: {count: 0, definitions: {}},
			weapons: {count: 0, definitions: {}},
			armors: {count: 0, definitions: {}},
			abilities: {count: 0, definitions: {}},
			predefined_units: {count: 0, definitions: {}},
		};
	}
	const chassis = validate_unit_manifest_entries(
		manifest.chassis,
		'chassis',
		chassis_fields,
		technologies,
		errors,
		(entry, path) => {
			validate_int(entry, 'speed', path, errors, true, 0, 100);
			validate_string(entry, 'triad', path, errors, true);
			if (
				#is_defined(entry.triad) &&
				entry.triad != 'land' && entry.triad != 'sea' && entry.triad != 'air'
			) {
				add_error(errors, path + '.triad', 'must be land, sea, or air');
			}
			validate_int(entry, 'range', path, errors, true, 0, 100);
			validate_bool(entry, 'missile', path, errors, true);
			validate_int(entry, 'cargo', path, errors, true, 0, 100);
			validate_int(entry, 'cost', path, errors, true, 0, 100);
		}
	);
	const reactors = validate_unit_manifest_entries(
		manifest.reactors,
		'reactors',
		reactor_fields,
		technologies,
		errors,
		(entry, path) => {
			validate_int(entry, 'power', path, errors, true, 1, 100);
		}
	);
	const weapons = validate_unit_manifest_entries(
		manifest.weapons,
		'weapons',
		weapon_fields,
		technologies,
		errors,
		(entry, path) => {
			validate_string(entry, 'short_name', path, errors, true);
			validate_int(entry, 'offense', path, errors, true, 0 - 1, 100);
			validate_int(entry, 'mode', path, errors, true, 0, 12);
			validate_int(entry, 'cost', path, errors, true, 0, 100);
			validate_int(entry, 'icon', path, errors, true, 0 - 1, 100);
		}
	);
	const armors = validate_unit_manifest_entries(
		manifest.armors,
		'armors',
		armor_fields,
		technologies,
		errors,
		(entry, path) => {
			validate_string(entry, 'short_name', path, errors, true);
			validate_int(entry, 'defense', path, errors, true, 0 - 1, 100);
			validate_int(entry, 'mode', path, errors, true, 0, 2);
			validate_int(entry, 'cost', path, errors, true, 0, 100);
		}
	);
	const abilities = validate_unit_manifest_entries(
		manifest.abilities,
		'abilities',
		ability_fields,
		technologies,
		errors,
		(entry, path) => {
			validate_int(entry, 'cost', path, errors, true, 0 - 7, 100);
			validate_optional_string(entry, 'abbreviation', path, errors);
			validate_string(entry, 'flags', path, errors, true);
			validate_string(entry, 'effect', path, errors, true);
		}
	);
	const predefined_units = validate_unit_manifest_entries(
		manifest.predefined_units,
		'predefined_units',
		predefined_unit_fields,
		technologies,
		errors,
		(entry, path) => {
			for (field of ['chassis', 'weapon', 'armor']) {
				validate_string(entry, field, path, errors, true);
			}
			if (#is_defined(entry.chassis) && !#is_defined(chassis.definitions[entry.chassis])) {
				add_error(errors, path + '.chassis', 'references missing chassis ' + entry.chassis);
			}
			if (#is_defined(entry.weapon) && !#is_defined(weapons.definitions[entry.weapon])) {
				add_error(errors, path + '.weapon', 'references missing weapon ' + entry.weapon);
			}
			if (#is_defined(entry.armor) && !#is_defined(armors.definitions[entry.armor])) {
				add_error(errors, path + '.armor', 'references missing armor ' + entry.armor);
			}
			validate_int(entry, 'plan', path, errors, true, 0 - 1, 12);
			validate_int(entry, 'mineral_cost', path, errors, true, 0, MAX_DEFINITION_VALUE);
			validate_int(entry, 'cargo', path, errors, true, 0, 100);
			validate_int(entry, 'icon', path, errors, true, 0 - 1, 100);
			validate_string(entry, 'ability_flags', path, errors, true);
		}
	);
	return {
		chassis: chassis,
		reactors: reactors,
		weapons: weapons,
		armors: armors,
		abilities: abilities,
		predefined_units: predefined_units,
	};
};

const validate_factions = (factions, technologies, errors) => {
	let count = 0;
	if (#typeof(factions) != 'Array') {
		add_error(errors, 'factions', 'must be an array');
		return count;
	}
	let seen = {};
	for (let i = 0; i < #sizeof(factions); i++) {
		const entry = factions[i];
		const index_path = 'factions[' + #to_string(i) + ']';
		if (#typeof(entry) != 'Object' || #typeof(entry.id) != 'String' || entry.id == '') {
			add_error(errors, index_path, 'must have a non-empty id');
			continue;
		}
		const path = 'factions.' + entry.id;
		if (#is_defined(seen[entry.id])) {
			add_error(errors, path, 'duplicates faction id ' + entry.id);
			continue;
		}
		seen[entry.id] = true;
		count++;
		validate_string(entry, 'name', path, errors, true);
		validate_string(entry, 'resource', path, errors, true);
		if (#typeof(entry.data) != 'Object') {
			add_error(errors, path + '.data', 'must be an object');
			continue;
		}
		validate_known_fields(entry.data, faction_fields, path, errors);
		validate_bool(entry.data, 'is_naval', path, errors, false);
		validate_bool(entry.data, 'is_progenitor', path, errors, false);
		validate_bool(entry.data, 'is_native', path, errors, false);
		if (#typeof(entry.data.starting_technologies) != 'Array') {
			add_error(errors, path + '.starting_technologies', 'must be an array');
			continue;
		}
		let starting = {};
		for (let tech_index = 0; tech_index < #sizeof(entry.data.starting_technologies); tech_index++) {
			const technology = entry.data.starting_technologies[tech_index];
			const technology_path = path + '.starting_technologies[' + #to_string(tech_index) + ']';
			if (#typeof(technology) != 'String' || technology == '') {
				add_error(errors, technology_path, 'must contain a non-empty technology id');
				continue;
			}
			if (#is_defined(starting[technology])) {
				add_error(errors, technology_path, 'duplicates starting technology ' + technology);
			}
			starting[technology] = true;
			if (!#is_defined(technologies[technology])) {
				add_error(errors, technology_path, 'references missing technology ' + technology);
			}
		}
	}
	return count;
};

const validate = (catalog) => {
	let errors = [];
	const technology_count = validate_technologies(
		catalog.technologies.definitions,
		catalog.technologies.order,
		errors
	);
	const morale_result = validate_moralesets(catalog.moralesets, errors);
	const facility_manifest_result = validate_facility_manifest(
		catalog.facility_manifest,
		catalog.technologies.definitions,
		errors
	);
	const facility_result = validate_facilities(
		catalog.facilities,
		catalog.technologies.definitions,
		errors
	);
	validate_facility_implementations(
		catalog.facilities,
		facility_manifest_result.definitions,
		errors
	);
	const facility_coverage_result = validate_facility_coverage(
		catalog.facility_coverage,
		catalog.facilities,
		errors,
		false
	);
	const project_coverage_result = validate_facility_coverage(
		catalog.project_coverage,
		catalog.facilities,
		errors,
		true
	);
	const unit_manifest_result = validate_unit_manifest(
		catalog.unit_manifest,
		catalog.technologies.definitions,
		errors
	);
	const unit_count = validate_units(
		catalog.units,
		catalog.technologies.definitions,
		morale_result.ids,
		unit_manifest_result,
		errors
	);
	const faction_count = validate_factions(
		catalog.factions,
		catalog.technologies.definitions,
		errors
	);
	return {
		errors: errors,
		counts: {
			technologies: technology_count,
			facilities: facility_result.facility_count,
			complete_facilities: facility_coverage_result.complete,
			partial_facilities: facility_coverage_result.partial,
			implemented_projects: facility_result.project_count,
			complete_projects: project_coverage_result.complete,
			partial_projects: project_coverage_result.partial,
			base_facilities: facility_manifest_result.facility_count,
			projects: facility_manifest_result.project_count,
			units: unit_count,
			chassis: unit_manifest_result.chassis.count,
			reactors: unit_manifest_result.reactors.count,
			weapons: unit_manifest_result.weapons.count,
			armors: unit_manifest_result.armors.count,
			abilities: unit_manifest_result.abilities.count,
			predefined_units: unit_manifest_result.predefined_units.count,
			moralesets: morale_result.count,
			factions: faction_count,
		},
	};
};

return {
	validate: validate,
};
