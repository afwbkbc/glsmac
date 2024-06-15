#ifdef DEBUG

#include "DebugMenu.h"

#include "engine/Engine.h"
#include "task/game/Game.h"

namespace task {
namespace game {
namespace ui {
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
