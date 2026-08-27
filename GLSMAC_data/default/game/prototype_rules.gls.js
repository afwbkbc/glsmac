const get_components = (def) => {
	let result = [];
	let seen = {};
	for (id of [def.chassis, def.weapon, def.armor]) {
		if (
			#is_defined(id) && #typeof(id) == 'String' && id != '' &&
			!#is_defined(seen[id])
		) {
			seen[id] = true;
			result :+id;
		}
	}
	return result;
};

const get_missing_components = (player, def) => {
	if (
		(#is_defined(def.is_native) && def.is_native) ||
		#typeof(player.has_prototyped_component) != 'Callable'
	) {
		return [];
	}
	let result = [];
	for (id of get_components(def)) {
		if (!player.has_prototyped_component(id)) {
			result :+id;
		}
	}
	return result;
};

const is_prototype = (player, def) => {
	return #sizeof(get_missing_components(player, def)) > 0;
};

const has_cost_waiver = (base) => {
	for (facility of base.get_facilities()) {
		if (#is_defined(facility.prototype_cost_waiver) && facility.prototype_cost_waiver) {
			return true;
		}
	}
	return base.get_owner().get_faction().id == 'SPARTANS';
};

const get_mineral_cost = (base, production, base_cost) => {
	if (
		production.production_kind != 'unit' ||
		!is_prototype(base.get_owner(), production) ||
		has_cost_waiver(base)
	) {
		return base_cost;
	}
	return #ceil(#to_float(base_cost) * 1.5);
};

const apply = (player, def) => {
	const missing = get_missing_components(player, def);
	if (#sizeof(missing) == 0) {
		return #undefined;
	}
	const previous = player.get_prototyped_components();
	let components = [];
	for (id of previous) {
		components :+id;
	}
	for (id of missing) {
		components :+id;
	}
	player.set_prototyped_components(components);
	return {player: player, components: previous};
};

const rollback = (applied) => {
	applied.player.set_prototyped_components(applied.components);
};

return {
	get_components: get_components,
	get_missing_components: get_missing_components,
	is_prototype: is_prototype,
	has_cost_waiver: has_cost_waiver,
	get_mineral_cost: get_mineral_cost,
	apply: apply,
	rollback: rollback,
};
