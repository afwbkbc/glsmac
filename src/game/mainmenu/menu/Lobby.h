#pragma once

#include "../PopupMenu.h"

#include "ui/object/Section.h"

namespace game {
namespace mainmenu {

CLASS( Lobby, PopupMenu )

	Lobby( MainMenu* mainmenu );

	void Show();
	void Hide();

protected:
	Section* m_map_settings_section = nullptr;
	Section* m_players_section = nullptr;
	Section* m_chat_section = nullptr;
	Section* m_game_settings_section = nullptr;
	
};

}
}
