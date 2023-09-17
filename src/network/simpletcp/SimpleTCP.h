#pragma once
#pragma once

#include <unordered_map>

#include <sys/types.h>
#include <memory.h>

#include "../Network.h"

#include "util/Timer.h"

using namespace util;

// seconds
#define SEND_PING_AFTER 12
#define WAIT_PONG_FOR 6

#define DISCONNECT_AFTER ( SEND_PING_AFTER + WAIT_PONG_FOR )

namespace network {
namespace simpletcp {

CLASS( SimpleTCP, Network )

	SimpleTCP();

	void Start() override;
	void Stop() override;
	void Iterate() override;

protected:

	MT_Response ListenStart() override;
	MT_Response ListenStop() override;
	MT_Response Connect( const std::string& remote_address ) override;
	MT_Response Disconnect() override;
	MT_Response DisconnectClient( const size_t cid ) override;
	void ProcessEvents() override;

private:

	const int32_t ptype_ping = Packet::PT_PING;

private:
	// true on success, false on error
	bool ReadFromSocket( Impl::remote_socket_data_t& socket, const size_t cid = 0 );
	bool WriteToSocket( int fd, const std::string& data );
	bool MaybePing( Impl::remote_socket_data_t& socket, const size_t cid = 0 );
	bool MaybePingDo( Impl::remote_socket_data_t& socket, const size_t cid = 0 );
	void CloseSocket( int fd, size_t cid = 0, bool skip_event = false );
};

}
}
