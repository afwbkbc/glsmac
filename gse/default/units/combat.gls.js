const MIN_DAMAGE_VALUE = 0.1;
const MAX_DAMAGE_VALUE = 0.3;

const get_unit_attack_power = (unit) => {
    const is_native = true; // TODO: non-native units
    if (is_native) {
        // TODO: proper logic
        return #to_float(unit.morale + 1) / 7.0;
    }
    // TODO: non-native
};

const get_unit_defence_power = (unit) => {
    // TODO: proper logic
    return get_unit_attack_power(unit);
};

const result = {

    init: (animations) => {

        #game.on.unit_attack_validate((e) => {

            if (e.attacker.is_immovable) {
                return 'Unit is immovable';
            }
            if (e.attacker.movement == 0.0) {
                return 'Unit is out of moves';
            }
            if (e.attacker == e.defender) {
                return 'Unit can\'t attack self';
            }
            let attacker_def = e.attacker.get_def();
            if (attacker_def.id == 'SporeLauncher') {
                // TODO: bombardments
                return 'Artillery units can\'t attack directly';
            }
            const source_tile = e.attacker.get_tile();
            const target_tile = e.defender.get_tile();
            if (!source_tile.is_adjactent_to(target_tile)) {
                return 'Defender tile is not adjactent to attacker tile';
            }
            if (e.attacker.is_land && e.defender.is_water) {
                return 'Land units can\'t attack water units';
            }
            if (e.attacker.is_water && e.defender.is_land) {
                return 'Water units can\'t attack land units';
            }
        });

        #game.on.unit_attack_resolve((e) => {

            let attack_power = get_unit_attack_power(e.attacker);
            let defence_power = get_unit_defence_power(e.defender);

            let attacker_health = e.attacker.health;
            let defender_health = e.defender.health;

            let damage_sequence = [];
            while (attacker_health > 0.0 && defender_health > 0.0) {
                let attack_roll = #game.random.get_float(0.0, attack_power);
                let defence_roll = #game.random.get_float(0.0, defence_power);
                if (attack_roll >= defence_roll) {
                    let damage = #min(defender_health, #game.random.get_float(MIN_DAMAGE_VALUE, MAX_DAMAGE_VALUE));
                    damage_sequence []= [true, damage];
                    defender_health -= damage;
                }
                if (defence_roll >= attack_roll) {
                    let damage = #min(attacker_health, #game.random.get_float(MIN_DAMAGE_VALUE, MAX_DAMAGE_VALUE));
                    damage_sequence []= [false, damage];
                    attacker_health -= damage;
                }
            }
            return damage_sequence;
        });

        #game.on.unit_attack_apply((e) => {

            let attacker_tile = e.attacker.get_tile();
            let defender_tile = e.defender.get_tile();

            #game.tiles.lock([attacker_tile, defender_tile], (unlock) => {

                let damages_sz = #size(e.resolutions);

                const process_next_damage = (damage_index) => {
                    if (damage_index < damages_sz) {
                        const damages = e.resolutions[damage_index];
                        if (damages[0]) {
                            #game.animations.show_on_tile(animations.ATTACK_PSI, defender_tile, () => {
                                e.defender.health = e.defender.health - damages[1];
                                process_next_damage(damage_index + 1);
                            });
                        } else {
                            #game.animations.show_on_tile(animations.ATTACK_PSI, attacker_tile, () => {
                                e.attacker.health = e.attacker.health - damages[1];
                                process_next_damage(damage_index + 1);
                            });
                        }
                    }
                    else {
                        e.attacker.movement = 0.0;
                        unlock();
                    }
                };
                process_next_damage(0);
            });
        });

    },

};

return result;
