const project_acquisition = #include('./project_acquisition');
const technology_acquisition = #include('./technology_acquisition');

const empty_effects = () => {
	return {
		talent_bonus: 0,
		growth_rating_bonus: 0,
		population_limit_bonus: 0,
		mineral_bonus: 0,
		support_bonus: 0,
		maintenance_multiplier: 1.0,
		native_lifecycle_bonus: 0,
		network_node_drone_modifier: 0,
		network_node_research_bonus: 0,
		prevent_riots: false,
		terraforming_rate_multiplier: 1.0,
		new_base_population: 1,
		small_base_drone_modifier: 0,
		psi_attack_multiplier: 1.0,
		psi_defense_multiplier: 1.0,
		naval_movement_bonus: 0.0,
		full_repair: false,
		police_rating_bonus: 0,
		extra_police_units: 0,
		unit_upgrade_cost_multiplier: 1.0,
		ecology_divisor_bonus: 0,
		native_fungus_combat: false,
		fungus_movement_as_road: false,
		fungus_terraforming_rate_multiplier: 1.0,
		drone_modifier: 0,
		economy_multiplier: 0.0,
		ignore_power_penalties: false,
		ignore_thought_control_penalties: false,
		ignore_cybernetic_penalties: false,
		orbital_access: false,
		orbital_production_multiplier: 1.0,
	};
};

const get_player_projects = (game, player) => {
	let result = [];
	for (candidate of game.get_bm().get_bases()) {
		if (candidate.get_owner().id != player.id) {
			continue;
		}
		for (facility of candidate.get_facilities()) {
			if (facility.is_project) {
				result :+facility;
			}
		}
	}
	return result;
};

const has_project = (game, player, id) => {
	for (project of get_player_projects(game, player)) {
		if (project.id == id) {
			return true;
		}
	}
	return false;
};

const get_planetary_datalinks_candidates = (game, player) => {
	const get_order = game.get('f_technology_get_order');
	if (!#is_defined(get_order)) {
		return [];
	}
	let result = [];
	for (id of get_order()) {
		if (player.has_technology(id)) {
			continue;
		}
		let other_factions = 0;
		for (other of game.get_players()) {
			if (other.id != player.id && other.has_technology(id)) {
				other_factions++;
			}
		}
		if (other_factions >= 3) {
			result :+id;
		}
	}
	return result;
};

const get_planetary_datalinks_grants = (game) => {
	let result = [];
	for (player of game.get_players()) {
		if (!has_project(game, player, 'ThePlanetaryDatalinks')) {
			continue;
		}
		const technologies = get_planetary_datalinks_candidates(game, player);
		if (#sizeof(technologies) > 0) {
			result :+{player: player, technologies: technologies};
		}
	}
	return result;
};

const get_owned_projects = (game, base) => {
	return get_player_projects(game, base.get_owner());
};

