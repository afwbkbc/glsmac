#include "MapMenu.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

MapMenu::MapMenu( World* world )
	: SideMenu( world )
{
	AddItem( "Save Map..." );
	AddItem( "Load Map..." );
}

}
}
}
}
