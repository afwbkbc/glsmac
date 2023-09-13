#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <unordered_map>

#include <sys/types.h>
#include <memory.h>
#include <fcntl.h>

#include "../Network.h"

#include "util/Timer.h"

using namespace util;

// should be sufficient to fit any packet
#define BUFFER_SIZE 65536

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
	
	struct local_socket_data_t {
		std::string local_address;
		int fd;
	};
	
	struct remote_socket_data_t {
		std::string remote_address;
		int fd;
		struct {
			char* data1;
			char* data2;
			char* data;
			char* ptr;
			size_t len;
		} buffer;
		time_t last_data_at;
		bool ping_needed;
		bool ping_sent;
		bool pong_needed;
	};
	
	struct {
		std::unordered_map< int, local_socket_data_t > listening_sockets;
		std::unordered_map< int, remote_socket_data_t > client_sockets;
		struct {
			struct sockaddr_in client_addr;
#ifndef _WIN32
			socklen_t client_addr_size;
#endif
			int newfd;
			std::vector< int > to_remove;
		} tmp;
	} m_server = {};
	
	struct {
		remote_socket_data_t socket;
	} m_client = {};
	
	struct {
		int tmpint;
		int tmpint2;
		Event event;
		char* ptr;
		time_t now;
		time_t time;
		char c;
	} m_tmp;

private:
	// true on success, false on error
	bool ReadFromSocket( remote_socket_data_t& socket, const size_t cid = 0 );
	bool WriteToSocket( int fd, const std::string& data );
	bool MaybePing( remote_socket_data_t& socket, const size_t cid = 0 );
	bool MaybePingDo( remote_socket_data_t& socket, const size_t cid = 0 );
	void CloseSocket( int fd, size_t cid = 0, bool skip_event = false );
};

}
}
