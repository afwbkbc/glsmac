#include "GameMenu.h"

#include "engine/Engine.h"
#include "game/frontend/Game.h"
#include "config/Config.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace menu {

GameMenu::GameMenu( Game* game )
	: Menu( game, "BBLeftMenu" ) {

	if ( !g_engine->GetConfig()->HasLaunchFlag( config::Config::LF_QUICKSTART ) ) {
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
				if ( g_engine->GetConfig()->HasLaunchFlag( config::Config::LF_QUICKSTART ) ) {
					return false;
				} // menu is already destroyed by now, can't do animation
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
			if ( g_engine->GetConfig()->HasLaunchFlag( config::Config::LF_QUICKSTART ) ) {
				return false;
			} // menu is already destroyed by now, can't do animation
			return true;
		}
	);
}

}
}
}
}
