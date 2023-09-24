#include "EditMapMenu.h"

#include "engine/Engine.h"

#include "../../popup/SaveMap.h"
#include "../../popup/LoadMap.h"

namespace task {
namespace game {
namespace ui {
namespace menu {

EditMapMenu::EditMapMenu( Game* game )
	: Menu( game ) {
	AddItem(
		"Save Map...", MH( this ) {
			NEWV( popup, popup::SaveMap, m_game );
			popup->Open();
			return true;
		}
	);
	AddItem(
		"Load Map...", MH( this ) {
			NEWV( popup, popup::LoadMap, m_game );
			popup->Open();
			return true;
		}
	);
}

}
}
}
}
