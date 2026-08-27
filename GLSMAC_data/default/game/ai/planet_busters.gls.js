const PLANET_BUSTER_WEAPON = 'PlanetBuster';
const MIN_TARGET_BASE_SIZE = 4;

const tile_key = (tile) => {
	return #to_string(tile.x) + ':' + #to_string(tile.y);
};

const get_tiles_in_radius = (center, radius) => {
	let result = [center];
	let frontier = [center];
	let seen = {};
	const center_key = tile_key(center);
	seen[center_key] = true;
	for (let distance = 0; distance < radius; distance++) {
		let next = [];
		for (tile of frontier) {
			for (nearby of tile.get_surrounding_tiles()) {
				const key = tile_key(nearby);
				if (!#is_defined(seen[key])) {
					seen[key] = true;
					result :+nearby;
					next :+nearby;
				}
			}
		}
		frontier = next;
	}
	return result;
};

const has_friendly_collateral = (target, radius, player_id) => {
	return has_protected_collateral(
		target,
		radius,
		(owner_id) => { return owner_id == player_id; }
	);
};

const has_protected_collateral = (target, radius, is_protected_partner) => {
	for (tile of get_tiles_in_radius(target, radius)) {
		const base = tile.get_base();
		if (base != null && is_protected_partner(base.get_owner().id)) {
			return true;
		}
		for (unit of tile.get_units(true)) {
			if (is_protected_partner(unit.owner)) {
				return true;
			}
		}
	}
	return false;
};

const get_target_score = (tile, radius, player_id) => {
	let base_population = 0;
	let enemy_units = 0;
	for (affected of get_tiles_in_radius(tile, radius)) {
		const base = affected.get_base();
		if (base != null && base.get_owner().id != player_id) {
			base_population += base.get_size();
		}
		for (unit of affected.get_units(true)) {
			if (unit.owner != player_id) {
				enemy_units++;
			}
		}
	}
	return base_population * 1000 + enemy_units * 100;
};

const choose_target = (unit, player, tiles, is_protected_partner, minimum_target_size) => {
	const definition = unit.get_def();
	if (
		definition.weapon != PLANET_BUSTER_WEAPON ||
		!definition.is_missile || definition.reactor_power <= 0
	) {
		return null;
	}
	const minimum_size = #is_defined(minimum_target_size)
		? minimum_target_size
		: MIN_TARGET_BASE_SIZE;
	const protects_owner = (owner_id) => {
		return owner_id == player.id ||
			(#is_defined(is_protected_partner) && is_protected_partner(owner_id));
	};
	let best = null;
	let best_score = 0;
	for (tile of tiles) {
		const base = tile.get_base();
		if (base == null) {
			continue;
		}
		const owner = base.get_owner();
		if (
			owner.id == player.id ||
			base.get_size() < minimum_size ||
			player.get_diplomatic_relation(owner) != 'vendetta' ||
			protects_owner(owner.id) ||
			has_protected_collateral(
				tile,
				definition.reactor_power,
				protects_owner
			)
		) {
			continue;
		}
		const score = get_target_score(tile, definition.reactor_power, player.id);
		if (
			best == null || score > best_score ||
			(
				score == best_score &&
				(tile.y < best.y || (tile.y == best.y && tile.x < best.x))
			)
		) {
			best = tile;
			best_score = score;
		}
	}
	return best;
};

return {
	choose_target: choose_target,
	has_friendly_collateral: has_friendly_collateral,
	has_protected_collateral: has_protected_collateral,
};
