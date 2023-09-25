#include <thread>

#ifdef _WIN32
#include <ws2tcpip.h>
#else

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#endif

#include "SimpleTCP.h"

namespace network {
namespace simpletcp {

static socklen_t sockaddr_in_size = sizeof( struct sockaddr_in );

SimpleTCP::SimpleTCP()
	: Network() {

}

void SimpleTCP::Start() {
	m_impl.Start();
}

void SimpleTCP::Stop() {
	m_impl.Stop();
	ASSERT( m_server.listening_sockets.empty(), "some connection(s) still active when destroying network" );
}

MT_Response SimpleTCP::ListenStart() {

	ASSERT( m_server.listening_sockets.empty(), "some connection socket(s) already active" );

	Log( (std::string)"Starting server on port " + std::to_string( GLSMAC_PORT ) );

	addrinfo hints, * res, * p;
	memset( &hints, 0, sizeof( hints ) );

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	m_tmp.tmpint = getaddrinfo( nullptr, std::to_string( GLSMAC_PORT ).c_str(), &hints, &res );
	if ( m_tmp.tmpint != 0 ) {
		return Error( (std::string)"Failed to getaddrinfo: " + gai_strerror( m_tmp.tmpint ) );
	}

	unsigned int addr_i = 0;
	char ip_str[INET6_ADDRSTRLEN];

	for ( p = res ; p != nullptr ; p = p->ai_next ) {
		void* addr;
		std::string ip_ver;

		if ( p->ai_family == AF_INET ) {
			ip_ver = "IPv4";
			sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>( p->ai_addr );
			addr = &( ipv4->sin_addr );
			++addr_i;
		}
		else if ( p->ai_family == AF_INET6 ) {
			ip_ver = "IPv6";
			sockaddr_in6* ipv6 = reinterpret_cast<sockaddr_in6*>( p->ai_addr );
			addr = &( ipv6->sin6_addr );
			++addr_i;
		}
		inet_ntop( p->ai_family, addr, ip_str, sizeof( ip_str ) );

		local_socket_data_t socket_data;
		socket_data.local_address = ip_str;

		Log( "Opening listening socket on " + socket_data.local_address );
		socket_data.fd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
		if ( socket_data.fd == -1 ) {
			Log( "Failed to creating listening socket on " + socket_data.local_address );
			continue; // don't fail right away, maybe we can open on some other interface // TODO: show warning to user?
		}

		m_impl.ConfigureSocket( socket_data.fd );

		m_tmp.tmpint = bind( socket_data.fd, p->ai_addr, p->ai_addrlen );
		if ( m_tmp.tmpint ) {
			Log( "Failed to bind to listening socket on " + socket_data.local_address );
			CloseSocket( socket_data.fd, 0, true );
			continue;
		}

		m_tmp.tmpint = listen( socket_data.fd, GLSMAC_MAX_INCOMING_CONNECTIONS );
		if ( m_tmp.tmpint == -1 ) {
			Log( "Failed to listen on " + socket_data.local_address );
			CloseSocket( socket_data.fd, 0, true );
			continue;
		}

		// TODO: for some reason there are no errors if port is already taken by other process, need to check it explicitly before listening

		ASSERT( m_server.listening_sockets.find( socket_data.fd ) == m_server.listening_sockets.end(), "duplicate listening socket id" );
		m_server.listening_sockets[ socket_data.fd ] = socket_data;
	}

	freeaddrinfo( res );

	if ( m_server.listening_sockets.empty() ) {
		return Error( "Failed to listen on port " + std::to_string( GLSMAC_PORT ) );
	}

	m_tmp.event.Clear();
	m_tmp.event.type = Event::ET_LISTEN;
	m_tmp.event.cid = 0; // server always has cid 0
	AddEvent( m_tmp.event );

	Log( "Server started" );

	return Success();

}

MT_Response SimpleTCP::ListenStop() {
	ASSERT( GetCurrentConnectionMode() == CM_SERVER, "ListenStop() on non-server" );

	Log( "Closing all connection(s)" );
	for ( auto& it : m_server.client_sockets ) {
		CloseClientSocket( it.second );
	}
	m_server.client_sockets.clear();

	Log( "Closing listening socket(s)" );
	for ( auto& socket : m_server.listening_sockets ) {
		CloseSocket( socket.second.fd );
	}
	m_server.listening_sockets.clear();

	ASSERT( !m_client.socket.fd, "client socket is open in server mode" );

	Log( "Server stopped" );

	return Success();
}

MT_Response SimpleTCP::Connect( const std::string& remote_address, MT_CANCELABLE ) {

	ASSERT( m_client.socket.fd == 0, "connection socket already active" );

	Log( (std::string)"Connecting to " + remote_address + " port " + std::to_string( GLSMAC_PORT ) );

	addrinfo hints, * p;
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	m_tmp.tmpint = getaddrinfo( remote_address.c_str(), std::to_string( GLSMAC_PORT ).c_str(), &hints, &p );
	if ( m_tmp.tmpint != 0 ) {
		return Error( (std::string)"Failed to getaddrinfo: " + gai_strerror( m_tmp.tmpint ) );
	}

	if ( p == nullptr ) {
		freeaddrinfo( p );
		return Error( "No addresses found" );
	}

	m_client.socket.fd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
	if ( m_impl.IsSocketInvalid( m_client.socket.fd ) ) {
		freeaddrinfo( p );
		m_client.socket.fd = 0;
		return Error( "Socket failed with error: " + m_impl.GetErrorMessage( m_impl.GetLastErrorCode() ) );
	}

	const auto error = [ this, p ]( const std::string& message ) -> const MT_Response {
		CloseSocket( m_client.socket.fd, 0, true );
		m_client.socket.fd = 0;
		freeaddrinfo( p );
		return Error( message );
	};

	m_impl.ConfigureSocket( m_client.socket.fd );

	m_tmp.tmpint2 = 100; // max attempts (100*20ms = 2s)
	unsigned long l = 0;
	while ( ( connect( m_client.socket.fd, p->ai_addr, p->ai_addrlen ) ) == -1 ) {

		if ( MT_C ) {
			// canceled by user
			return Canceled();
		}

		m_tmp.tmpint = m_impl.GetLastErrorCode();

		if ( m_impl.IsConnectionSucceeded( m_tmp.tmpint ) ) {
			break;
		}
		else if ( m_impl.IsConnectionIdle( m_tmp.tmpint ) && m_tmp.tmpint2 > 0 ) {
			m_tmp.tmpint2--;
			std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
		}
		else {
			return error( "Connection failed: " + m_impl.GetErrorMessage( m_tmp.tmpint ) );
		}

	}

	if ( p->ai_family == AF_INET ) {
		struct sockaddr_in* psai = (struct sockaddr_in*)p->ai_addr;
		char ip[INET_ADDRSTRLEN];
		if ( inet_ntop( p->ai_family, &( psai->sin_addr ), ip, INET_ADDRSTRLEN ) != NULL ) {
			m_client.socket.remote_address = ip;
		}
	}
	else if ( p->ai_family == AF_INET6 ) {
		struct sockaddr_in6* psai = (struct sockaddr_in6*)p->ai_addr;
		char ip[INET6_ADDRSTRLEN];
		if ( inet_ntop( p->ai_family, &( psai->sin6_addr ), ip, INET6_ADDRSTRLEN ) != NULL ) {
			m_client.socket.remote_address = ip;
		}
	}
	else {
		return error( "Unsupported IP type: " + remote_address );
	}

	m_client.socket.buffer.len = 0;
	m_client.socket.buffer.data1 = (char*)malloc( BUFFER_SIZE );
	m_client.socket.buffer.data2 = (char*)malloc( BUFFER_SIZE );
	m_client.socket.buffer.data = m_client.socket.buffer.data1;
	m_client.socket.buffer.ptr = m_client.socket.buffer.data;
	m_client.socket.last_data_at = time( nullptr );
	m_client.socket.ping_needed = false;
	m_client.socket.pong_needed = false;
	m_client.socket.ping_sent = false;

	Log( "Connection successful" );

	return Success();
}

MT_Response SimpleTCP::Disconnect() {

	if ( m_client.socket.fd ) {
		CloseSocket( m_client.socket.fd, 0, true ); // no need to send event if disconnect was initiated by user
		free( m_client.socket.buffer.data1 );
		free( m_client.socket.buffer.data2 );
		m_client.socket.fd = 0;
	}

	return Success();
}

MT_Response SimpleTCP::DisconnectClient( const size_t cid ) {

	if ( GetCurrentConnectionMode() != CM_SERVER ) {
		Log( "WARNING: DisconnectClient() on non-server" );
		return Success();
	}

	const auto fd = GetFdFromCid( cid );
	ASSERT( fd, "could not find client fd" );
	auto it = m_server.client_sockets.find( fd );
	if ( it != m_server.client_sockets.end() ) {
		CloseClientSocket( it->second );
		m_server.client_sockets.erase( it );
	}
	return Success();
}

void SimpleTCP::ProcessEvents() {
	// process events
	auto events = GetEvents();
	for ( auto& event : events ) {
		switch ( event.type ) {
			case Event::ET_PACKET: {
				Log( "Packet event ( cid = " + std::to_string( event.cid ) + " )" );
				if ( event.cid ) { // presence of cid means we are server
					if ( GetCurrentConnectionMode() != CM_SERVER ) {
						Log( "WARNING: got non-zero cid in packet while not being server, ignoring" );
						break;
					}
					m_tmp.tmpint = 0;
					const auto fd = GetFdFromCid( event.cid );
					ASSERT( fd, "event fd not found for cid " + std::to_string( event.cid ) );
					auto it = m_server.client_sockets.find( fd );
					if ( it != m_server.client_sockets.end() ) { // if not found it may mean event is old so can be ignored
						if ( !WriteToSocket( it->second.fd, event.data.packet_data ) ) {
							CloseClientSocket( it->second );
							m_server.client_sockets.erase( it );
						}
					}
				}
				else if ( m_client.socket.fd ) {
					if ( !WriteToSocket( m_client.socket.fd, event.data.packet_data ) ) {
						CloseSocket( m_client.socket.fd );
						free( m_client.socket.buffer.data1 );
						free( m_client.socket.buffer.data2 );
						m_client.socket.fd = 0;
					}
				}
				break;
			}
			case Event::ET_DISCONNECT: {
				Log( "Disconnect event" );
				switch ( GetCurrentConnectionMode() ) {
					case CM_NONE: {
						// not connected, nothing to do
						break;
					}
					case CM_SERVER: {
						auto response = ListenStop();
						ASSERT( response.result == R_SUCCESS, "failed to stop listening" );
						break;
					}
					case CM_CLIENT: {
						auto response = Disconnect();
						ASSERT( response.result == R_SUCCESS, "failed to disconnect" );
						break;
					}
					default: {
						ASSERT( false, "invalid mode on disconnect" );
					}
				}
				break;
			}
			case Event::ET_CLIENT_DISCONNECT: {
				Log( "Disconnect client event ( cid = " + std::to_string( event.cid ) + " )" );
				auto response = DisconnectClient( event.cid );
				ASSERT( response.result == R_SUCCESS, "failed to disconnect client" );
				break;
			}
			default: {
				// ignore for now
			}
		}
	}
}

void SimpleTCP::Iterate() {
	Network::Iterate();

	m_tmp.now = time( nullptr );

	// accept new connections (server)
	if ( !m_server.listening_sockets.empty() ) {
		// Log( "Checking for connections" ); // SPAMMY
		for ( auto& it : m_server.listening_sockets ) {

			m_server.tmp.newfd = accept( it.first, (sockaddr*)&m_server.tmp.client_addr, &sockaddr_in_size );
			if ( m_server.tmp.newfd != -1 ) {

				Log( "Accepting connection " + std::to_string( m_server.tmp.newfd ) );

				m_impl.ConfigureSocket( m_server.tmp.newfd );

				remote_socket_data_t data;
				data.buffer.len = 0;
				data.buffer.data1 = (char*)malloc( BUFFER_SIZE );
				data.buffer.data2 = (char*)malloc( BUFFER_SIZE );
				data.buffer.data = data.buffer.data1;
				data.buffer.ptr = data.buffer.data;
				data.fd = m_server.tmp.newfd;

				data.remote_address = inet_ntoa( ( (struct sockaddr_in*)&m_server.tmp.client_addr )->sin_addr );

				data.last_data_at = m_tmp.now;
				data.ping_needed = false;
				data.pong_needed = false;
				data.ping_sent = false;
				// find free cid
				if ( m_server.cid_to_fd.empty() ) {
					m_server.cid_to_fd.push_back( 0 ); // reserve zero cid for server
				}
				for ( data.cid = 1 ; data.cid < m_server.cid_to_fd.size() ; data.cid++ ) {
					if ( m_server.cid_to_fd[ data.cid ] == 0 ) {
						// free cid found
						m_server.cid_to_fd[ data.cid ] = data.fd;
						break;
					}
				}
				if ( data.cid == m_server.cid_to_fd.size() ) {
					// no free cid found, append to the end
					m_server.cid_to_fd.push_back( data.fd );
				}

				ASSERT( m_server.client_sockets.find( data.fd ) == m_server.client_sockets.end(), "client socket already added" );
				m_server.client_sockets[ data.fd ] = data;

				Log( "Accepted connection from " + data.remote_address + " (cid " + std::to_string( data.cid ) + ")" );

				m_tmp.event.Clear();
				m_tmp.event.type = Event::ET_CLIENT_CONNECT;
				m_tmp.event.data.remote_address = data.remote_address;
				m_tmp.event.cid = data.cid;

				AddEvent( m_tmp.event );
			}
		}
	}

	// process pings
	for ( auto it = m_server.client_sockets.begin() ; it != m_server.client_sockets.end() ; ) {
		if ( !MaybePingDo( it->second ) ) {
			CloseClientSocket( it->second );
			m_server.client_sockets.erase( it++ );
		}
		else {
			++it;
		}
	}
	if ( m_client.socket.fd ) {
		if ( !MaybePingDo( m_client.socket ) ) {
			CloseSocket( m_client.socket.fd );
			free( m_client.socket.buffer.data1 );
			free( m_client.socket.buffer.data2 );
			m_client.socket.fd = 0;
		}
	}

	// read from connection (client)
	if ( m_client.socket.fd ) {
		if ( !ReadFromSocket( m_client.socket ) ) {
			CloseSocket( m_client.socket.fd );
			free( m_client.socket.buffer.data1 );
			free( m_client.socket.buffer.data2 );
			m_client.socket.fd = 0;
		}
	}

	// read from connections (server)
	for ( auto it = m_server.client_sockets.begin() ; it != m_server.client_sockets.end() ; ) {
		if ( !ReadFromSocket( it->second ) ) {
			CloseClientSocket( it->second );
			m_server.client_sockets.erase( it++ );
		}
		else {
			++it;
		}
	}
}

bool SimpleTCP::ReadFromSocket( remote_socket_data_t& socket ) {

	// max allowed size to read
	m_tmp.tmpint = ( BUFFER_SIZE - socket.buffer.len );

	//Log( "Reading up to " + std::to_string( m_tmp.tmpint ) + " bytes from " + std::to_string( socket.fd ) + " (buffer=" + std::to_string( (long int) socket.buffer.ptr ) + ", BUFFER_SIZE=" + std::to_string( BUFFER_SIZE ) + " buffer len = " + std::to_string( socket.buffer.len ) + ")" );
	m_tmp.tmpint2 = m_impl.Receive( socket.fd, socket.buffer.ptr, m_tmp.tmpint );

	if ( m_tmp.tmpint2 < 0 ) {
		m_tmp.tmpint = m_impl.GetLastErrorCode();
		if ( m_impl.IsConnectionIdle( m_tmp.tmpint ) ) {
			// no pending data
			if ( !MaybePing( socket ) ) {
				return false;
			}
		}
		else {
			Log( "Connection failed (result=" + std::to_string( m_tmp.tmpint2 ) + " code=" + std::to_string( m_tmp.tmpint ) + ")" );
			return false;
		}
	}

	if ( m_tmp.tmpint2 == 0 ) {
		// no pending data
		if ( !MaybePing( socket ) ) {
			return false;
		}
	}

	if ( m_tmp.tmpint2 > 0 ) {

		Log( "Read " + std::to_string( m_tmp.tmpint2 ) + " bytes into buffer " + std::to_string( (long long)socket.buffer.ptr ) + " (size=" + std::to_string( socket.buffer.len ) + ")" );

		socket.last_data_at = m_tmp.now;
		socket.ping_needed = false;
		socket.pong_needed = false;

		socket.buffer.len += m_tmp.tmpint2;
		socket.buffer.ptr += m_tmp.tmpint2;

	}

	{

		if ( socket.buffer.len == 0 ) {
			return true; // no new data
		}

		//Log( "Trying to process buffer " + to_string( (long int) socket.buffer.data ) + " ( size=" + to_string( socket.buffer.len ) + " )" );
		if ( socket.buffer.len < sizeof( m_tmp.tmpint ) ) {
			//Log( "Buffer " + to_string( (long int) socket.buffer.data ) + " does not contain full size field yet ( " + to_string( socket.buffer.len ) + " < " + to_string( sizeof( m_tmp.tmpint ) ) + " )" );
			return true;
		}

		m_tmp.ptr = socket.buffer.data;
		memcpy( &m_tmp.tmpint, m_tmp.ptr, sizeof( m_tmp.tmpint ) );
		m_tmp.tmpint2 = sizeof( m_tmp.tmpint ) + m_tmp.tmpint;

		if ( socket.buffer.len < m_tmp.tmpint2 ) {
			//Log( "Buffer " + std::to_string( (long int) socket.buffer.data ) + " does not contain all data yet ( " + std::to_string( socket.buffer.len ) + " < " + std::to_string( m_tmp.tmpint2 ) + " )" );
			return true;
		}
		else {
			//Log( "Buffer " + std::to_string( (long int) socket.buffer.data ) + " contains enough data ( " + std::to_string( socket.buffer.len ) + " >= " + std::to_string( m_tmp.tmpint2 ) + " )" );
		}

		//Log( "Processing buffer " + to_string( (long int) m_tmp.ptr ) + " ( size=" + to_string( m_tmp.tmpint2 ) + " )" );

		if ( m_tmp.tmpint == 0 && m_tmp.tmpint2 == sizeof( m_tmp.tmpint ) ) {
			// zero length means 'bye'
			Log( "Connection closed by remote host" );
			return false;
		}

		m_tmp.ptr += sizeof( m_tmp.tmpint );

		{
			//Log( "Read packet (" + std::to_string( m_tmp.tmpint ) + " bytes)" );
			m_tmp.event.Clear();
			m_tmp.event.cid = socket.cid;
			m_tmp.event.data.remote_address = socket.remote_address;
			m_tmp.event.data.packet_data = std::string( m_tmp.ptr, m_tmp.tmpint );
			try {
				Packet p;
				p.Unserialize( Buffer( m_tmp.event.data.packet_data ) );
				// quick hack to respond to pings without escalating events outside
				// TODO: refactor
				if ( p.type == Packet::PT_PING ) {
					Log( "Ping received" );
					socket.pong_needed = true;
				}
				else if ( p.type == Packet::PT_PONG ) {
					Log( "Pong received" );
					socket.ping_sent = false;
				}
				else {
					//Log( "Sending event" );
					m_tmp.event.type = Event::ET_PACKET;
					AddEvent( m_tmp.event );
				}
			}
			catch ( std::runtime_error& err ) {
				m_tmp.event.type = Event::ET_ERROR;
				m_tmp.event.data.packet_data = err.what();
				AddEvent( m_tmp.event );
			}

		}

		Log( "Processed " + std::to_string( m_tmp.tmpint2 ) + " bytes" );

		if ( m_tmp.tmpint2 > 0 ) {
			ASSERT( socket.buffer.len >= m_tmp.tmpint2, "processed more than total" );
			socket.buffer.len -= m_tmp.tmpint2;
			if ( socket.buffer.len > 0 ) {
				if ( socket.buffer.data == socket.buffer.data1 ) {
					//Log( "SWAP data1 -> data2" );
					memcpy( socket.buffer.data2, socket.buffer.data + m_tmp.tmpint2, socket.buffer.len );
					socket.buffer.data = socket.buffer.data2;
				}
				else {
					//Log( "SWAP data2 -> data1" );
					memcpy( socket.buffer.data1, socket.buffer.data + m_tmp.tmpint2, socket.buffer.len );
					socket.buffer.data = socket.buffer.data1;
				}
			}
			socket.buffer.ptr = socket.buffer.data + socket.buffer.len;

			//Log( "Buffer size changed to " + to_string( socket.buffer.len ) );
		}

		return true;

	}
	ASSERT( false, "?" );
	return false;
}

bool SimpleTCP::WriteToSocket( int fd, const std::string& data ) {
	//Log( "WriteToSocket( " + to_string( fd ) + " )" ); // SPAMMY
	m_tmp.tmpint2 = data.size();
	//Log( "Writing " + to_string( m_tmp.tmpint2 ) + " bytes" );
	// send size
	m_tmp.tmpint = m_impl.Send( fd, &m_tmp.tmpint2, sizeof( m_tmp.tmpint2 ) );
	if ( m_tmp.tmpint <= 0 ) {
		m_tmp.tmpint = m_impl.GetLastErrorCode();
		if ( m_impl.IsConnectionIdle( m_tmp.tmpint ) ) {
			return true; // no data but connection is alive
		}
		Log( "Error writing size to socket (errno=" + std::to_string( m_tmp.tmpint ) + " size=" + std::to_string( m_tmp.tmpint ) + " reqsize=" + std::to_string( m_tmp.tmpint2 ) + ")" );

		return false;
	}
	// send data
	m_tmp.tmpint = m_impl.Send( fd, data.data(), data.size() );
	if ( m_tmp.tmpint2 <= 0 ) {
		if ( errno == EAGAIN ) {
			return true; // no data but connection is alive
		}
		Log( "Error writing data to socket" );
		return false;
	}
	//Log( "Write successful" );
	return true;
}

bool SimpleTCP::MaybePing( remote_socket_data_t& socket ) {
	m_tmp.time = m_tmp.now - socket.last_data_at;

	if ( m_tmp.time > SEND_PING_AFTER && !socket.ping_sent ) {
		Log( "Need ping" );
		socket.ping_needed = true;
	}

	return true;
}

bool SimpleTCP::MaybePingDo( remote_socket_data_t& socket ) {
	m_tmp.time = m_tmp.now - socket.last_data_at;

	if ( m_tmp.time > DISCONNECT_AFTER ) {
		Log( "Ping timeout on " + std::to_string( socket.fd ) + " (cid " + std::to_string( socket.cid ) + ")" );

		return false;
	}

	if ( socket.ping_needed && !socket.ping_sent ) {
		Log( "Sending ping to " + std::to_string( socket.fd ) + " (cid " + std::to_string( socket.cid ) + ")" );
		Packet packet;
		packet.type = Packet::PT_PING;
		std::string data = packet.Serialize().ToString();
		socket.ping_sent = true;
		return WriteToSocket( socket.fd, data );
	}
	if ( socket.pong_needed ) {
		Log( "Ping received, sending pong to " + std::to_string( socket.fd ) + " (cid " + std::to_string( socket.cid ) + ")" );
		Packet packet;
		packet.type = Packet::PT_PONG;
		socket.pong_needed = false;
		std::string data = packet.Serialize().ToString();
		return WriteToSocket( socket.fd, data );
	}

	return true;
}

void SimpleTCP::CloseSocket( int fd, size_t cid, bool skip_event ) {
	Log( "Closing socket " + std::to_string( fd ) );
	uint32_t bye = 0;
	m_impl.Send( fd, &bye, sizeof( bye ) );
	m_impl.CloseSocket( fd );
	if ( !skip_event ) {
		m_tmp.event.Clear();
		if ( cid ) {
			m_tmp.event.type = Event::ET_CLIENT_DISCONNECT;
			m_tmp.event.cid = cid;
		}
		else {
			m_tmp.event.type = Event::ET_DISCONNECT;
		}
		AddEvent( m_tmp.event );
	}
}

void SimpleTCP::CloseClientSocket( const remote_socket_data_t& socket ) {
	ASSERT( GetCurrentConnectionMode() == CM_SERVER, "can't close client socket as non-server" );
	Log( "Closing connection to " + socket.remote_address + " ( cid " + std::to_string( socket.cid ) + " )" );
	CloseSocket( socket.fd, socket.cid );
	free( socket.buffer.data1 );
	free( socket.buffer.data2 );
	m_server.cid_to_fd[ socket.cid ] = 0;
}

}
}
