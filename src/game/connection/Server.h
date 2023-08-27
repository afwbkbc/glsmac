#pragma once

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Server, Connection )

	Server( LocalSettings* const settings );

	void UpdateSlot( const size_t slot_num, const Slot* slot ) override;

protected:
	void ProcessEvent( const network::Event& event ) override;

private:
	void Broadcast( std::function< void( const size_t cid ) > callback );
	void Kick( const size_t cid, const std::string& reason );
	void Error( const size_t cid, const std::string& reason );

};

}
}
