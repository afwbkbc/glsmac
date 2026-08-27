const rules = #include('orbital_rules');

return (game) => {
	game.set('f_orbital_is_facility', rules.is_orbital);
	game.set(
		'f_orbital_has_full_access',
		(base) => { return rules.has_full_access(game, base); }
	);
	game.set(
		'f_orbital_get_base_resource_bonus',
		(base, resource) => { return rules.get_base_resource_bonus(game, base, resource); }
	);
	game.set(
		'f_orbital_get_marginal_yield',
		(player, definition) => { return rules.get_marginal_yield(game, player, definition); }
	);
	game.set(
		'f_orbital_apply_launch',
		(base, definition) => { return rules.apply_launch(game, base, definition); }
	);
	game.set('f_orbital_rollback_launch', rules.rollback_launch);
};
