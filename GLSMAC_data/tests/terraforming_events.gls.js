const terraform_tile = #include('../default/game/event/terraform_tile');
const cancel_terraform = #include('../default/game/event/cancel_terraform');
const terraforming = #include('../default/units/terraforming');

let tile_state = {
	is_water: false,
	is_locked: false,
	base: null,
	other_units: [],
	nearby_tiles: [],
	updates: 0,
	feature_updates: 0,
};

let known_technologies = {};
const owner = {
	id: 1,
	has_technology: (id) => {
		return #is_defined(known_technologies[id]);
	},
};

let tile = null;
tile = {
	is_water: false,
	features: {
		monolith: false,
		xenofungus: false,
	},
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
	rockiness: 1,
	is_locked: () => {
		return tile_state.is_locked;
	},
	get_base: () => {
		return tile_state.base;
	},
	get_units: () => {
		return tile_state.other_units;
	},
	get_surrounding_tiles: () => {
		return tile_state.nearby_tiles;
	},
	update_terraforming: (changes) => {
		for (type in changes) {
			tile.terraforming[type] = changes[type];
		}
		tile_state.updates = tile_state.updates + 1;
	},
	update_features: (changes) => {
		for (type in changes) {
			tile.features[type] = changes[type];
		}
		tile_state.feature_updates = tile_state.feature_updates + 1;
	},
};

let can_terraform = true;
let former_abilities = [];
let unit = null;
unit = {
	id: 10,
	owner: 1,
	movement: 1.0,
	health: 1.0,
	moved_this_turn: false,
	terraforming: 'none',
	terraforming_turns_remaining: 0,
	get_def: () => {
		return {can_terraform: can_terraform, abilities: former_abilities};
	},
	get_tile: () => {
		return tile;
	},
	get_owner: () => { return owner; },
	set_terraforming_order: (type, turns) => {
		unit.terraforming = type;
		unit.terraforming_turns_remaining = turns;
	},
};

let turn_complete = false;
let terraforming_rate_multiplier = 1.0;
let fungus_terraforming_rate_multiplier = 1.0;
const event = {
	caller: 1,
	data: {
		unit: unit,
		type: 'farm',
	},
	game: {
		is_turn_complete: (player) => {
			return turn_complete;
		},
		get: (key) => {
			test.assert(key == 'f_project_get_player_effects');
			return (owner) => {
				test.assert(owner.id == 1);
				return {
					terraforming_rate_multiplier: terraforming_rate_multiplier,
					fungus_terraforming_rate_multiplier:
						fungus_terraforming_rate_multiplier,
				};
			};
		},
	},
};

