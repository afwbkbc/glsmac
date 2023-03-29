#include "GameMenu.h"

#include "engine/Engine.h"
#include "../../../World.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

GameMenu::GameMenu( World* world )
	: Menu( world )
{
	AddItem( "Start New Game", MH( this ) {
		m_world->ConfirmExit( UH( this ) {
			m_world->ReturnToMainMenu();
		});
#ifdef DEBUG
		if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) )
			return false; // menu is already destroyed by now, can't do animation
#endif
		return true;
	});
	
	AddItem( "Quit", MH( this ) {
		m_world->ConfirmExit( UH( this ) {
			g_engine->ShutDown();
		});
#ifdef DEBUG
		if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) )
			return false; // menu is already destroyed by now, can't do animation
#endif
		return true;
	});
}

}
}
}
}
