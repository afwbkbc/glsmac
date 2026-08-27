const rules = #include('unit_upgrade_rules');

return (game) => {
	game.set('f_unit_upgrade_get_targets', (player, source) => {
		return rules.get_targets(game, player, source);
	});
	game.set('f_unit_upgrade_get_cost', (player, source, target) => {
		return rules.get_cost(game, player, source, target);
	});
	game.set('f_unit_upgrade_get_error', (unit, caller, target_id) => {
		return rules.get_error(game, unit, caller, target_id);
	});
	game.set('f_unit_upgrade_choose_ai_target', (player, unit) => {
		return rules.choose_ai_target(game, player, unit);
	});
};
