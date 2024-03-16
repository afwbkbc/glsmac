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
	MT_Response Connect( const std::string& remote_address, MT_CANCELABLE ) override;
	MT_Response Disconnect() override;
	MT_Response DisconnectClient( const network::cid_t cid ) override;
	void ProcessEvents() override;

private:
	// true on success, false on error
	bool ReadFromSocket( remote_socket_data_t& socket );
	bool WriteToSocket( int fd, const std::string& data );
	bool MaybePing( remote_socket_data_t& socket );
	bool MaybePingDo( remote_socket_data_t& socket );
	void CloseSocket( int fd, network::cid_t cid = 0, bool skip_event = false );
	void CloseClientSocket( const remote_socket_data_t& socket );

#ifdef DEBUG
	bool m_need_pings = true;
#endif
};

}
}
