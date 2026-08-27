const COPTER_FIELD_DAMAGE = 0.3;
const unit_abilities = #include('../game/unit_abilities');

const is_refueling = (unit) => {
	const tile = unit.get_tile();
	const base = tile.get_base();
	if (base != null && base.get_owner().id == unit.owner) {
		return true;
	}
	if (
		#is_defined(tile.terraforming) &&
		#is_defined(tile.terraforming.airbase) &&
		tile.terraforming.airbase
	) {
		return true;
	}
	if (#is_defined(tile.get_units)) {
		for (other of tile.get_units()) {
			if (
				other.id != unit.id && other.owner == unit.owner &&
				unit_abilities.has(other, 'CarrierDeck')
			) {
				return true;
			}
		}
	}
	return false;
};

const get_turn_state = (unit, def) => {
	if (!def.is_air || def.operational_range <= 0) {
		return {fuel: 0, damage: 0.0, crash: false, refueling: false};
	}
	if (is_refueling(unit)) {
		return {
			fuel: def.operational_range,
			damage: 0.0,
			crash: false,
			refueling: true,
		};
	}
	const fuel = #max(unit.fuel - 1, 0);
	if (def.chassis == 'Copter') {
		return {fuel: fuel, damage: COPTER_FIELD_DAMAGE, crash: false, refueling: false};
	}
	return {fuel: fuel, damage: 0.0, crash: fuel == 0, refueling: false};
};

return {
	COPTER_FIELD_DAMAGE: COPTER_FIELD_DAMAGE,
	is_refueling: is_refueling,
	get_turn_state: get_turn_state,
};
