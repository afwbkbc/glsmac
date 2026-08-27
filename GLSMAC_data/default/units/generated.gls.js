const manifest = #include('../content/base_units');
const technologies = #include('../content/base_technologies');

const add_technology_closure = (known, technology_id) => {
	if (technology_id == '' || #is_defined(known[technology_id])) {
		return;
	}
	const technology = technologies.definitions[technology_id];
	if (!#is_defined(technology)) {
		throw Error('Unknown unit component technology: ' + technology_id);
	}
	for (prerequisite of technology.prerequisites) {
		add_technology_closure(known, prerequisite);
	}
	known[technology_id] = true;
};

const is_available = (entry, known) => {
	if (entry.availability == 'always') {
		return true;
	}
	return entry.availability == 'technology' &&
		#is_defined(known[entry.required_technology]);
};

const get_best_weapon = (known) => {
	let best = null;
	for (weapon of manifest.weapons) {
		if (
			!is_available(weapon, known) || weapon.offense <= 0 ||
			weapon.id == 'PlanetBuster' || weapon.id == 'ConventionalPayload'
		) {
			continue;
		}
		if (best == null || weapon.offense > best.offense) {
			best = weapon;
		}
	}
	return best;
};

const get_best_armor = (known) => {
	let best = null;
	for (armor of manifest.armors) {
		if (!is_available(armor, known) || armor.defense <= 0) {
			continue;
		}
		if (best == null || armor.defense > best.defense) {
			best = armor;
		}
	}
	return best;
};

const get_best_chassis_by_triad = (known) => {
	let result = {};
	for (chassis of manifest.chassis) {
		if (!is_available(chassis, known) || chassis.missile) {
			continue;
		}
		if (
			!#is_defined(result[chassis.triad]) ||
			chassis.speed > result[chassis.triad].speed
		) {
			result[chassis.triad] = chassis;
		}
	}
	return result;
};

const find_component = (entries, id) => {
	for (entry of entries) {
		if (entry.id == id) {
			return entry;
		}
	}
	throw Error('Missing unit component: ' + id);
};

const hand_weapons = find_component(manifest.weapons, 'HandWeapons');
const no_armor = find_component(manifest.armors, 'NoArmor');
const colony_module = find_component(manifest.weapons, 'ColonyModule');
const terraforming_unit = find_component(manifest.weapons, 'TerraformingUnit');
const troop_transport = find_component(manifest.weapons, 'TroopTransport');
const conventional_payload = find_component(manifest.weapons, 'ConventionalPayload');
const planet_buster = find_component(manifest.weapons, 'PlanetBuster');
const heavy_artillery = find_component(manifest.abilities, 'HeavyArtillery');
const carrier_deck = find_component(manifest.abilities, 'CarrierDeck');
const amphibious_pods = find_component(manifest.abilities, 'AmphibiousPods');
const air_superiority = find_component(manifest.abilities, 'AirSuperiority');

const get_role_abilities = (known, role) => {
	let result = [];
	const add_if_available = (id) => {
		const ability = find_component(manifest.abilities, id);
		if (is_available(ability, known)) {
			result :+ability;
		}
	};
	if (role == 'assault') {
		add_if_available('EmpathSong');
		add_if_available('BlinkDisplacer');
	} else if (role == 'garrison') {
		add_if_available('HypnoticTrance');
		const aaa = find_component(manifest.abilities, 'AAATracking');
		if (is_available(aaa, known)) {
			result :+aaa;
		} else {
			add_if_available('CommJammer');
		}
	}
	return result;
};

const get_render = (chassis, role) => {
	let x = role == 'assault' || role == 'former' ? 206 : 2;
	let y = 156;
	if (chassis.triad == 'land' && chassis.speed > 1) {
		x = 104;
	} else if (chassis.triad == 'sea') {
		x = 2;
		y = 310;
	} else if (chassis.triad == 'air') {
		x = 2;
		y = 541;
	}
	return {
		type: 'sprite',
		file: 'units.pcx',
		x: x,
		y: y,
		w: 100,
		h: 75,
		cx: x + 51,
		cy: y + 51,
	};
};

