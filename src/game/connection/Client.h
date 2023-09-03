#pragma once

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Client, Connection)

	Client( LocalSettings* const settings );

	void UpdateSlot( const size_t slot_num, const Slot* slot ) override;
	void UpdateGameSettings() override;
protected:
	void ProcessEvent( const network::Event& event ) override;

private:
	void Error( const std::string& reason );
	
};

}
}
