#pragma once

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Server, Connection )

	Server( LocalSettings* const settings );

	void UpdateSlot( const size_t slot_num, const Slot* slot ) override;
	void Message( const std::string& message ) override;

	void UpdateGameSettings();
	void GlobalMessage( const std::string& message );
	void KickFromSlot( const size_t slot_num, const std::string& reason = "Kicked by host" );
	void BanFromSlot( const size_t slot_num, const std::string& reason = "Banned by host" );

	void SendMapGenerationPercentage( const size_t percentage );

protected:
	void ProcessEvent( const network::Event& event ) override;

private:
	void Broadcast( std::function< void( const size_t cid ) > callback );
	void Kick( const size_t cid, const std::string& reason );
	void KickFromSlot( Slot& slot, const std::string& reason );
	void Error( const size_t cid, const std::string& reason );
	void SendGlobalSettings( size_t cid );
	const std::string FormatChatMessage( const Player* player, const std::string& message ) const;
};

}
}
