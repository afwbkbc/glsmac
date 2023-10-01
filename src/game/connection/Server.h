#pragma once

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Server, Connection )

	Server( LocalSettings* const settings );

	std::function< void() > m_on_listen = nullptr;

	void UpdateSlot( const size_t slot_num, const Slot* slot ) override;
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
};

}
}
