#include "LeftMenu.h"

#include "task/game/Game.h"

// submenus
#include "GameMenu.h"
#include "EditMapMenu.h"

#ifdef DEBUG

#include "DebugMenu.h"

#endif

namespace task {
namespace game {
namespace ui {
namespace menu {

LeftMenu::LeftMenu( Game* game )
	: Menu( game, "BBLeftMenu" ) {
#define x( _n, _m ) { \
        NEWV( menu, _m, m_game ); \
        AddSubMenu( _n, menu ); \
    }
	x( "GAME", GameMenu );
	if ( m_game->IsMapEditingAllowed() ) {
		x( "EDIT MAP", EditMapMenu );
	}
#ifdef DEBUG
	// TODO: unfinished
	//x( "DEBUG", DebugMenu );
#endif
#undef x

	m_config.use_slide_animation = true;
}

}
}
}
}
