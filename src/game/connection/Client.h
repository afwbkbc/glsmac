#pragma once

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Client, Connection )

	Client( LocalSettings* const settings );

	std::function< void() > m_on_players_list_update = nullptr;
	std::function< void( const game_state_t state ) > m_on_game_state_change = nullptr;

	void UpdateSlot( const size_t slot_num, const Slot* slot ) override;
	void Message( const std::string& message ) override;

	void ResetHandlers() override;

protected:
	void ProcessEvent( const network::Event& event ) override;

private:
	void Error( const std::string& reason );

};

}
}
