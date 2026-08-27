const prototype_rules = #include('../default/game/prototype_rules');

let components = ['ColonyModule', 'HandWeapons', 'Infantry', 'NoArmor'];
let faction_id = 'UNIVERSITY';
const player = {
	get_faction: () => { return {id: faction_id}; },
	get_prototyped_components: () => { return #clone(components); },
	has_prototyped_component: (id) => {
		for (component of components) {
			if (component == id) {
				return true;
			}
		}
		return false;
	},
	set_prototyped_components: (value) => { components = #clone(value); },
};

let facilities = [];
const base = {
	get_owner: () => { return player; },
	get_facilities: () => { return facilities; },
};

const prototype = {
	production_kind: 'unit',
	is_native: false,
	chassis: 'Speeder',
	weapon: 'Laser',
	armor: 'NoArmor',
};
test.assert(prototype_rules.get_components(prototype) == [
	'Speeder', 'Laser', 'NoArmor',
]);
test.assert(prototype_rules.get_missing_components(player, prototype) == [
	'Speeder', 'Laser',
]);
test.assert(prototype_rules.is_prototype(player, prototype));
test.assert(prototype_rules.get_mineral_cost(base, prototype, 20) == 30);

const applied = prototype_rules.apply(player, prototype);
test.assert(components == [
	'ColonyModule', 'HandWeapons', 'Infantry', 'NoArmor', 'Speeder', 'Laser',
]);
test.assert(!prototype_rules.is_prototype(player, prototype));
test.assert(prototype_rules.get_mineral_cost(base, prototype, 20) == 20);
prototype_rules.rollback(applied);
test.assert(components == ['ColonyModule', 'HandWeapons', 'Infantry', 'NoArmor']);

facilities = [{prototype_cost_waiver: true}];
test.assert(prototype_rules.has_cost_waiver(base));
test.assert(prototype_rules.get_mineral_cost(base, prototype, 20) == 20);
facilities = [];

faction_id = 'SPARTANS';
test.assert(prototype_rules.has_cost_waiver(base));
test.assert(prototype_rules.get_mineral_cost(base, prototype, 20) == 20);
faction_id = 'UNIVERSITY';

test.assert(prototype_rules.get_mineral_cost(base, {
	production_kind: 'unit',
	is_native: true,
	chassis: 'Infantry',
	weapon: 'PsiAttack',
	armor: 'PsiDefense',
}, 30) == 30);
test.assert(prototype_rules.get_mineral_cost(base, {
	production_kind: 'facility',
}, 60) == 60);
