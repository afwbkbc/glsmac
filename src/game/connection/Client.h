#pragma once

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Client, Connection)

	Client( LocalSettings* const settings );

protected:
	void ProcessEvent( const network::Event& event ) override;

};

}
}
