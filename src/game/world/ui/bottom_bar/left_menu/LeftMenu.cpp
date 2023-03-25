#include "LeftMenu.h"

// submenus
#include "GameMenu.h"
#include "EditMapMenu.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

LeftMenu::LeftMenu( World* world )
	: Menu( world )
{
	#define x( _n, _m ) { \
		NEWV( menu, _m, m_world ); \
		AddSubMenu( _n, menu ); \
	}
		x( "GAME", GameMenu );
		x( "EDIT MAP", EditMapMenu );
	#undef x
}

}
}
}
}
