#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	glsmac.on('configure_state', (e) => {
		#async(100, () => {
			if (#sizeof(glsmac.game.get_players()) != 2) {
				return true;
			}
			#print('MULTIPLAYER_LOBBY_PROBE_CONNECTED');
			return false;
		});
	});

	glsmac.run();

});
