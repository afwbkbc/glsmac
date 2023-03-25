#ifdef DEBUG

#include "DebugMenu.h"

#include "engine/Engine.h"
#include "../../../World.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

DebugMenu::DebugMenu( World* world )
	: Menu( world )
{
	AddItem( "Save Dump..." );
	AddItem( "Load Dump..." );
}

}
}
}
}

#endif
