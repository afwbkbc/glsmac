#pragma once

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Server, Connection )

	Server( LocalSettings* const settings );

protected:
	void ProcessEvent( const network::Event& event ) override;

private:
	void Kick( const size_t cid, const std::string& reason );

};

}
}