const get_player_effects = (game, player) => {
	const result = empty_effects();
	for (project of get_player_projects(game, player)) {
		result.talent_bonus = result.talent_bonus + project.global_talent_bonus;
		result.growth_rating_bonus = result.growth_rating_bonus + project.global_growth_rating_bonus;
		result.population_limit_bonus = result.population_limit_bonus + project.global_population_limit_bonus;
		result.mineral_bonus = result.mineral_bonus + project.global_mineral_bonus;
		result.support_bonus = result.support_bonus + project.global_support_bonus;
		result.maintenance_multiplier = result.maintenance_multiplier *
			project.global_maintenance_multiplier;
		result.native_lifecycle_bonus = result.native_lifecycle_bonus +
			project.global_native_lifecycle_bonus;
		result.network_node_drone_modifier = result.network_node_drone_modifier +
			project.network_node_drone_modifier;
		result.network_node_research_bonus = result.network_node_research_bonus +
			project.network_node_research_bonus;
		result.prevent_riots = result.prevent_riots || project.global_prevent_riots;
		result.terraforming_rate_multiplier = result.terraforming_rate_multiplier * (
			#is_defined(project.global_terraforming_rate_multiplier)
				? project.global_terraforming_rate_multiplier
				: 1.0
		);
		result.new_base_population = #max(
			result.new_base_population,
			#is_defined(project.new_base_population) ? project.new_base_population : 0
		);
		result.small_base_drone_modifier = result.small_base_drone_modifier + (
			#is_defined(project.small_base_drone_modifier) ? project.small_base_drone_modifier : 0
		);
		result.psi_attack_multiplier = result.psi_attack_multiplier * (
			#is_defined(project.global_psi_attack_multiplier)
				? project.global_psi_attack_multiplier
				: 1.0
		);
		result.psi_defense_multiplier = result.psi_defense_multiplier * (
			#is_defined(project.global_psi_defense_multiplier)
				? project.global_psi_defense_multiplier
				: 1.0
		);
		result.naval_movement_bonus = result.naval_movement_bonus + (
			#is_defined(project.global_naval_movement_bonus)
				? project.global_naval_movement_bonus
				: 0.0
		);
		result.full_repair = result.full_repair || (
			#is_defined(project.global_full_repair) && project.global_full_repair
		);
		result.police_rating_bonus = result.police_rating_bonus + (
			#is_defined(project.global_police_rating_bonus)
				? project.global_police_rating_bonus
				: 0
		);
		result.extra_police_units = result.extra_police_units + (
			#is_defined(project.global_extra_police_units)
				? project.global_extra_police_units
				: 0
		);
		if (project.id == 'TheNanoFactory') {
			result.unit_upgrade_cost_multiplier = 0.5;
		}
		if (project.id == 'TheSpaceElevator') {
			result.orbital_access = true;
			result.orbital_production_multiplier = 2.0;
		}
		if (project.id == 'ThePholusMutagen') {
			result.ecology_divisor_bonus = result.ecology_divisor_bonus + 1;
			result.native_fungus_combat = true;
		}
		if (project.id == 'TheXenoempathyDome') {
			result.fungus_movement_as_road = true;
			result.fungus_terraforming_rate_multiplier =
				result.fungus_terraforming_rate_multiplier * 2.0;
		}
		if (project.id == 'TheCloningVats') {
			result.ignore_power_penalties = true;
			result.ignore_thought_control_penalties = true;
		} else if (project.id == 'TheNetworkBackbone') {
			result.ignore_cybernetic_penalties = true;
		} else if (project.id == 'TheLongevityVaccine') {
			const economics = player.get_social_engineering().economics;
			if (economics == 'Planned') {
				result.drone_modifier = result.drone_modifier - 2;
			} else if (economics == 'Simple' || economics == 'Green') {
				result.drone_modifier = result.drone_modifier - 1;
			}
		}
	}
	return result;
};

const get_effects = (game, base) => {
	const result = get_player_effects(game, base.get_owner());
	if (
		base.has_facility('TheLongevityVaccine') &&
		base.get_owner().get_social_engineering().economics == 'FreeMarket'
	) {
		result.economy_multiplier = result.economy_multiplier + 0.5;
	}
	return result;
};

const get_effective_facilities = (game, base) => {
	let result = [];
	let seen = {};
	for (facility of base.get_facilities()) {
		result :+facility;
		seen[facility.id] = true;
	}
	for (project of get_owned_projects(game, base)) {
		if (project.granted_facility == '' || #is_defined(seen[project.granted_facility])) {
			continue;
		}
		const facility = game.get_bm().get_facility_def(project.granted_facility);
		result :+facility;
		seen[facility.id] = true;
	}
	return result;
};

