return (i) => {
	i.glsmac.deinit();
	i.sliding.show({
		entries: [
			['Start Game', () => {
				i.glsmac.init();
				i.settings.local.game_mode = 'single';
				i.steps.start_game(i);
			}],
			['Quick Start', () => {
				i.glsmac.init();
				i.settings.local.game_mode = 'single';
				i.randomize_map();
				i.settings.global.difficulty_level = 'Citizen';
				i.glsmac.add_single_player();
				i.glsmac.game.event('select_faction', {faction: 'GAIANS'});
				for (let opponent = 0; opponent < 6; opponent++) {
					i.glsmac.add_ai_player();
				}
				i.glsmac.start_game();
			}],
			['Scenario', () => {
				i.steps.notimpl(i);
			}],
			['Load Game', () => {
				i.steps.notimpl(i);
			}],
			['Multiplayer', () => {
				i.glsmac.init();
				i.settings.local.game_mode = 'multi';
				i.steps.multiplayer_type(i);
			}],
			['View Credits', () => {
				i.steps.notimpl(i);
			}],
			['Exit Game', () => {
				i.glsmac.exit();
			}],
		]
	});
};
