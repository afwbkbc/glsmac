#include "LeftMenu.h"

// submenus
#include "GameMenu.h"
#include "EditMapMenu.h"
#ifdef DEBUG
#include "DebugMenu.h"
#endif

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
#ifdef DEBUG
		x( "DEBUG", DebugMenu );
#endif
	#undef x

	m_config.use_slide_animation = true;
}

}
}
}
}
