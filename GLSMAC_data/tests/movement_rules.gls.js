const movement_rules = #include('../default/game/movement_rules');
const move_unit = #include('../default/game/event/move_unit');

const owner = {id: 1};
const enemy_owner = {id: 2};

const make_tile = () => {
	let neighbours = [];
	let units = [];
	let base = null;
	return {
		is_land: true,
		is_water: false,
		features: {river: false, xenofungus: false},
		terraforming: {road: false},
		rockiness: 0,
		get_surrounding_tiles: () => { return neighbours; },
		get_units: () => { return units; },
		get_base: () => { return base; },
		is_locked: () => { return false; },
		is_adjactent_to: (tile) => {
			for (neighbour of neighbours) {
				if (neighbour == tile) {
					return true;
				}
			}
			return false;
		},
		set_neighbours: (value) => { neighbours = value; },
		set_units: (value) => { units = value; },
		set_base: (value) => { base = value; },
	};
};

const source = make_tile();
const destination = make_tile();
const source_guard = make_tile();
const destination_guard = make_tile();
const mover = {
	owner: owner.id,
	is_land: true,
	is_water: false,
	is_air: false,
	is_immovable: false,
	terraforming: 'none',
	movement: 1.0,
	health: 1.0,
	get_tile: () => { return source; },
};
const enemy_land = {owner: enemy_owner.id, is_land: true, is_water: false, is_air: false};

source.set_neighbours([source_guard, destination]);
destination.set_neighbours([destination_guard, source]);
source_guard.set_units([enemy_land]);
destination_guard.set_units([enemy_land]);

test.assert(movement_rules.tile_is_in_enemy_zoc(mover, source));
test.assert(movement_rules.tile_is_in_enemy_zoc(mover, destination));
test.assert(movement_rules.is_zoc_move_blocked(mover, source, destination));
test.assert(move_unit.validate({
	caller: owner.id,
	game: {is_turn_complete: () => { return false; }},
	data: {unit: mover, tile: destination},
}) == 'Unit cannot move directly between enemy zones of control');

source_guard.set_units([]);
test.assert(!movement_rules.is_zoc_move_blocked(mover, source, destination));
source_guard.set_units([enemy_land]);

destination.set_base({get_owner: () => { return enemy_owner; }});
test.assert(!movement_rules.is_zoc_move_blocked(mover, source, destination));
destination.set_base(null);

destination.set_units([{owner: owner.id, is_land: true, is_water: false, is_air: false}]);
test.assert(!movement_rules.is_zoc_move_blocked(mover, source, destination));
destination.set_units([]);

const sea_mover = {owner: owner.id, is_land: false, is_water: true, is_air: false};
const air_mover = {owner: owner.id, is_land: false, is_water: false, is_air: true};
test.assert(!movement_rules.is_zoc_move_blocked(sea_mover, source, destination));
test.assert(!movement_rules.is_zoc_move_blocked(air_mover, source, destination));

const enemy_sea = {owner: enemy_owner.id, is_land: false, is_water: true, is_air: false};
source_guard.set_units([enemy_sea]);
test.assert(!movement_rules.tile_is_in_enemy_zoc(mover, source));

source_guard.set_units([]);
source_guard.set_base({get_owner: () => { return enemy_owner; }});
test.assert(movement_rules.tile_is_in_enemy_zoc(mover, source));
