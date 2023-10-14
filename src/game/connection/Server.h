#pragma once

#include <unordered_map>
#include <string>

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Server, Connection )

	Server( LocalSettings* const settings );

	std::function< void() > m_on_listen = nullptr;
	std::function< const std::string() > m_on_map_request = nullptr; // return serialized Tiles

	void UpdateSlot( const size_t slot_num, Slot* slot ) override;
	void Message( const std::string& message ) override;

	void ResetHandlers() override;

	void UpdateGameSettings();
	void GlobalMessage( const std::string& message );
	void KickFromSlot( const size_t slot_num, const std::string& reason = "Kicked by host" );
	void BanFromSlot( const size_t slot_num, const std::string& reason = "Banned by host" );

	void SetGameState( const game_state_t game_state );

protected:
	void ProcessEvent( const network::Event& event ) override;

private:
	void Broadcast( std::function< void( const size_t cid ) > callback );
	void Kick( const size_t cid, const std::string& reason );
	void KickFromSlot( Slot& slot, const std::string& reason );
	void Error( const size_t cid, const std::string& reason );
	void SendGlobalSettings( const size_t cid );
	void SendGameState( const size_t cid );
	void SendSlotUpdate( const size_t slot_num, const Slot* slot, size_t skip_cid = 0 );
	const std::string FormatChatMessage( const Player* player, const std::string& message ) const;

	struct map_data_t {
		size_t next_expected_offset = 0; // for extra consistency checks
		std::string serialized_tiles = "";
	};
	std::unordered_map< size_t, map_data_t > m_map_data = {}; // cid -> serialized tiles

	void ClearReadyFlags();
};

}
}
