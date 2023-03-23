#include "GameMenu.h"

#include "engine/Engine.h"
#include "../../../World.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

GameMenu::GameMenu( World* world )
	: SideMenu( world )
{
#if DEBUG
	AddItem( "Save Dump..." );
	AddItem( "Load Dump..." );
#endif
	
	AddItem( "Start New Game", SMH( this ) {
		m_world->ReturnToMainMenu();
		return false;
	});
	
	AddItem( "Quit", SMH( this ) {
		g_engine->ShutDown();
		return false;
	});
}

}
}
}
}
