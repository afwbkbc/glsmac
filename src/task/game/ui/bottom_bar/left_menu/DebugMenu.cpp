#ifdef DEBUG

#include "DebugMenu.h"

#include "engine/Engine.h"
#include "../../../Game.h"

namespace task {
namespace game {
namespace ui {
namespace menu {

DebugMenu::DebugMenu( Game* game )
	: Menu( game )
{
	AddItem( "Save Dump..." );
	AddItem( "Load Dump..." );
}

}
}
}
}

#endif
