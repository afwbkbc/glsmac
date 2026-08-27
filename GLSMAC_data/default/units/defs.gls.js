const generated = #include('generated');

const moralesets = [
	{
		id: 'STANDARD',
		data: [
			{name: 'Very Green'},
			{name: 'Green'},
			{name: 'Disciplined'},
			{name: 'Hardened'},
			{name: 'Veteran'},
			{name: 'Commando'},
			{name: 'Elite'},
		]
	},
	{
		id: 'NATIVE',
		data: [
			{name: 'Hatchling'},
			{name: 'Larval Mass'},
			{name: 'Pre-Boil'},
			{name: 'Boil'},
			{name: 'Mature Boil'},
			{name: 'Great Boil'},
			{name: 'Demon Boil'},
		]
	},
];

const native_lifeform = (
	id,
	name,
	mineral_cost,
	movement_type,
	movement_per_turn,
	base_y,
	abilities,
	cargo_capacity,
	required_technology
) => {
	return {
		id: id,
		data: {
			name: name,
			mineral_cost: mineral_cost,
			is_native: true,
			offense: 1,
			defense: 1,
			chassis: movement_type == 'water'
				? 'Foil'
				: (movement_type == 'air' ? 'Gravship' : 'Infantry'),
			weapon: 'PsiAttack',
			armor: 'PsiDefense',
			reactor: 'FissionPlant',
			reactor_power: 1,
			abilities: abilities,
			required_technology: required_technology,
			morale: 'NATIVE',
			type: 'static',
			movement_type: movement_type,
			movement_per_turn: movement_per_turn,
			operational_range: 0,
			is_missile: false,
			cargo_capacity: cargo_capacity,
			render: {
				type: 'sprite',
				file: 'units.pcx',
				x: 2, y: base_y,
				w: 100, h: 75,
				cx: 53, cy: base_y + 51,
				morale_based_xshift: 102
			},
		},
	};
};

const conventional_unit = (
	id,
	name,
	mineral_cost,
	offense,
	defense,
	sprite_x,
	sprite_y,
	can_found_base,
	can_terraform,
	required_technology,
	movement_per_turn,
	chassis,
	weapon,
	armor
) => {
	return {
		id: id,
		data: {
			name: name,
			mineral_cost: mineral_cost,
			is_native: false,
			offense: offense,
			defense: defense,
			can_found_base: can_found_base,
			can_terraform: can_terraform,
			required_technology: required_technology,
			chassis: chassis,
			weapon: weapon,
			armor: armor,
			reactor: 'FissionPlant',
			reactor_power: 1,
			abilities: [],
			morale: 'STANDARD',
			type: 'static',
			movement_type: 'land',
			movement_per_turn: movement_per_turn,
			operational_range: 0,
			is_missile: false,
			cargo_capacity: 0,
			render: {
				type: 'sprite',
				file: 'units.pcx',
				x: sprite_x, y: sprite_y,
				w: 100, h: 75,
				cx: sprite_x + 51, cy: sprite_y + 51,
			},
		},
	};
};

const special_unit = (
	id,
	name,
	mineral_cost,
	offense,
	defense,
	sprite_x,
	sprite_y,
	movement_type,
	movement_per_turn,
	operational_range,
	cargo_capacity,
	chassis,
	weapon,
	armor,
	required_technology
) => {
	return {
		id: id,
		data: {
			name: name,
			mineral_cost: mineral_cost,
			is_native: false,
			offense: offense,
			defense: defense,
			can_found_base: false,
			can_terraform: false,
			required_technology: required_technology,
			chassis: chassis,
			weapon: weapon,
			armor: armor,
			reactor: 'FissionPlant',
			reactor_power: 1,
			abilities: [],
			morale: 'STANDARD',
			type: 'static',
			movement_type: movement_type,
			movement_per_turn: movement_per_turn,
			operational_range: operational_range,
			is_missile: false,
			cargo_capacity: cargo_capacity,
			render: {
				type: 'sprite',
				file: 'units.pcx',
				x: sprite_x, y: sprite_y,
				w: 100, h: 75,
				cx: sprite_x + 51, cy: sprite_y + 51,
			},
		},
	};
};