return (game) => {
	let planetary_datalinks_pending = false;
	const apply_completion_effects = (base, project_id) => {
		if (project_id == 'TheEmpathGuild') {
			const applied = project_acquisition.apply_empath_guild(game, base);
			if (#is_defined(applied)) {
				game.message(
					base.get_owner().name +
					' has infiltrated every faction through The Empath Guild.'
				);
				return {kind: 'empath_guild', applied: applied};
			}
			return #undefined;
		}
		if (project_id != 'TheUniversalTranslator') {
			return #undefined;
		}
		const player = base.get_owner();
		const acquired = technology_acquisition.apply(game, player, 2);
		if (!#is_defined(acquired)) {
			return #undefined;
		}
		for (name of acquired.completed_names) {
			game.message(
				player.name + ' has acquired ' + name +
					' through The Universal Translator.'
			);
		}
		return {
			kind: 'universal_translator',
			player: acquired.player,
			state: acquired.state,
			completed_count: acquired.completed_count,
		};
	};
	const rollback_completion_effects = (applied) => {
		if (applied.kind == 'empath_guild') {
			project_acquisition.rollback_empath_guild(applied.applied);
			return;
		}
		technology_acquisition.rollback(game, applied);
	};
	const apply_planetary_datalinks = () => {
		planetary_datalinks_pending = false;
		let snapshots = [];
		let snapshotted = {};
		let updated = {};
		while (true) {
			const grants = get_planetary_datalinks_grants(game);
			if (#sizeof(grants) == 0) {
				break;
			}
			for (grant of grants) {
				const player = grant.player;
				const key = 'p' + #to_string(player.id);
				const previous = player.get_research_state();
				if (!#is_defined(snapshotted[key])) {
					snapshots :+{player: player, state: previous};
					snapshotted[key] = true;
				}
				let technologies = [];
				for (id of previous.technologies) {
					technologies :+id;
				}
				for (id of grant.technologies) {
					technologies :+id;
					const definition = game.get('f_technology_get_definition')(id);
					game.message(
						player.name + ' has acquired ' + definition.name +
						' through The Planetary Datalinks.'
					);
				}
				let target = previous.target;
				let progress = previous.progress;
				for (id of grant.technologies) {
					if (target == id) {
						target = game.get('f_technology_get_next_target')(technologies, player);
						if (target == '') {
							progress = 0;
						}
						break;
					}
				}
				player.set_research_state({
					technologies: technologies,
					target: target,
					progress: progress,
				});
				updated[key] = player;
			}
		}
		for (key in updated) {
			game.trigger('research_updated', {player: updated[key]});
		}
		return {players: snapshots};
	};
	const rollback_planetary_datalinks = (applied) => {
		planetary_datalinks_pending = false;
		for (snapshot of applied.players) {
			snapshot.player.set_research_state(snapshot.state);
			game.trigger('research_updated', {player: snapshot.player});
		}
	};
	const queue_planetary_datalinks = () => {
		if (
			!game.is_master() || planetary_datalinks_pending ||
			#sizeof(get_planetary_datalinks_grants(game)) == 0
		) {
			return false;
		}
		planetary_datalinks_pending = true;
		game.event('process_planetary_datalinks', {});
		return true;
	};

	game.set('f_project_get_owned', (base) => { return get_owned_projects(game, base); });
	game.set('f_project_has', (player, id) => { return has_project(game, player, id); });
	game.set('f_project_get_effects', (base) => { return get_effects(game, base); });
	game.set(
		'f_project_get_player_effects',
		(player) => { return get_player_effects(game, player); }
	);
	game.set(
		'f_base_get_effective_facilities',
		(base) => { return get_effective_facilities(game, base); }
	);
	game.set(
		'f_project_get_planetary_datalinks_candidates',
		(player) => { return get_planetary_datalinks_candidates(game, player); }
	);
	game.set('f_project_queue_planetary_datalinks', queue_planetary_datalinks);
	game.set('f_project_apply_planetary_datalinks', apply_planetary_datalinks);
	game.set('f_project_rollback_planetary_datalinks', rollback_planetary_datalinks);
	game.set('f_project_apply_completion_effects', apply_completion_effects);
	game.set('f_project_rollback_completion_effects', rollback_completion_effects);
	if (#is_defined(game.on)) {
		game.on('turn', queue_planetary_datalinks);
	}
};
