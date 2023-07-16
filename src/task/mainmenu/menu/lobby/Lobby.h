#pragma once

#include "../../PopupMenu.h"
#include "ui/object/Section.h"

#include "game/State.h"

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

	::game::Settings& GetSettings();
	
	void UpdatePlayer( const size_t cid, const ::game::Player& player );
	
protected:

	bool OnCancel();
	
	Section* m_map_settings_section = nullptr;
	PlayersSection* m_players_section = nullptr;
		Button* m_launch_button = nullptr;
		Button* m_cancel_button = nullptr;
	Section* m_chat_section = nullptr;
	Section* m_game_settings_section = nullptr;
	
	
	mt_id_t m_getevents_mt_id = 0;
	
	::game::State m_state;

private:
	void RefreshUI();

	bool m_is_disconnecting = false; // to prevent double-disconnect in some cases
	void Disconnect( const std::string& message = "" );

	size_t m_slot = 0;
	::game::Player* m_player = nullptr;
};

}
}
}
