#pragma once

#include "../../PopupMenu.h"
#include "ui/object/Section.h"

#include "game/State.h"
#include "game/connection/Connection.h"

#include "PlayersSection.h"

using namespace ::game::connection;

namespace task {
namespace mainmenu {
namespace lobby {

CLASS( Lobby, PopupMenu )

	Lobby( MainMenu* mainmenu, Connection* connection );
	virtual ~Lobby();

	void Show();
	void Hide();
	void Iterate();

	::game::Settings& GetSettings();
	
	void UpdateSlot( const size_t slot_num, ::game::Slot* slot );
	void KickFromSlot( const size_t slot_num );
	void BanFromSlot( const size_t slot_num );
	
	const Connection* GetConnection() const;
	
protected:

	bool OnCancel();
	
	Section* m_map_settings_section = nullptr;
	PlayersSection* m_players_section = nullptr;
		Button* m_launch_button = nullptr;
		Button* m_cancel_button = nullptr;
	Section* m_chat_section = nullptr;
	Section* m_game_settings_section = nullptr;
	
	::game::State m_state;

private:
	Connection* m_connection = nullptr;

};

}
}
}
