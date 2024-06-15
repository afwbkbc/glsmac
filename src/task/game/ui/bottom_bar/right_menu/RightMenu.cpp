#include "RightMenu.h"

#include "task/game/ui/popup/ChatPopup.h"

namespace task {
namespace game {
namespace ui {
namespace menu {

RightMenu::RightMenu( Game* game )
	: Menu( game, "BBRightMenu" ) {

	AddItem(
		"CHAT", MH( this ) {
			NEWV( popup, popup::ChatPopup, m_game );
			popup->Open();
			return true;
		}
	);

	m_config.use_slide_animation = true;

}

}
}
}
}
