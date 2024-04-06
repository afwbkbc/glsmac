const weapons = #include('combat/weapons');

const MIN_DAMAGE_VALUE = 0.25;
const MAX_DAMAGE_VALUE = 1.0;
const DAMAGE_MULTIPLIER = 1.0 / 3.0;

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

    init: () => {

        #game.on.unit_attack_validate((e) => {
            #print('UNIT ATTACK VALIDATE:', e.attacker.id, ' -> ', e.defender.id);

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
            #print('UNIT ATTACK RESOLVE:', e.attacker.id, ' -> ', e.defender.id);

            let attack_power = get_unit_attack_power(e.attacker);
            let defence_power = get_unit_defence_power(e.defender);
            #print('ATTACK POWER: ', attack_power);
            #print('DEFENCE POWER: ', defence_power);

            let attacker_health = e.attacker.health;
            let defender_health = e.defender.health;
            let damage_sequence = [];
            while (attacker_health > 0.0 && defender_health > 0.0) {
                const damage_to_defender = #min(defender_health, #game.random.get_float(attack_power * MIN_DAMAGE_VALUE, attack_power * MAX_DAMAGE_VALUE) * DAMAGE_MULTIPLIER);
                const damage_to_attacker = #min(attacker_health, #game.random.get_float(defence_power * MIN_DAMAGE_VALUE, defence_power * MAX_DAMAGE_VALUE) * DAMAGE_MULTIPLIER);
                defender_health -= damage_to_defender;
                attacker_health -= damage_to_attacker;
                damage_sequence []= [damage_to_defender, damage_to_attacker];
                #print( defender_health, attacker_health );
            }

            return damage_sequence;
        });

        #game.on.unit_attack_apply((e) => {
            #print('UNIT ATTACK APPLY: ', e.attacker.id, ' -> ', e.defender.id, ' // ', e.resolutions);
            e.attacker.movement = 0.0;
            let sz = #size(e.resolutions);
            let i = 0;
            #print( e.defender.health, e.attacker.health );
            while ( i < sz ) {
                const damages = e.resolutions[i];
                #print( 'Defender takes ' + #to_string(damages[0]) + ' damage');
                #print( 'Attacker takes ' + #to_string(damages[1]) + ' damage');

                #game.animations.show_on_tile('WEAPON_PSI', e.defender.get_tile(), () => {
                    //#print('ANIMATION CALLBACK');
                });
                e.defender.health = e.defender.health - damages[0];

                #game.animations.show_on_tile('WEAPON_PSI', e.attacker.get_tile(), () => {
                    //#print('ANIMATION CALLBACK');
                });
                e.attacker.health = e.attacker.health - damages[1];

                i++;
            }
            #print( e.defender.health, e.attacker.health );
            if ( e.defender.health <= 0.0 ) {
                #print( 'Defender is dead' );
            }
            if ( e.attacker.health <= 0.0 ) {
                #print( 'Attacker is dead' );
            }
        });

    },

    define: () => {

        weapons.define();

    },

};

return result;