test.assert(!#is_defined(terraform_tile.validate(event)));
test.assert(terraform_tile.resolve(event) == {});

event.data.type = 1;
test.assert(#is_defined(terraform_tile.validate(event)));
event.data.type = 'unknown';
test.assert(#is_defined(terraform_tile.validate(event)));
event.data.type = 'farm';
event.caller = 2;
test.assert(#is_defined(terraform_tile.validate(event)));
event.caller = 1;
turn_complete = true;
test.assert(#is_defined(terraform_tile.validate(event)));
turn_complete = false;
can_terraform = false;
test.assert(#is_defined(terraform_tile.validate(event)));
can_terraform = true;
tile.is_water = true;
test.assert(#is_defined(terraform_tile.validate(event)));
tile.is_water = false;
tile.features.xenofungus = true;
test.assert(#is_defined(terraform_tile.validate(event)));
tile.features.xenofungus = false;
tile.terraforming.farm = true;
test.assert(#is_defined(terraform_tile.validate(event)));
tile.terraforming.farm = false;
tile_state.other_units = [{id: 11, terraforming: 'mine'}];
test.assert(#is_defined(terraform_tile.validate(event)));
tile_state.other_units = [];

event.data.type = 'condenser';
test.assert(#is_defined(terraform_tile.validate(event)));
known_technologies.EcologicalEngineering = true;
test.assert(!#is_defined(terraform_tile.validate(event)));
known_technologies = {};

event.data.type = 'farm';
tile.rockiness = 3;
test.assert(#is_defined(terraform_tile.validate(event)));
tile.rockiness = 1;

event.data.type = 'soil_enricher';
known_technologies.AdvancedEcologicalEngineering = true;
test.assert(#is_defined(terraform_tile.validate(event)));
tile.terraforming.farm = true;
test.assert(!#is_defined(terraform_tile.validate(event)));
tile.terraforming.farm = false;
known_technologies = {};

event.data.type = 'mag_tube';
known_technologies.MonopoleMagnets = true;
test.assert(#is_defined(terraform_tile.validate(event)));
tile.terraforming.road = true;
test.assert(!#is_defined(terraform_tile.validate(event)));
tile.terraforming.road = false;
known_technologies = {};

event.data.type = 'borehole';
known_technologies.EcologicalEngineering = true;
tile_state.nearby_tiles = [{terraforming: {borehole: true}}];
test.assert(#is_defined(terraform_tile.validate(event)));
tile_state.nearby_tiles = [];
test.assert(!#is_defined(terraform_tile.validate(event)));
known_technologies = {};

event.data.type = 'remove_fungus';
test.assert(#is_defined(terraform_tile.validate(event)));
tile.features.xenofungus = true;
test.assert(!#is_defined(terraform_tile.validate(event)));
event.data.type = 'plant_fungus';
known_technologies.EcologicalEngineering = true;
test.assert(#is_defined(terraform_tile.validate(event)));
tile.features.xenofungus = false;
known_technologies = {};

event.data.type = 'farm';
event.applied = terraform_tile.apply(event);
test.assert(unit.terraforming == 'farm');
test.assert(unit.terraforming_turns_remaining == 4);
test.assert(event.data.unit.movement == 0.0);
test.assert(event.data.unit.moved_this_turn);
terraform_tile.rollback(event);
test.assert(unit.terraforming == 'none');
test.assert(unit.terraforming_turns_remaining == 0);
test.assert(event.data.unit.movement == 1.0);
test.assert(!event.data.unit.moved_this_turn);

terraforming_rate_multiplier = 1.5;
event.applied = terraform_tile.apply(event);
test.assert(unit.terraforming_turns_remaining == 3);
terraform_tile.rollback(event);
terraforming_rate_multiplier = 1.0;

former_abilities = ['SuperFormer'];
event.applied = terraform_tile.apply(event);
test.assert(unit.terraforming_turns_remaining == 2);
terraform_tile.rollback(event);
former_abilities = [];

event.data.type = 'remove_fungus';
tile.features.xenofungus = true;
former_abilities = ['FungicideTanks'];
event.applied = terraform_tile.apply(event);
test.assert(unit.terraforming_turns_remaining == 3);
terraform_tile.rollback(event);
former_abilities = [];

fungus_terraforming_rate_multiplier = 2.0;
event.applied = terraform_tile.apply(event);
test.assert(unit.terraforming_turns_remaining == 3);
terraform_tile.rollback(event);
fungus_terraforming_rate_multiplier = 1.0;
event.data.type = 'farm';
tile.features.xenofungus = false;

unit.set_terraforming_order('farm', 4);
test.assert(terraforming.advance_order(unit));
test.assert(unit.terraforming_turns_remaining == 3);
test.assert(terraforming.advance_order(unit));
test.assert(unit.terraforming_turns_remaining == 2);
test.assert(terraforming.advance_order(unit));
test.assert(unit.terraforming_turns_remaining == 1);
test.assert(!terraforming.advance_order(unit));
test.assert(unit.terraforming == 'none');
test.assert(unit.terraforming_turns_remaining == 0);
test.assert(tile.terraforming.farm);
test.assert(tile_state.updates == 1);

tile.terraforming.solar = true;
unit.set_terraforming_order('mine', 1);
test.assert(!terraforming.advance_order(unit));
test.assert(tile.terraforming.mine);
test.assert(!tile.terraforming.solar);
test.assert(tile_state.updates == 2);

unit.set_terraforming_order('road', 2);
test.assert(terraforming.advance_order(unit));
test.assert(unit.terraforming_turns_remaining == 1);
test.assert(!terraforming.advance_order(unit));
test.assert(tile.terraforming.road);
test.assert(tile_state.updates == 3);

tile.terraforming.farm = true;
tile.terraforming.mine = true;
tile.terraforming.solar = true;
unit.set_terraforming_order('forest', 1);
test.assert(!terraforming.advance_order(unit));
test.assert(tile.terraforming.forest);
test.assert(!tile.terraforming.farm);
test.assert(!tile.terraforming.mine);
test.assert(!tile.terraforming.solar);
test.assert(tile_state.updates == 4);

unit.set_terraforming_order('solar', 1);
test.assert(!terraforming.advance_order(unit));
test.assert(!tile.terraforming.forest);
test.assert(tile.terraforming.solar);
test.assert(!tile.terraforming.mine);
test.assert(tile_state.updates == 5);

tile.features.xenofungus = true;
unit.set_terraforming_order('remove_fungus', 1);
test.assert(!terraforming.advance_order(unit));
test.assert(!tile.features.xenofungus);
test.assert(tile_state.updates == 5);
test.assert(tile_state.feature_updates == 1);

tile.terraforming.farm = true;
tile.terraforming.soil_enricher = true;
tile.terraforming.solar = true;
unit.set_terraforming_order('plant_fungus', 1);
test.assert(!terraforming.advance_order(unit));
test.assert(tile.features.xenofungus);
test.assert(!tile.terraforming.farm);
test.assert(!tile.terraforming.soil_enricher);
test.assert(!tile.terraforming.solar);
test.assert(tile_state.updates == 6);
test.assert(tile_state.feature_updates == 2);

unit.set_terraforming_order('borehole', 1);
test.assert(!terraforming.advance_order(unit));
test.assert(tile.terraforming.borehole);
test.assert(tile_state.updates == 7);

unit.set_terraforming_order('solar', 2);
unit.movement = 0.0;
unit.moved_this_turn = true;
const cancel_event = {
	caller: 1,
	game: event.game,
	data: {unit: unit},
};
test.assert(!#is_defined(cancel_terraform.validate(cancel_event)));
cancel_event.applied = cancel_terraform.apply(cancel_event);
test.assert(unit.terraforming == 'none');
test.assert(unit.terraforming_turns_remaining == 0);
cancel_terraform.rollback(cancel_event);
test.assert(unit.terraforming == 'solar');
test.assert(unit.terraforming_turns_remaining == 2);
test.assert(unit.movement == 0.0);
test.assert(unit.moved_this_turn);
