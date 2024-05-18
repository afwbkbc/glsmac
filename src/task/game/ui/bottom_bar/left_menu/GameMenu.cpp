#include "GameMenu.h"

#include "engine/Engine.h"
#include "task/game/Game.h"
#include "config/Config.h"

namespace task {
namespace game {
namespace ui {
namespace menu {

GameMenu::GameMenu( Game* game )
	: Menu( game, "BBLeftMenu" ) {

#ifdef DEBUG
	if ( !g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) )
#endif
	{
		AddItem(
			"Start New Game", MH( this ) {
				m_game->ConfirmExit(
					UH( this ) {
						m_game->ExitGame(
							[ this ]() -> void {
								m_game->ReturnToMainMenu();
							}
						);
					}
				);
#ifdef DEBUG
				if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
					return false;
				} // menu is already destroyed by now, can't do animation
#endif
				return true;
			}
		);
	}

	AddItem(
		"Quit", MH( this ) {
			m_game->ConfirmExit(
				UH( this ) {
					g_engine->ShutDown();
				}
			);
#ifdef DEBUG
			if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
				return false;
			} // menu is already destroyed by now, can't do animation
#endif
			return true;
		}
	);
}

}
}
}
}
