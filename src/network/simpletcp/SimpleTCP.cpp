#include <signal.h>
#include <thread>

#include "SimpleTCP.h"

#define GLSMAC_PORT 4888
#define GLSMAC_MAX_INCOMING_CONNECTIONS 64

namespace network {
namespace simpletcp {

SimpleTCP::SimpleTCP() : Network() {
#ifndef _WIN32
	m_server.tmp.client_addr_size = sizeof( m_server.tmp.client_addr );
	
	signal(SIGPIPE, SIG_IGN);
#endif
}

void SimpleTCP::Start() {
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
}

void SimpleTCP::Stop() {
#ifndef _WIN32
	ASSERT( m_server.listening_sockets.empty(), "some connection(s) still active when destroying network" );
#endif
}

MT_Response SimpleTCP::ListenStart() {
#ifdef _WIN32
	return Error( "winsock not implemented yet" );
#else

	ASSERT( m_server.listening_sockets.empty(), "some connection socket(s) already active" );
	
	Log( (std::string) "Starting server on port " + std::to_string( GLSMAC_PORT ) );
	
	addrinfo hints, *res, *p;
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	m_tmp.tmpint = getaddrinfo( nullptr, std::to_string( GLSMAC_PORT ).c_str(), &hints, &res );
	if ( m_tmp.tmpint != 0 ) {
		return Error( (std::string) "Failed to getaddrinfo: " + gai_strerror( m_tmp.tmpint ) );
	}

	unsigned int addr_i = 0;
	char ip_str[ INET6_ADDRSTRLEN ];

	for ( p = res; p != nullptr; p = p->ai_next ) {
		void *addr;
		std::string ip_ver;
		
		if (p->ai_family == AF_INET) {
			ip_ver = "IPv4";
			sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in *>( p->ai_addr );
			addr = &( ipv4->sin_addr );
			++addr_i;
		}
		else {
			ip_ver = "IPv6";
			sockaddr_in6 *ipv6 = reinterpret_cast<sockaddr_in6 *>( p->ai_addr );
			addr = &(ipv6->sin6_addr);
			++addr_i;
		}
		
		inet_ntop( p->ai_family, addr, ip_str, sizeof(ip_str) );
		
		local_socket_data_t socket_data;
		socket_data.local_address = ip_str;
		
		Log( "Opening listening socket on " + socket_data.local_address );
		socket_data.fd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
		if ( socket_data.fd == -1 ) {
			Log( "Failed to creating listening socket on " + socket_data.local_address );
			continue; // don't fail right away, maybe we can open on some other interface // TODO: show warning to user?
		}
		
		m_tmp.tmpint = fcntl( socket_data.fd, F_GETFL );
		if ( m_tmp.tmpint == -1 ) {
			return Error( "Failed to get socket flags" );
		}
		if ( fcntl( socket_data.fd, F_SETFL, m_tmp.tmpint | O_NONBLOCK ) == -1 ) {
			return Error( "Failed to set socket flags" );
		}
		
		m_tmp.tmpint = bind( socket_data.fd, p->ai_addr, p->ai_addrlen );
		if ( m_tmp.tmpint ) {
			Log( "Failed to bind to listening socket on " + socket_data.local_address );
			close( socket_data.fd );
			continue;
		}
		
		m_tmp.tmpint = listen( socket_data.fd, GLSMAC_MAX_INCOMING_CONNECTIONS );
		if ( m_tmp.tmpint == -1 ) {
			Log( "Failed to listen on " + socket_data.local_address );
			close(socket_data.fd);
			continue;
		}
		
		// TODO: for some reason there are no errors if port is already taken by other process, need to check it explicitly before listening
		
		ASSERT( m_server.listening_sockets.find( socket_data.fd ) == m_server.listening_sockets.end(), "duplicate listening socket id" );
		m_server.listening_sockets[ socket_data.fd ] = socket_data;
    }
	
	freeaddrinfo(res);

	if ( m_server.listening_sockets.empty() ) {
		return Error( "Failed to listen on port " + std::to_string( GLSMAC_PORT ) );
	}
	
	Log( "Server started" );
	
	return Success();
#endif
}

MT_Response SimpleTCP::ListenStop() {
#ifdef _WIN32
	return Error( "winsock not implemented yet" );
#else
	for ( auto& it : m_server.client_sockets ) {
		CloseSocket( it.second.fd, it.first );
		free( it.second.buffer.data1 );
		free( it.second.buffer.data2 );
	}
	m_server.client_sockets.clear();
	
	for ( auto& socket : m_server.listening_sockets ) {
		CloseSocket( socket.second.fd, socket.first );
	}
    m_server.listening_sockets.clear();
	
	if ( m_client.socket.fd ) {
		CloseSocket( m_client.socket.fd );
		free( m_client.socket.buffer.data1 );
		free( m_client.socket.buffer.data2 );
		m_client.socket.fd = 0;
	}
	
	Log( "Server stopped" );
	
	return Success();
#endif
}

MT_Response SimpleTCP::Connect( const std::string& remote_address ) {
	
#ifdef _WIN32
	return Error( "winsock not implemented yet" );
#else
	ASSERT( m_client.socket.fd == 0, "connection socket already active" );
	
	addrinfo hints, *p;
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	Log( (std::string) "Connecting to " + remote_address + " port " + std::to_string( GLSMAC_PORT ) );
	
	m_tmp.tmpint = getaddrinfo( remote_address.c_str(), std::to_string( GLSMAC_PORT ).c_str(), &hints, &p );
	if ( m_tmp.tmpint != 0 ) {
		return Error( (std::string) "Failed to getaddrinfo: " + gai_strerror( m_tmp.tmpint ) );
	}
	
	if ( p == nullptr ) {
		return Error( "No addresses found" );
	}
	
	m_client.socket.fd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
	if ( m_client.socket.fd == -1 ) {
		return Error( "Error while creating socket" );
	}

	m_tmp.tmpint = fcntl( m_client.socket.fd, F_GETFL, 0 );
	fcntl( m_client.socket.fd, F_SETFL, m_tmp.tmpint | O_NONBLOCK );
	
	m_tmp.tmpint2 = 100; // max attempts (100*20ms = 2s)
	while ( ( m_tmp.tmpint = connect(m_client.socket.fd, p->ai_addr, p->ai_addrlen) ) == -1 ) {
		if ( errno == EINPROGRESS && m_tmp.tmpint2 > 0 )
		{
			m_tmp.tmpint2--;
			std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
		}
		else {
			close( m_client.socket.fd );
			m_client.socket.fd = 0;
			return Error( "Connection failed: " + std::to_string( errno ) );
		}
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
#endif
}

MT_Response SimpleTCP::Disconnect() {
	
#ifdef _WIN32
	return Error( "winsock not implemented yet" );
#else
	if ( m_client.socket.fd ) {
		CloseSocket( m_client.socket.fd, 0, true ); // no need to send event if disconnect was initiated by user
		free( m_client.socket.buffer.data1 );
		free( m_client.socket.buffer.data2 );
		m_client.socket.fd = 0;
	}
	
	return Success();
#endif
}

MT_Response SimpleTCP::DisconnectClient( const size_t cid ) {
#ifdef _WIN32
	return Error( "winsock not implemented yet" );
#else

	auto it = m_server.client_sockets.find( cid );
	if ( it != m_server.client_sockets.end() ) {
		CloseSocket( it->second.fd, cid );
		free( it->second.buffer.data1 );
		free( it->second.buffer.data2 );
		m_server.client_sockets.erase( it );
	}
	return Success();
#endif
}

void SimpleTCP::Iterate() {
#ifdef _WIN32
	return;
#else
	Network::Iterate();
	
	m_tmp.now = time( nullptr );
	
	// accept new connections (server)
	if ( !m_server.listening_sockets.empty() ) {
		// Log( "Checking for connections" ); // SPAMMY
		for ( auto& it : m_server.listening_sockets ) {
			
			m_server.tmp.newfd = accept( it.first, (sockaddr *) &m_server.tmp.client_addr, &m_server.tmp.client_addr_size );
			if (m_server.tmp.newfd != -1) {
				
				Log( "Accepting connection " + std::to_string( m_server.tmp.newfd ) );
				
				m_tmp.tmpint = 1;
				setsockopt( m_server.tmp.newfd, SOL_SOCKET, MSG_NOSIGNAL, (void *)&m_tmp.tmpint, sizeof(m_tmp.tmpint) );
				
				m_tmp.tmpint = fcntl( m_server.tmp.newfd, F_GETFL, 0 );
				fcntl( m_server.tmp.newfd, F_SETFL, m_tmp.tmpint | O_NONBLOCK );

				remote_socket_data_t data;
				data.buffer.len = 0;
				data.buffer.data1 = (char*)malloc( BUFFER_SIZE );
				data.buffer.data2 = (char*)malloc( BUFFER_SIZE );
				data.buffer.data = data.buffer.data1;
				data.buffer.ptr = data.buffer.data;
				data.fd = m_server.tmp.newfd;
				data.remote_address = inet_ntoa( m_server.tmp.client_addr.sin_addr );
				data.last_data_at = m_tmp.now;
				data.ping_needed = false;
				data.pong_needed = false;
				data.ping_sent = false;
				
				ASSERT( m_server.client_sockets.find( m_server.tmp.newfd ) == m_server.client_sockets.end(), "client socket already added" );
				m_server.client_sockets[ m_server.tmp.newfd ] = data;
				
				//Log( "Accepted connection from " + data.remote_address );
				{
					m_tmp.event.Clear();
					m_tmp.event.type = Event::ET_CLIENT_CONNECT;
					m_tmp.event.cid = m_server.tmp.newfd;
					m_tmp.event.data.remote_address = data.remote_address;
					AddEvent( m_tmp.event );
				}
			}
		}
	}
	
	// process pings
	for ( auto it = m_server.client_sockets.begin(); it != m_server.client_sockets.end() ; ) {
		if ( !MaybePingDo( it->second, it->first ) ) {
			CloseSocket( it->second.fd, it->first );
			free( it->second.buffer.data1 );
			free( it->second.buffer.data2 );
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
	
	// process events
	auto events = GetEvents();
	for ( auto& event : events ) {
		switch ( event.type ) {
			case Event::ET_PACKET: {
				Log( "Packet event ( cid = " + std::to_string( event.cid ) + " )" );
				if ( event.cid ) { // presense of cid means we are server
					m_tmp.tmpint = 0;
					auto it = m_server.client_sockets.find( event.cid );
					if ( it != m_server.client_sockets.end() ) { // if not found it may mean event is old so can be ignored
						if ( !WriteToSocket( it->second.fd, event.data.packet_data ) ) {
							CloseSocket( it->second.fd, it->first );
							free( it->second.buffer.data1 );
							free( it->second.buffer.data2 );
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
			case Event::ET_CLIENT_DISCONNECT: {
				DisconnectClient( event.cid );
				break;
			}
			default: {
				// ignore for now
			}
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
	for ( auto it = m_server.client_sockets.begin(); it != m_server.client_sockets.end() ; ) {
		if ( !ReadFromSocket( it->second, it->first ) ) {
			CloseSocket( it->second.fd, it->first );
			free( it->second.buffer.data1 );
			free( it->second.buffer.data2 );
			m_server.client_sockets.erase( it++ );
		}
		else {
			++it;
		}
	}
#endif
}

bool SimpleTCP::ReadFromSocket( remote_socket_data_t& socket, const size_t cid ) {
	
#ifdef _WIN32
	return false;
#else
	// max allowed size to read
	m_tmp.tmpint = ( BUFFER_SIZE - socket.buffer.len );
	
	// Log( "Reading up to " + to_string( m_tmp.tmpint ) + " bytes from " + to_string( socket.fd ) + " (buffer=" + to_string( (long int) socket.buffer.ptr ) + ")" );
	m_tmp.tmpint2 = recv( socket.fd, socket.buffer.ptr, m_tmp.tmpint, O_NONBLOCK );
	
	if ( m_tmp.tmpint2 < 0 ) {
		if (errno == EAGAIN) {
			// no pending data
			if ( !MaybePing( socket, cid ) ) {
				return false;
			}
		}
		else {
			Log( "Connection failed (result=" + std::to_string( m_tmp.tmpint2 ) + " code=" + std::to_string( errno ) + ")" );
			return false;
		}
	}
	
	if ( m_tmp.tmpint2 == 0 ) {
		// no pending data
		if ( !MaybePing( socket, cid ) ) {
			return false;
		}
	}
	
	if ( m_tmp.tmpint2 > 0 ) {
		
		//Log( "Read " + to_string( m_tmp.tmpint2 ) + " bytes into buffer " + to_string( (long int) socket.buffer.ptr ) + " (size=" + to_string( socket.buffer.len ) + ")" );
		
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
			//Log( "Buffer " + to_string( (long int) socket.buffer.data ) + " does not contain all data yet ( " + to_string( socket.buffer.len ) + " < " + to_string( m_tmp.tmpint2 ) + " )" );
			return true;
		}
		else {
			//Log( "Buffer " + to_string( (long int) socket.buffer.data ) + " contains enough data ( " + to_string( socket.buffer.len ) + " >= " + to_string( m_tmp.tmpint2 ) + " )" );
		}
		
		//Log( "Processing buffer " + to_string( (long int) m_tmp.ptr ) + " ( size=" + to_string( m_tmp.tmpint2 ) + " )" );
		
		if ( m_tmp.tmpint == 0 && m_tmp.tmpint2 == sizeof( m_tmp.tmpint ) ) {
			// zero length means 'bye'
			Log( "Connection closed by remote host" );
			return false;
		}
		
		m_tmp.ptr += sizeof( m_tmp.tmpint );

		{
		
			//Log( "Read packet (" + to_string( m_tmp.tmpint ) + " bytes)" );
			m_tmp.event.Clear();
			m_tmp.event.type = Event::ET_PACKET;
			m_tmp.event.data.packet_data = std::string( m_tmp.ptr, m_tmp.tmpint );
			if ( cid ) {
				m_tmp.event.cid = cid;
			}

			// quick hack to respond to pings without escalating events outside
			// TODO: optimize
			Packet p;
			p.Unserialize( Buffer( m_tmp.event.data.packet_data ) );
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
				AddEvent( m_tmp.event );
			}
			
		}
		
		//Log( "Processed " + to_string( m_tmp.tmpint2 ) + " bytes" );
		
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
#endif
}

bool SimpleTCP::WriteToSocket( int fd, const std::string& data ) {
#ifdef _WIN32
	return false;
#else
	//Log( "WriteToSocket( " + to_string( fd ) + " )" ); // SPAMMY
	m_tmp.tmpint2 = data.size();
	//Log( "Writing " + to_string( m_tmp.tmpint2 ) + " bytes" );
	// send size
	m_tmp.tmpint = send( fd, &m_tmp.tmpint2, sizeof( m_tmp.tmpint2 ), MSG_NOSIGNAL );
	if ( m_tmp.tmpint <= 0 ) {
		if (errno == EAGAIN)
		{
			return true; // no data but connection is alive
		}
		Log( "Error writing size to socket (errno=" + std::to_string( errno ) + " size=" + std::to_string( m_tmp.tmpint ) + " reqsize=" + std::to_string( m_tmp.tmpint2 ) + ")" );
		
		return false;
	}
	// send data
	m_tmp.tmpint = send( fd, data.data(), data.size(), MSG_NOSIGNAL );
	if ( m_tmp.tmpint2 <= 0 ) {
		if (errno == EAGAIN)
		{
			return true; // no data but connection is alive
		}
		Log( "Error writing data to socket" );
		return false;
	}
	//Log( "Write successful" );
	return true;
#endif
}

bool SimpleTCP::MaybePing( remote_socket_data_t& socket, const size_t cid ) {
#ifdef _WIN32
	return false;
#else
	m_tmp.time = m_tmp.now - socket.last_data_at;
	
	if ( m_tmp.time > SEND_PING_AFTER && !socket.ping_sent ) {
		Log( "Need ping" );
		socket.ping_needed = true;
	}
	
	return true;
#endif
}

bool SimpleTCP::MaybePingDo( remote_socket_data_t& socket, const size_t cid ) {
#ifdef _WIN32
	return false;
#else
	m_tmp.time = m_tmp.now - socket.last_data_at;
	
	if ( m_tmp.time > DISCONNECT_AFTER ) {
		Log( "Ping timeout on " + std::to_string( socket.fd ) );
		
		return false;
	}
	
	if ( socket.ping_needed && !socket.ping_sent ) {
		Log( "Sending ping to " + std::to_string( socket.fd ) );
		Packet packet;
		packet.type = Packet::PT_PING;
		std::string data = packet.Serialize().ToString();
		socket.ping_sent = true;
		return WriteToSocket( socket.fd, data );
	}
	if ( socket.pong_needed ) {
		Log( "Ping received, sending pong to " + std::to_string( socket.fd ) );
		Packet packet;
		packet.type = Packet::PT_PONG;
		socket.pong_needed = false;
		std::string data = packet.Serialize().ToString();
		return WriteToSocket( socket.fd, data );
	}
	
	return true;
#endif
}

void SimpleTCP::CloseSocket( int fd, size_t cid, bool skip_event ) {
#ifdef _WIN32
	return;
#else
	Log( "Closing socket " + std::to_string( fd ) );
	uint32_t bye = 0;
	send( fd, &bye, sizeof( bye ), MSG_NOSIGNAL );
	close( fd );
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
#endif
}


}
}
