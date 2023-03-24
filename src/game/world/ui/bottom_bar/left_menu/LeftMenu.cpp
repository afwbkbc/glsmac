#include "LeftMenu.h"

// submenus
#include "GameMenu.h"
#include "MapMenu.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

LeftMenu::LeftMenu( World* world )
	: Menu( world )
{
	{ NEWV( game_menu, GameMenu, m_world ); AddSubMenu( "GAME", game_menu ); }
	{ NEWV( map_menu, MapMenu, m_world ); AddSubMenu( "MAP", map_menu ); }
}

}
}
}
}
