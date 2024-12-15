#include "EditMapMenu.h"

#include "engine/Engine.h"

#include "game/frontend/ui_legacy/popup/SaveMap.h"
#include "game/frontend/ui_legacy/popup/LoadMap.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace menu {

EditMapMenu::EditMapMenu( Game* game )
	:
	Menu(
		game,
		"BBLeftMenu"
	) {
	AddItem(
		"Save Map...", MH( this ) {
			NEWV( popup, popup::SaveMap, m_game );
			popup->
				Open();
			return true;
		}
	);

// TODO: broken
/*AddItem(
	"Load Map...", MH( this ) {
		NEWV( popup, popup::LoadMap, m_game );
		popup->Open();
		return true;
	}
);*/
}

}
}
}
}
