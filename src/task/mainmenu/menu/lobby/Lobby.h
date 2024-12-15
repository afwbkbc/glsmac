#pragma once

#include "task/mainmenu/PopupMenu.h"

#include "util/Timer.h"

namespace game {
namespace backend {
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
}

namespace ui_legacy::object {
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

	Lobby( MainMenu* mainmenu, ::game::backend::connection::Connection* connection );
	virtual ~Lobby();

	void Show() override;
	void Hide() override;
	void Iterate() override;

	::game::backend::settings::Settings* GetSettings();
	const ::game::backend::Player* GetPlayer();
	::game::backend::State* GetState() const;

	void Message( const std::string& message );

	void UpdateSlot( const size_t slot_num, ::game::backend::slot::Slot* slot, const bool only_flags = false );
	void KickFromSlot( const size_t slot_num );
	void BanFromSlot( const size_t slot_num );
	void GlobalMessage( const std::string& message );

	void UpdateGameSettings();

	const ::game::backend::connection::Connection* GetConnection() const;

	void LockInput();
	void UnlockInput();

protected:

	bool OnCancel() override;

	GameSettingsSection* m_game_settings_section = nullptr;
	PlayersSection* m_players_section = nullptr;
	ui_legacy::object::Button* m_ready_button = nullptr;
	ui_legacy::object::Button* m_cancel_button = nullptr;
	ChatSection* m_chat_section = nullptr;
	ui_legacy::object::Section* m_game_options_section = nullptr;

	::game::backend::State* m_state = nullptr;

private:
	::game::backend::connection::Connection* m_connection = nullptr;

	void ManageCountdown();
	util::Timer m_countdown_timer;
	char m_countdown = COUNTDOWN_SECONDS;
};

}
}
}