const make_definition = (technology_id, chassis, weapon, armor, role, abilities) => {
	const role_name = role == 'assault' ? weapon.short_name : armor.short_name;
	let ability_name = '';
	let ability_cost = 0;
	let ability_ids = [];
	let ability_suffix = '';
	for (ability of abilities) {
		if (ability.abbreviation != '') {
			ability_name += ability.abbreviation + ' ';
		}
		ability_cost += ability.cost > 0 ? ability.cost : 1;
		ability_ids :+ability.id;
		ability_suffix += ability.id;
	}
	let name = ability_name + role_name + ' ' + chassis.name;
	if (role == 'artillery') {
		name = weapon.short_name + ' Artillery ' + chassis.name;
	} else if (role == 'former') {
		name = ability_name + (chassis.id == 'Infantry' ? 'Former' : chassis.name + ' Former');
	} else if (role == 'transport') {
		name = chassis.name + ' Transport';
	} else if (role == 'colony') {
		name = chassis.id == 'Infantry' ? 'Colony Pod' : chassis.name + ' Colony Pod';
	}
	const reactor_power = 1;
	const cargo_capacity = weapon.id == 'TroopTransport'
		? chassis.cargo * reactor_power
		: 0;
	return {
		id: 'Generated' + chassis.id + weapon.id + armor.id + ability_suffix,
		data: {
			name: name,
			mineral_cost: #max((chassis.cost + weapon.cost + armor.cost + ability_cost) * 5, 10),
			is_native: false,
			offense: weapon.offense,
			defense: armor.defense,
			can_found_base: role == 'colony',
			can_terraform: role == 'former',
			required_technology: technology_id,
			chassis: chassis.id,
			weapon: weapon.id,
			armor: armor.id,
			reactor: 'FissionPlant',
			reactor_power: reactor_power,
			abilities: ability_ids,
			morale: 'STANDARD',
			type: 'static',
			movement_type: chassis.triad == 'sea' ? 'water' : chassis.triad,
			movement_per_turn: chassis.speed,
			operational_range: chassis.range,
			is_missile: chassis.missile,
			cargo_capacity: cargo_capacity,
			render: get_render(chassis, role),
		},
	};
};

let definitions = [];
let seen = {};
seen['Infantry|HandWeapons|NoArmor|'] = true;
seen['Speeder|HandWeapons|NoArmor|'] = true;
seen['Infantry|Laser|NoArmor|'] = true;
seen['Infantry|HandWeapons|SynthmetalArmor|'] = true;
seen['Infantry|ColonyModule|NoArmor|'] = true;
seen['Infantry|TerraformingUnit|NoArmor|'] = true;

const add_design = (technology_id, chassis, weapon, armor, role, abilities) => {
	let ability_signature = '';
	for (ability of abilities) {
		ability_signature += ability.id + ',';
	}
	const signature = chassis.id + '|' + weapon.id + '|' + armor.id + '|' + ability_signature;
	if (#is_defined(seen[signature])) {
		return;
	}
	seen[signature] = true;
	definitions :+make_definition(technology_id, chassis, weapon, armor, role, abilities);
};

