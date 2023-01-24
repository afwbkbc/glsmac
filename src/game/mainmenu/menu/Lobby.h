#pragma once

#include "../PopupMenu.h"

#include "ui/object/Section.h"

#include "base/MTModule.h"

namespace game {
namespace mainmenu {

CLASS( Lobby, PopupMenu )

	Lobby( MainMenu* mainmenu );

	void Show();
	void Hide();
	void Iterate();

protected:

	bool OnCancel();
	
	Section* m_map_settings_section = nullptr;
	Section* m_players_section = nullptr;
		unordered_map< size_t, Label* > m_player_labels = {};
		Button* m_launch_button = nullptr;
		Button* m_cancel_button = nullptr;
	Section* m_chat_section = nullptr;
	Section* m_game_settings_section = nullptr;
	
	
	mt_id_t m_getevents_mt_id = 0;
	
	unordered_map< size_t, string > m_player_names = {};
	
private:
	void RefreshUI();
	
	GlobalSettings m_settings_backup;
	
	
};

}
}
