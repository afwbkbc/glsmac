const MIN_DAMAGE_VALUE = 0.1;
const MAX_DAMAGE_VALUE = 0.3;

const get_unit_attack_power = (unit) => {
	const is_native = true; // TODO: non-native units
	if (is_native) {
		// TODO: proper logic
		let power = #to_float(unit.morale + 1) / 7.0;
		if (unit.is_land) {
			power *= 3.0;
		}
		return power;
	}
	// TODO: non-native
};

const get_unit_defence_power = (unit) => {
	const is_native = true; // TODO: non-native units
	if (is_native) {
		// TODO: proper logic
		let power = #to_float(unit.morale + 1) / 7.0;
		if (unit.is_land) {
			power *= 2.0;
		}
		return power;
	}
	// TODO: non-native
};

return {

	validate: (e) => {
		if (e.data.attacker.owner != e.caller) {
			return 'Unit can only be ordered to attack by it\'s owner';
		}

		const attacker_tile = e.data.attacker.get_tile();
		const defender_tile = e.data.defender.get_tile();

		if (attacker_tile == defender_tile) {
			return 'Attacker tile is same as defender tile';
		}

		if (attacker_tile.is_locked()) {
			return 'Attacker tile is locked';
		}
		if (defender_tile.is_locked()) {
			return 'Defender tile is locked';
		}

		if (e.data.attacker.is_immovable) {
			return 'Attacker is immovable';
		}
		if (e.data.attacker.movement == 0.0) {
			return 'Attacker is out of moves';
		}
		if (attacker_tile == defender_tile) {
			return 'Defender tile is same as attacker tile';
		}
		if (!attacker_tile.is_adjactent_to(defender_tile)) {
			return 'Defender tile is not adjactent to attacker tile';
		}
		if (e.data.attacker.is_land && defender_tile.is_water) {
			// TODO: marine
			return 'Land units can\'t attack water tiles';
		}
		if (e.data.attacker.is_water && defender_tile.is_land) {
			// TODO: marine
			return 'Water units can\'t attack land tiles';
		}

		let attacker_def = e.data.attacker.get_def();
		if (attacker_def.id == 'SporeLauncher') {
			// TODO: bombardments
			return 'Artillery units can\'t attack directly';
		}

	},

	resolve: (e) => {
		const attacker = e.data.attacker;
		const defender = e.data.defender;

		let attack_power = get_unit_attack_power(attacker);
		let defence_power = get_unit_defence_power(defender);

		let attacker_health = attacker.health;
		let defender_health = defender.health;

		let damage_sequence = [];
		while (attacker_health > 0.0 && defender_health > 0.0) {
			let attack_roll = e.game.random.get_float(0.0, attack_power);
			let defence_roll = e.game.random.get_float(0.0, defence_power);
			if (attack_roll >= defence_roll) {
				let damage = #min(defender_health, e.game.random.get_float(MIN_DAMAGE_VALUE, MAX_DAMAGE_VALUE));
				damage_sequence [] = [true, damage];
				defender_health -= damage;
			}
			if (defence_roll >= attack_roll) {
				let damage = #min(attacker_health, e.game.random.get_float(MIN_DAMAGE_VALUE, MAX_DAMAGE_VALUE));
				damage_sequence [] = [false, damage];
				attacker_health -= damage;
			}
		}
		return damage_sequence;
	},

	apply: (e) => {
		const attacker = e.data.attacker;
		const defender = e.data.defender;
		let attacker_tile = attacker.get_tile();
		let defender_tile = defender.get_tile();

		let damages_sz = #sizeof(e.resolved);

		#print('RESOLUTIONS', e.resolved);

		const process_next_damage = (damage_index) => {
			let next = () => {
				process_next_damage(damage_index + 1);
			};
			if (damage_index < damages_sz) {
				const damages = e.resolved[damage_index];
				if (damages[0]) {
					e.game.am.show_animation('ATTACK_PSI', defender_tile, () => {
						defender.health = defender.health - damages[1];
						if (defender.health <= 0.0) {
							e.game.event('despawn_unit', { unit: defender });
							e.game.am.show_animation('DEATH_PSI', defender_tile, next);
						} else {
							next();
						}
					});
				} else {
					e.game.am.show_animation('ATTACK_PSI', attacker_tile, () => {
						attacker.health = attacker.health - damages[1];
						if (attacker.health <= 0.0) {
							e.game.event('despawn_unit', { unit: attacker });
							e.game.am.show_animation('DEATH_PSI', attacker_tile, next);
						} else {
							next();
						}
					});
				}
			} else {
				// TODO: multiattack i.e. rovers
				attacker.movement = 0.0;
			}
		};
		process_next_damage(0);
	},

	rollback: (e) => {
		#print('ROLLBACK', e);
	},

};