const add_milestone_designs = (technology_id) => {
	let known = {};
	add_technology_closure(known, technology_id);
	const weapon = get_best_weapon(known);
	const armor = get_best_armor(known);
	const chassis_by_triad = get_best_chassis_by_triad(known);
	if (weapon == null || armor == null) {
		return;
	}
	for (triad in chassis_by_triad) {
		const chassis = chassis_by_triad[triad];
		add_design(technology_id, chassis, weapon, armor, 'assault', []);
		add_design(technology_id, chassis, hand_weapons, armor, 'garrison', []);
		const assault_abilities = get_role_abilities(known, 'assault');
		if (#sizeof(assault_abilities) > 0) {
			add_design(technology_id, chassis, weapon, armor, 'assault', assault_abilities);
		}
		const garrison_abilities = get_role_abilities(known, 'garrison');
		if (#sizeof(garrison_abilities) > 0) {
			add_design(technology_id, chassis, hand_weapons, armor, 'garrison', garrison_abilities);
		}
		if (triad != 'air' && is_available(heavy_artillery, known)) {
			add_design(technology_id, chassis, weapon, armor, 'artillery', [heavy_artillery]);
		}
		if (triad == 'land' && is_available(amphibious_pods, known)) {
			add_design(technology_id, chassis, weapon, armor, 'assault', [amphibious_pods]);
		}
		if (is_available(air_superiority, known)) {
			add_design(technology_id, chassis, weapon, armor, 'assault', [air_superiority]);
		}
	}
	for (chassis of manifest.chassis) {
		if (
			chassis.missile || !is_available(chassis, known) ||
			chassis.required_technology != technology_id
		) {
			continue;
		}
		add_design(technology_id, chassis, weapon, armor, 'assault', []);
		add_design(technology_id, chassis, hand_weapons, armor, 'garrison', []);
		if (chassis.triad != 'air') {
			add_design(technology_id, chassis, colony_module, no_armor, 'colony', []);
		}
	}
	if (is_available(troop_transport, known)) {
		for (chassis of manifest.chassis) {
			if (
				chassis.triad != 'sea' || !is_available(chassis, known) ||
				(
					chassis.required_technology != technology_id &&
					troop_transport.required_technology != technology_id
				)
			) {
				continue;
			}
			add_design(technology_id, chassis, troop_transport, no_armor, 'transport', []);
		}
	}
	const missile = find_component(manifest.chassis, 'Missile');
	if (
		technology_id == missile.required_technology &&
		is_available(missile, known) && is_available(conventional_payload, known)
	) {
		add_design(
			technology_id,
			missile,
			conventional_payload,
			no_armor,
			'assault',
			[]
		);
	}
	if (
		technology_id == planet_buster.required_technology &&
		is_available(missile, known) && is_available(planet_buster, known)
	) {
		add_design(
			technology_id,
			missile,
			planet_buster,
			no_armor,
			'assault',
			[]
		);
	}
	const carrier_chassis = chassis_by_triad['sea'];
	if (
		technology_id == carrier_deck.required_technology &&
		#is_defined(carrier_chassis) && is_available(carrier_deck, known)
	) {
		add_design(
			technology_id,
			carrier_chassis,
			hand_weapons,
			armor,
			'garrison',
			[carrier_deck]
		);
	}
	const infantry = find_component(manifest.chassis, 'Infantry');
	for (ability_id of ['HighMorale', 'CleanReactor']) {
		const ability = find_component(manifest.abilities, ability_id);
		if (is_available(ability, known)) {
			add_design(technology_id, infantry, hand_weapons, no_armor, 'garrison', [ability]);
		}
	}

	const former_chassis = chassis_by_triad['land'];
	if (#is_defined(former_chassis) && is_available(terraforming_unit, known)) {
		add_design(technology_id, former_chassis, terraforming_unit, no_armor, 'former', []);
		let former_abilities = [];
		for (ability_id of ['SuperFormer', 'FungicideTanks']) {
			const ability = find_component(manifest.abilities, ability_id);
			if (is_available(ability, known)) {
				former_abilities :+ability;
			}
		}
		if (#sizeof(former_abilities) > 0) {
			add_design(
				technology_id,
				former_chassis,
				terraforming_unit,
				no_armor,
				'former',
				former_abilities
			);
		}
		const clean_reactor = find_component(manifest.abilities, 'CleanReactor');
		if (is_available(clean_reactor, known)) {
			add_design(
				technology_id,
				former_chassis,
				terraforming_unit,
				no_armor,
				'former',
				[clean_reactor]
			);
		}
	}
};

add_milestone_designs('');
for (technology_id of technologies.order) {
	add_milestone_designs(technology_id);
}

return {
	definitions: definitions,
};
