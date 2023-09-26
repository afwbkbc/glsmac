#pragma once

#include "../../PopupMenu.h"
#include "ui/object/Section.h"

#include "util/Timer.h"

#include "game/State.h"
#include "game/connection/Connection.h"

#include "GameSettingsSection.h"
#include "PlayersSection.h"
#include "ChatSection.h"

using namespace ::game::connection;

namespace task {
namespace mainmenu {
namespace lobby {

CLASS( Lobby, PopupMenu )

	static const char COUNTDOWN_SECONDS = 3;

	Lobby( MainMenu* mainmenu, Connection* connection );
	virtual ~Lobby();

	void Show() override;
	void Hide() override;
	void Iterate() override;

	::game::Settings& GetSettings();
	const ::game::Player* GetPlayer();

	void Message( const std::string& message );

	void UpdateSlot( const size_t slot_num, ::game::Slot* slot );
	void KickFromSlot( const size_t slot_num );
	void BanFromSlot( const size_t slot_num );
	void GlobalMessage( const std::string& message );

	void UpdateGameSettings();

	const Connection* GetConnection() const;

	void LockInput();
	void UnlockInput();

protected:

	bool OnCancel() override;

	GameSettingsSection* m_game_settings_section = nullptr;
	PlayersSection* m_players_section = nullptr;
	Button* m_ready_button = nullptr;
	Button* m_cancel_button = nullptr;
	ChatSection* m_chat_section = nullptr;
	Section* m_game_options_section = nullptr;

	::game::State* m_state = nullptr;

private:
	Connection* m_connection = nullptr;

	void ManageCountdown();
	util::Timer m_countdown_timer;
	char m_countdown = COUNTDOWN_SECONDS;
};

}
}
}
