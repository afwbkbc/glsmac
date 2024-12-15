#ifdef DEBUG

#include "DebugMenu.h"

#include "engine/Engine.h"
#include "game/frontend/Game.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace menu {

DebugMenu::DebugMenu( Game* game )
	: Menu( game, "BBLeftMenu" ) {
	AddItem( "Save Dump..." );
	AddItem( "Load Dump..." );
}

}
}
}
}

#endif