const units = [
	// Stock-sheet fallbacks keep these roles distinct until CVR composition is available.
	conventional_unit('ScoutPatrol', 'Scout Patrol', 10, 1, 1, 2, 156, false, false, '', 1, 'Infantry', 'HandWeapons', 'NoArmor'),
	conventional_unit('ColonyPod', 'Colony Pod', 30, 0, 1, 2, 2, true, false, '', 1, 'Infantry', 'ColonyModule', 'NoArmor'),
	conventional_unit('Former', 'Former', 20, 0, 1, 206, 156, false, true, 'CentauriEcology', 1, 'Infantry', 'TerraformingUnit', 'NoArmor'),
	conventional_unit('ReconRover', 'Recon Rover', 20, 1, 1, 104, 156, false, false, 'DoctrineMobility', 2, 'Speeder', 'HandWeapons', 'NoArmor'),
	conventional_unit('LaserInfantry', 'Laser Infantry', 20, 2, 1, 206, 156, false, false, 'AppliedPhysics', 1, 'Infantry', 'Laser', 'NoArmor'),
	conventional_unit('SynthmetalSentinels', 'Synthmetal Sentinels', 20, 1, 2, 2, 156, false, false, 'IndustrialBase', 1, 'Infantry', 'HandWeapons', 'SynthmetalArmor'),
	conventional_unit('ProbeTeam', 'Probe Team', 40, 0, 1, 104, 156, false, false, 'PlanetaryNetworks', 2, 'Speeder', 'ProbeTeam', 'NoArmor'),
	conventional_unit('AlienArtifact', 'Alien Artifact', 100, 0, 1, 2, 156, false, false, '', 1, 'Infantry', 'AlienArtifact', 'NoArmor'),
	special_unit('UnityRover', 'Unity Rover', 0, 1, 1, 104, 156, 'land', 2, 0, 0, 'Speeder', 'HandWeapons', 'NoArmor', ''),
	special_unit('UnityScoutChopper', 'Unity Scout Chopper', 0, 1, 1, 2, 541, 'air', 8, 1, 0, 'Copter', 'HandWeapons', 'NoArmor', ''),
	special_unit('UnityFoil', 'Unity Foil', 0, 0, 1, 2, 310, 'water', 4, 0, 2, 'Foil', 'TroopTransport', 'NoArmor', ''),
	native_lifeform('FungalTower', 'Fungal Tower', 0, 'immovable', 0, 79, [], 0, ''),
	native_lifeform('MindWorms', 'Mind Worms', 50, 'land', 1, 233, [], 0, 'CentauriEmpathy'),
	native_lifeform('IsleOfTheDeep', 'Isle of the Deep', 80, 'water', 4, 310, [], 4, 'CentauriMeditation'),
	native_lifeform('LocustsOfChiron', 'Locusts of Chiron', 100, 'air', 8, 387, [], 0, 'CentauriGenetics'),
	native_lifeform('SeaLurk', 'Sea Lurk', 40, 'water', 4, 310, [], 0, ''),
	native_lifeform('SporeLauncher', 'Spore Launcher', 50, 'land', 1, 387, ['HeavyArtillery'], 0, ''),
];

for (unit of generated.definitions) {
	units :+unit;
}

for (unit of units) {
	if (
		unit.id == 'AlienArtifact' ||
		unit.id == 'UnityRover' ||
		unit.id == 'UnityScoutChopper' ||
		unit.id == 'UnityFoil'
	) {
		unit.data.buildable = false;
	}
}

const result = {
	moralesets: moralesets,
	definitions: units,
	generated_count: #sizeof(generated.definitions),

	define: (game) => {

		for (moraleset of moralesets) {
			game.event('define_moraleset', moraleset);
		}

		for (unit of units) {
			game.event('define_unit', unit);
		}

	},
};

return result;
