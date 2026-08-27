const get_priority = (priorities, name, fallback) => {
	return #is_defined(priorities[name]) ? priorities[name] : fallback;
};

const score_ratings = (ratings, priorities) => {
	const development = get_priority(priorities, 'development', 50);
	const growth = get_priority(priorities, 'growth', 50);
	const psych = get_priority(priorities, 'psych', 25);
	const military = get_priority(priorities, 'military', 25);
	const expansion = get_priority(priorities, 'expansion', 25);
	const terraforming = get_priority(priorities, 'terraforming', 25);
	return ratings.economy * (100 + development * 4) +
		ratings.effic * (100 + development * 2) +
		ratings.support * (100 + military * 3 + expansion) +
		ratings.talent * (100 + psych * 5) +
		ratings.morale * (100 + military * 5) +
		ratings.police * (100 + psych * 3 + military * 2) +
		ratings.growth * (100 + growth * 5 + expansion * 2) +
		ratings.planet * (100 + terraforming * 3) +
		ratings.probe * (100 + development * 2 + military) +
		ratings.industry * (200 + development * 2 + military * 2 + expansion * 2) +
		ratings.research * (100 + development * 5);
};

const choices_equal = (left, right) => {
	return (
		left.politics == right.politics &&
		left.economics == right.economics &&
		left.values == right.values &&
		left.future_society == right.future_society
	);
};

const choose = (player, categories, get_available, get_ratings, priorities) => {
	const current = player.get_social_engineering();
	let selected = {
		politics: current.politics,
		economics: current.economics,
		values: current.values,
		future_society: current.future_society,
	};
	// Four passes make each category respond to choices made in the other three.
	for (let pass = 0; pass < 4; pass++) {
		for (category of categories) {
			let best_id = selected[category.id];
			let best_score = 0;
			let has_best = false;
			for (choice of get_available(player, category.id)) {
				let candidate = {
					politics: selected.politics,
					economics: selected.economics,
					values: selected.values,
					future_society: selected.future_society,
				};
				candidate[category.id] = choice.id;
				const score = score_ratings(get_ratings(player, candidate), priorities);
				if (
					!has_best || score > best_score ||
					(score == best_score && choice.id < best_id)
				) {
					has_best = true;
					best_id = choice.id;
					best_score = score;
				}
			}
			selected[category.id] = best_id;
		}
	}
	return selected;
};

const choose_adoption = (
	player,
	desired,
	categories,
	get_ratings,
	priorities,
	get_cost,
	available_energy
) => {
	const current = player.get_social_engineering();
	if (get_cost(player, desired) == 0) {
		return desired;
	}
	let selected = {
		politics: current.politics,
		economics: current.economics,
		values: current.values,
		future_society: current.future_society,
	};
	let best_score = score_ratings(get_ratings(player, current), priorities);
	let best_cost = 0;
	for (category of categories) {
		if (current[category.id] == desired[category.id]) {
			continue;
		}
		let candidate = {
			politics: current.politics,
			economics: current.economics,
			values: current.values,
			future_society: current.future_society,
		};
		candidate[category.id] = desired[category.id];
		const cost = get_cost(player, candidate);
		if (cost > available_energy) {
			continue;
		}
		const score = score_ratings(get_ratings(player, candidate), priorities);
		if (
			score > best_score ||
			(score == best_score && best_cost > 0 && cost < best_cost)
		) {
			selected = candidate;
			best_score = score;
			best_cost = cost;
		}
	}
	return selected;
};

return {
	score_ratings: score_ratings,
	choices_equal: choices_equal,
	choose: choose,
	choose_adoption: choose_adoption,
};
