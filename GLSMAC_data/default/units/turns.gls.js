const terraforming = #include('terraforming');
const air = #include('air');
const unit_abilities = #include('../game/unit_abilities');

const FIELD_REPAIR_MAX_HEALTH = 0.8;

const facility_repairs_unit = (facility, def) => {
	return (
		#is_defined(def.is_land) && def.is_land &&
		#is_defined(facility.full_repair_land) && facility.full_repair_land
	) || (
		#is_defined(def.is_water) && def.is_water &&
		#is_defined(facility.full_repair_water) && facility.full_repair_water
	) || (
		#is_defined(def.is_air) && def.is_air &&
		#is_defined(facility.full_repair_air) && facility.full_repair_air
	) || (
		#is_defined(def.is_native) && def.is_native &&
		#is_defined(facility.full_repair_native) && facility.full_repair_native
	);
};

const get_repair = (unit, def, project_effects, base_facilities) => {
	if (unit.moved_this_turn || unit.terraforming != 'none' || unit.health >= def.health_max) {
		return 0.0;
	}
	if (#is_defined(project_effects) && project_effects.full_repair) {
		return def.health_max - unit.health;
	}
	const tile = unit.get_tile();
	const base = tile.get_base();
	const is_friendly_base = base != null && base.get_owner().id == unit.owner;
	if (is_friendly_base) {
		if (#is_defined(base_facilities)) {
			for (facility of base_facilities) {
				if (facility_repairs_unit(facility, def)) {
					return def.health_max - unit.health;
				}
			}
		}
	}
	const maximum_repaired_health = is_friendly_base
		? def.health_max
		: #min(def.health_max, FIELD_REPAIR_MAX_HEALTH);
	if (unit.health >= maximum_repaired_health) {
		return 0.0;
	}
	let repair = def.health_per_turn;
	if (is_friendly_base) {
		repair *= 2.0;
	}
	if (#is_defined(tile.terraforming)) {
		if (
			#is_defined(def.is_air) && def.is_air &&
			#is_defined(tile.terraforming.airbase) && tile.terraforming.airbase
		) {
			repair += def.health_per_turn;
		}
		if (
			#is_defined(def.is_land) && def.is_land &&
			#is_defined(tile.terraforming.bunker) && tile.terraforming.bunker
		) {
			repair += def.health_per_turn;
		}
	}
	if (
		#is_defined(def.is_land) && def.is_land &&
		#is_defined(unit.is_embarked) && unit.is_embarked &&
		#is_defined(unit.get_transport)
	) {
		const transport = unit.get_transport();
		if (transport != null && unit_abilities.has(transport, 'RepairBay')) {
			repair *= 2.0;
		}
	}
	return #min(repair, maximum_repaired_health - unit.health);
};

const get_movement = (unit, def, project_effects) => {
	return def.movement_per_turn + (
		#is_defined(project_effects) && unit.is_water && !def.is_native
			? project_effects.naval_movement_bonus
			: 0.0
	);
};

const result = {
	get_repair: get_repair,
	facility_repairs_unit: facility_repairs_unit,
	FIELD_REPAIR_MAX_HEALTH: FIELD_REPAIR_MAX_HEALTH,
	get_movement: get_movement,
	get_air_turn_state: air.get_turn_state,

	configure: (game) => {

		const um = game.get_um();

		um.on('unit_turn', (e) => {
			const def = e.unit.get_def();
			const air_state = air.get_turn_state(e.unit, def);
			if (e.unit.fuel != air_state.fuel) {
				e.unit.set_fuel(air_state.fuel);
			}
			if (air_state.damage > 0.0) {
				e.unit.health = #max(0.0, e.unit.health - air_state.damage);
			}
			if (air_state.crash || (air_state.damage > 0.0 && e.unit.health <= 0.0)) {
				e.unit.movement = 0.0;
				if (game.is_master()) {
					game.event('despawn_unit', {unit: e.unit});
				}
				return;
			}
			const get_project_effects = #is_defined(game.get)
				? game.get('f_project_get_player_effects')
				: #undefined;
			const project_effects = #is_defined(get_project_effects)
				? get_project_effects(e.unit.get_owner())
				: {naval_movement_bonus: 0.0, full_repair: false};
			let base_facilities = [];
			const base = e.unit.get_tile().get_base();
			if (base != null && base.get_owner().id == e.unit.owner) {
				const get_effective_facilities = #is_defined(game.get)
					? game.get('f_base_get_effective_facilities')
					: #undefined;
				if (#is_defined(get_effective_facilities)) {
					base_facilities = get_effective_facilities(base);
				}
			}
			const repair = air_state.damage > 0.0
				? 0.0
				: get_repair(e.unit, def, project_effects, base_facilities);
			if (repair > 0.0) {
				e.unit.health = e.unit.health + repair;
			}
			let is_still_terraforming = false;
			if (e.unit.terraforming != 'none') {
				is_still_terraforming = terraforming.advance_order(e.unit);
			}
			if (!def.is_immovable && !is_still_terraforming) {
				e.unit.movement = get_movement(e.unit, def, project_effects);
			}
		});

	},
};

return result;
