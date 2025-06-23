#pragma once

#include <unordered_map>
#include <string>

#include "Connection.h"

namespace game {
namespace backend {

namespace slot {
class Slot;
}

namespace connection {

class Server : public Connection {
public:

	Server( gc::Space* const gc_space, settings::LocalSettings* const settings );

	std::function< void() > m_on_listen = nullptr;
	std::function< const std::string() > m_on_download_request = nullptr; // return serialized snapshot of world

	void SendGameEventResponse( const size_t cid, const std::string& event_id, const bool result );

	void UpdateSlot( const size_t slot_num, slot::Slot* slot, const bool only_flags = false ) override;
	void SendMessage( const std::string& message ) override;

	void ResetHandlers() override;

	void UpdateGameSettings();
	void GlobalMessage( const std::string& message );
	void KickFromSlot( const size_t slot_num, const std::string& reason = "Kicked by host" );
	void BanFromSlot( const size_t slot_num, const std::string& reason = "Banned by host" );

	void SetGameState( const game_state_t game_state );
	void SendPlayersList();

protected:
	void ProcessEvent( const network::LegacyEvent& event ) override;
	void SendGameEvents( const game_events_t& game_events ) override;

private:
	void Broadcast( std::function< void( const network::cid_t cid ) > callback );
	void Kick( const network::cid_t cid, const std::string& reason );
	void KickFromSlot( slot::Slot& slot, const std::string& reason );
	void Error( const network::cid_t cid, const std::string& reason );
	void SendGlobalSettings( const network::cid_t cid );
	void SendGameState( const network::cid_t cid );
	void SendPlayersList( const network::cid_t cid, const size_t slot_num = 0 );
	void SendSlotUpdate( const size_t slot_num, const slot::Slot* slot, network::cid_t skip_cid = 0 );
	void SendFlagsUpdate( const size_t slot_num, const slot::Slot* slot, network::cid_t skip_cid = 0 );
	const std::string FormatChatMessage( const Player* player, const std::string& message ) const;

	struct download_data_t {
		size_t next_expected_offset = 0; // for extra consistency checks
		std::string serialized_snapshot = "";
	};
	std::unordered_map< network::cid_t, download_data_t > m_download_data = {}; // cid -> serialized snapshot of world

	void ClearReadyFlags();
};

}
}
}
