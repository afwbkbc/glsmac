#pragma once

#include "task/mainmenu/PopupMenu.h"

#include "util/Timer.h"

namespace game {
class State;
namespace slot {
class Slot;
}
class Player;
namespace connection {
class Connection;
}
namespace settings {
class Settings;
}
}

namespace ui::object {
class Button;
class Section;
}

namespace task {
namespace mainmenu {
namespace lobby {

class GameSettingsSection;
class PlayersSection;
class ChatSection;

CLASS( Lobby, PopupMenu )

	static const char COUNTDOWN_SECONDS = 3;

	Lobby( MainMenu* mainmenu, ::game::connection::Connection* connection );
	virtual ~Lobby();

	void Show() override;
	void Hide() override;
	void Iterate() override;

	::game::settings::Settings* GetSettings();
	const ::game::Player* GetPlayer();

	void Message( const std::string& message );

	void UpdateSlot( const size_t slot_num, ::game::slot::Slot* slot, const bool only_flags = false );
	void KickFromSlot( const size_t slot_num );
	void BanFromSlot( const size_t slot_num );
	void GlobalMessage( const std::string& message );

	void UpdateGameSettings();

	const ::game::connection::Connection* GetConnection() const;

	void LockInput();
	void UnlockInput();

protected:

	bool OnCancel() override;

	GameSettingsSection* m_game_settings_section = nullptr;
	PlayersSection* m_players_section = nullptr;
	ui::object::Button* m_ready_button = nullptr;
	ui::object::Button* m_cancel_button = nullptr;
	ChatSection* m_chat_section = nullptr;
	ui::object::Section* m_game_options_section = nullptr;

	::game::State* m_state = nullptr;

private:
	::game::connection::Connection* m_connection = nullptr;

	void ManageCountdown();
	util::Timer m_countdown_timer;
	char m_countdown = COUNTDOWN_SECONDS;
};

}
}
}
