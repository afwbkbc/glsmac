#pragma once

#include <sys/types.h>
#include <memory.h>

#include "network/Network.h"

// seconds
#define SEND_PING_AFTER 12
#define WAIT_PONG_FOR 6

#define DISCONNECT_AFTER ( SEND_PING_AFTER + WAIT_PONG_FOR )

namespace network {
namespace simpletcp {

CLASS( SimpleTCP, Network )

	explicit SimpleTCP( const uint16_t port = 4888 );

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
	static constexpr size_t MAX_PENDING_WRITE_SIZE = BUFFER_SIZE * 16;

	// true on success, false on error
	bool ReadFromSocket( remote_socket_data_t& socket );
	bool WriteToSocket( remote_socket_data_t& socket, const std::string& data );
	bool FlushSocketWrites( remote_socket_data_t& socket );
	bool MaybePing( remote_socket_data_t& socket );
	bool MaybePingDo( remote_socket_data_t& socket );
	void CloseSocket( fd_t fd, network::cid_t cid = 0, bool skip_event = false );
	void CloseClientSocket( const remote_socket_data_t& socket );

#ifdef DEBUG
	bool m_need_pings = true;
#endif
};

}
}
