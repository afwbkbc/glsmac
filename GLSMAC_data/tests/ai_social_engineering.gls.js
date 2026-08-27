const define_social_engineering = #include('../default/game/social_engineering');
const ai_social = #include('../default/game/ai/social_engineering');

const callbacks = {};
const values = {};
const game = {
	on: (name, callback) => { callbacks[name] = callback; },
	set: (name, value) => { values[name] = value; },
};
define_social_engineering(game);
callbacks.start({});

let technologies = {};
let choices = {
	politics: 'Frontier',
	economics: 'Simple',
	values: 'Survival',
	future_society: 'None',
};
const player = {
	difficulty_level: 'Transcend',
	get_social_engineering: () => { return choices; },
	get_faction: () => { return {id: 'NEUTRAL'}; },
	has_technology: (id) => { return #is_defined(technologies[id]); },
};

const choose = (priorities) => {
	return ai_social.choose(
		player,
		values.f_social_get_categories(),
		values.f_social_get_available_choices,
		values.f_social_get_ratings_for_choices,
		priorities
	);
};

let selected = choose({
	development: 100, growth: 0, psych: 0, military: 0,
	expansion: 0, terraforming: 0,
});
test.assert(ai_social.choices_equal(selected, choices));

technologies.EthicalCalculus = true;
selected = choose({
	development: 20, growth: 100, psych: 0, military: 0,
	expansion: 100, terraforming: 0,
});
test.assert(selected.politics == 'Democratic');
test.assert(selected.economics == 'Simple');
test.assert(selected.values == 'Survival');
test.assert(selected.future_society == 'None');
test.assert(!ai_social.choices_equal(selected, choices));

const military_score = ai_social.score_ratings({
	economy: 0, effic: 0, support: 2, talent: 0, morale: 2,
	police: 2, growth: 0, planet: 0, probe: 0, industry: 0, research: 0,
}, {
	development: 20, growth: 0, psych: 0, military: 100,
	expansion: 0, terraforming: 0,
});
const peaceful_score = ai_social.score_ratings({
	economy: 2, effic: 2, support: 0, talent: 0, morale: 0,
	police: 0, growth: 0, planet: 0, probe: 0, industry: 0, research: 2,
}, {
	development: 20, growth: 0, psych: 0, military: 100,
	expansion: 0, terraforming: 0,
});
test.assert(military_score > peaceful_score);

const desired = {
	politics: 'Democratic',
	economics: 'Planned',
	values: 'Power',
	future_society: 'Cybernetic',
};
const adoption_priorities = {
	development: 50, growth: 100, psych: 25, military: 50,
	expansion: 50, terraforming: 25,
};
let adoption = ai_social.choose_adoption(
	player,
	desired,
	values.f_social_get_categories(),
	values.f_social_get_ratings_for_choices,
	adoption_priorities,
	values.f_social_get_adoption_cost,
	39
);
test.assert(ai_social.choices_equal(adoption, choices));

adoption = ai_social.choose_adoption(
	player,
	desired,
	values.f_social_get_categories(),
	values.f_social_get_ratings_for_choices,
	adoption_priorities,
	values.f_social_get_adoption_cost,
	40
);
let adoption_changes = 0;
for (category of values.f_social_get_categories()) {
	if (adoption[category.id] != choices[category.id]) {
		adoption_changes++;
	}
}
test.assert(adoption_changes == 1);
test.assert(values.f_social_get_adoption_cost(player, adoption) == 40);

player.difficulty_level = 'Citizen';
adoption = ai_social.choose_adoption(
	player,
	desired,
	values.f_social_get_categories(),
	values.f_social_get_ratings_for_choices,
	adoption_priorities,
	values.f_social_get_adoption_cost,
	0
);
test.assert(ai_social.choices_equal(adoption, desired));
