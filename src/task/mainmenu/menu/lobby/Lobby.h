#pragma once

#include "../../PopupMenu.h"

#include "ui/object/Section.h"

#include "base/MTModule.h"

#include "PlayersSection.h"

namespace task {
namespace mainmenu {
namespace lobby {

CLASS( Lobby, PopupMenu )

	Lobby( MainMenu* mainmenu );

	void Show();
	void Hide();
	void Iterate();

protected:

	bool OnCancel();
	
	Section* m_map_settings_section = nullptr;
	PlayersSection* m_players_section = nullptr;
		Button* m_launch_button = nullptr;
		Button* m_cancel_button = nullptr;
	Section* m_chat_section = nullptr;
	Section* m_game_settings_section = nullptr;
	
	
	mt_id_t m_getevents_mt_id = 0;
	
	PlayersSection::players_t m_players = {};
	
private:
	void RefreshUI();
	
	::game::GlobalSettings m_settings_backup = {};
	
	
};

}
}
}
