#include <thread>

#include "SimpleTCP.h"

namespace network {
namespace simpletcp {

SimpleTCP::SimpleTCP() : Network() {

}

void SimpleTCP::Start() {
	m_impl.Start();
}

void SimpleTCP::Stop() {
	m_impl.Stop();
	ASSERT( m_impl.m_server.listening_sockets.empty(), "some connection(s) still active when destroying network" );
}

MT_Response SimpleTCP::ListenStart() {

	ASSERT( m_impl.m_server.listening_sockets.empty(), "some connection socket(s) already active" );

	Log( (std::string) "Starting server on port " + std::to_string( m_impl.GLSMAC_PORT ) );

	auto error = m_impl.Listen();
	if ( !error.empty() ) {
		return Error( error );
	}

	if ( m_impl.m_server.listening_sockets.empty() ) {
		return Error( "Failed to listen on port " + std::to_string( m_impl.GLSMAC_PORT ) );
	}

	m_impl.m_tmp.event.Clear();
	m_impl.m_tmp.event.type = Event::ET_LISTEN;
	m_impl.m_tmp.event.cid = m_impl.m_server.tmp.newfd;
	AddEvent(m_impl.m_tmp.event);

	Log( "Server started" );

	return Success();

}

MT_Response SimpleTCP::ListenStop() {
	for ( auto& it : m_impl.m_server.client_sockets ) {
		CloseSocket( it.second.fd, it.first );
		free( it.second.buffer.data1 );
		free( it.second.buffer.data2 );
	}
	m_impl.m_server.client_sockets.clear();

	for ( auto& socket : m_impl.m_server.listening_sockets ) {
		CloseSocket( socket.second.fd, socket.first );
	}
	m_impl.m_server.listening_sockets.clear();

	if ( m_impl.m_client.socket.fd ) {
		CloseSocket( m_impl.m_client.socket.fd );
		free( m_impl.m_client.socket.buffer.data1 );
		free( m_impl.m_client.socket.buffer.data2 );
		m_impl.m_client.socket.fd = 0;
	}

	Log( "Server stopped" );

	return Success();
}

MT_Response SimpleTCP::Connect( const std::string& remote_address ) {

	ASSERT( m_impl.m_client.socket.fd == 0, "connection socket already active" );

	Log((std::string)"Connecting to " + remote_address + " port " + std::to_string( m_impl.GLSMAC_PORT) );

	auto error = m_impl.Connect( remote_address );
	if ( !error.empty() ) {
		return Error(error);
	}

	m_impl.m_client.socket.buffer.len = 0;
	m_impl.m_client.socket.buffer.data1 = (char*)malloc( m_impl.BUFFER_SIZE );
	m_impl.m_client.socket.buffer.data2 = (char*)malloc( m_impl.BUFFER_SIZE );
	m_impl.m_client.socket.buffer.data = m_impl.m_client.socket.buffer.data1;
	m_impl.m_client.socket.buffer.ptr = m_impl.m_client.socket.buffer.data;
	m_impl.m_client.socket.last_data_at = time( nullptr );
	m_impl.m_client.socket.ping_needed = false;
	m_impl.m_client.socket.pong_needed = false;
	m_impl.m_client.socket.ping_sent = false;

	Log( "Connection successful" );

	return Success();
}

MT_Response SimpleTCP::Disconnect() {

	if ( m_impl.m_client.socket.fd ) {
		CloseSocket( m_impl.m_client.socket.fd, 0, true ); // no need to send event if disconnect was initiated by user
		free( m_impl.m_client.socket.buffer.data1 );
		free( m_impl.m_client.socket.buffer.data2 );
		m_impl.m_client.socket.fd = 0;
	}

	return Success();
}

MT_Response SimpleTCP::DisconnectClient( const size_t cid ) {

	if ( GetCurrentConnectionMode() != CM_SERVER ) {
		return Success();
	}

	auto it = m_impl.m_server.client_sockets.find( cid );
	if ( it != m_impl.m_server.client_sockets.end() ) {
		CloseSocket( it->second.fd, cid );
		free( it->second.buffer.data1 );
		free( it->second.buffer.data2 );
		m_impl.m_server.client_sockets.erase( it );
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
				if ( event.cid ) { // presense of cid means we are server
					m_impl.m_tmp.tmpint = 0;
					auto it = m_impl.m_server.client_sockets.find( event.cid );
					if ( it != m_impl.m_server.client_sockets.end() ) { // if not found it may mean event is old so can be ignored
						if ( !WriteToSocket( it->second.fd, event.data.packet_data ) ) {
							CloseSocket( it->second.fd, it->first );
							free( it->second.buffer.data1 );
							free( it->second.buffer.data2 );
							m_impl.m_server.client_sockets.erase( it );
						}
					}
				} else if ( m_impl.m_client.socket.fd ) {
					if ( !WriteToSocket( m_impl.m_client.socket.fd, event.data.packet_data ) ) {
						CloseSocket( m_impl.m_client.socket.fd );
						free( m_impl.m_client.socket.buffer.data1 );
						free( m_impl.m_client.socket.buffer.data2 );
						m_impl.m_client.socket.fd = 0;
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
				ASSERT( DisconnectClient( event.cid ).result == R_SUCCESS, "failed to disconnect client" );
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

	m_impl.m_tmp.now = time( nullptr );

	// accept new connections (server)
	if ( !m_impl.m_server.listening_sockets.empty() ) {
		// Log( "Checking for connections" ); // SPAMMY
		for ( auto& it : m_impl.m_server.listening_sockets ) {

			const auto remote_address = m_impl.Accept( it.first );
			if ( !remote_address.empty() ) {
				m_impl.m_tmp.event.Clear();
				m_impl.m_tmp.event.type = Event::ET_CLIENT_CONNECT;
				m_impl.m_tmp.event.cid = m_impl.m_server.tmp.newfd;
				m_impl.m_tmp.event.data.remote_address = remote_address;
				AddEvent(m_impl.m_tmp.event);
			}

		}
	}

	// process pings
	for ( auto it = m_impl.m_server.client_sockets.begin(); it != m_impl.m_server.client_sockets.end() ; ) {
		if ( !MaybePingDo( it->second, it->first ) ) {
			CloseSocket( it->second.fd, it->first );
			free( it->second.buffer.data1 );
			free( it->second.buffer.data2 );
			m_impl.m_server.client_sockets.erase( it++ );
		}
		else {
			++it;
		}
	}
	if ( m_impl.m_client.socket.fd ) {
		if ( !MaybePingDo( m_impl.m_client.socket ) ) {
			CloseSocket( m_impl.m_client.socket.fd );
			free( m_impl.m_client.socket.buffer.data1 );
			free( m_impl.m_client.socket.buffer.data2 );
			m_impl.m_client.socket.fd = 0;
		}
	}

	// read from connection (client)
	if ( m_impl.m_client.socket.fd ) {
		if ( !ReadFromSocket( m_impl.m_client.socket ) ) {
			CloseSocket( m_impl.m_client.socket.fd );
			free( m_impl.m_client.socket.buffer.data1 );
			free( m_impl.m_client.socket.buffer.data2 );
			m_impl.m_client.socket.fd = 0;
		}
	}

	// read from connections (server)
	for ( auto it = m_impl.m_server.client_sockets.begin(); it != m_impl.m_server.client_sockets.end() ; ) {
		if ( !ReadFromSocket( it->second, it->first ) ) {
			CloseSocket( it->second.fd, it->first );
			free( it->second.buffer.data1 );
			free( it->second.buffer.data2 );
			m_impl.m_server.client_sockets.erase( it++ );
		}
		else {
			++it;
		}
	}
}

bool SimpleTCP::ReadFromSocket( Impl::remote_socket_data_t& socket, const size_t cid ) {

	// max allowed size to read
	m_impl.m_tmp.tmpint = ( m_impl.BUFFER_SIZE - socket.buffer.len );

	//Log( "Reading up to " + std::to_string( m_tmp.tmpint ) + " bytes from " + std::to_string( socket.fd ) + " (buffer=" + std::to_string( (long int) socket.buffer.ptr ) + ", BUFFER_SIZE=" + std::to_string( BUFFER_SIZE ) + " buffer len = " + std::to_string( socket.buffer.len ) + ")" );
	m_impl.m_tmp.tmpint2 = m_impl.Receive( socket.fd, socket.buffer.ptr, m_impl.m_tmp.tmpint );

	if ( m_impl.m_tmp.tmpint2 < 0 ) {
		if (errno == EAGAIN) {
			// no pending data
			if ( !MaybePing( socket, cid ) ) {
				return false;
			}
		}
		else {
			Log( "Connection failed (result=" + std::to_string( m_impl.m_tmp.tmpint2 ) + " code=" + std::to_string( errno ) + ")" );
			return false;
		}
	}

	if ( m_impl.m_tmp.tmpint2 == 0 ) {
		// no pending data
		if ( !MaybePing( socket, cid ) ) {
			return false;
		}
	}

	if ( m_impl.m_tmp.tmpint2 > 0 ) {

		Log( "Read " + std::to_string( m_impl.m_tmp.tmpint2 ) + " bytes into buffer " + std::to_string( (long long) socket.buffer.ptr ) + " (size=" + std::to_string( socket.buffer.len ) + ")" );

		socket.last_data_at = m_impl.m_tmp.now;
		socket.ping_needed = false;
		socket.pong_needed = false;

		socket.buffer.len += m_impl.m_tmp.tmpint2;
		socket.buffer.ptr += m_impl.m_tmp.tmpint2;

	}

	{

		if ( socket.buffer.len == 0 ) {
			return true; // no new data
		}

		//Log( "Trying to process buffer " + to_string( (long int) socket.buffer.data ) + " ( size=" + to_string( socket.buffer.len ) + " )" );
		if ( socket.buffer.len < sizeof( m_impl.m_tmp.tmpint ) ) {
			//Log( "Buffer " + to_string( (long int) socket.buffer.data ) + " does not contain full size field yet ( " + to_string( socket.buffer.len ) + " < " + to_string( sizeof( m_tmp.tmpint ) ) + " )" );
			return true;
		}

		m_impl.m_tmp.ptr = socket.buffer.data;
		memcpy( &m_impl.m_tmp.tmpint, m_impl.m_tmp.ptr, sizeof( m_impl.m_tmp.tmpint ) );
		m_impl.m_tmp.tmpint2 = sizeof( m_impl.m_tmp.tmpint ) + m_impl.m_tmp.tmpint;

		if ( socket.buffer.len < m_impl.m_tmp.tmpint2 ) {
			//Log( "Buffer " + std::to_string( (long int) socket.buffer.data ) + " does not contain all data yet ( " + std::to_string( socket.buffer.len ) + " < " + std::to_string( m_tmp.tmpint2 ) + " )" );
			return true;
		}
		else {
			//Log( "Buffer " + std::to_string( (long int) socket.buffer.data ) + " contains enough data ( " + std::to_string( socket.buffer.len ) + " >= " + std::to_string( m_tmp.tmpint2 ) + " )" );
		}

		//Log( "Processing buffer " + to_string( (long int) m_tmp.ptr ) + " ( size=" + to_string( m_tmp.tmpint2 ) + " )" );

		if ( m_impl.m_tmp.tmpint == 0 && m_impl.m_tmp.tmpint2 == sizeof( m_impl.m_tmp.tmpint ) ) {
			// zero length means 'bye'
			Log( "Connection closed by remote host" );
			return false;
		}

		m_impl.m_tmp.ptr += sizeof( m_impl.m_tmp.tmpint );

		{
			//Log( "Read packet (" + std::to_string( m_tmp.tmpint ) + " bytes)" );
			m_impl.m_tmp.event.Clear();
			if ( cid ) {
				m_impl.m_tmp.event.cid = cid;
			}
			m_impl.m_tmp.event.data.remote_address = socket.remote_address;
			m_impl.m_tmp.event.data.packet_data = std::string( m_impl.m_tmp.ptr, m_impl.m_tmp.tmpint );
			try {
				Packet p;
				p.Unserialize( Buffer( m_impl.m_tmp.event.data.packet_data ) );
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
					m_impl.m_tmp.event.type = Event::ET_PACKET;
					AddEvent( m_impl.m_tmp.event );
				}
			} catch ( std::runtime_error& err ) {
				m_impl.m_tmp.event.type = Event::ET_ERROR;
				m_impl.m_tmp.event.data.packet_data = err.what();
				AddEvent( m_impl.m_tmp.event );
			}

		}

		Log( "Processed " + std::to_string( m_impl.m_tmp.tmpint2 ) + " bytes" );

		if ( m_impl.m_tmp.tmpint2 > 0 ) {
			ASSERT( socket.buffer.len >= m_impl.m_tmp.tmpint2, "processed more than total" );
			socket.buffer.len -= m_impl.m_tmp.tmpint2;
			if ( socket.buffer.len > 0 ) {
				if ( socket.buffer.data == socket.buffer.data1 ) {
					//Log( "SWAP data1 -> data2" );
					memcpy( socket.buffer.data2, socket.buffer.data + m_impl.m_tmp.tmpint2, socket.buffer.len );
					socket.buffer.data = socket.buffer.data2;
				}
				else {
					//Log( "SWAP data2 -> data1" );
					memcpy( socket.buffer.data1, socket.buffer.data + m_impl.m_tmp.tmpint2, socket.buffer.len );
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
	m_impl.m_tmp.tmpint2 = data.size();
	//Log( "Writing " + to_string( m_tmp.tmpint2 ) + " bytes" );
	// send size
	m_impl.m_tmp.tmpint = m_impl.Send( fd, &m_impl.m_tmp.tmpint2, sizeof( m_impl.m_tmp.tmpint2 ) );
	if ( m_impl.m_tmp.tmpint <= 0 ) {
		if (errno == EAGAIN)
		{
			return true; // no data but connection is alive
		}
		Log( "Error writing size to socket (errno=" + std::to_string( errno ) + " size=" + std::to_string( m_impl.m_tmp.tmpint ) + " reqsize=" + std::to_string( m_impl.m_tmp.tmpint2 ) + ")" );

		return false;
	}
	// send data
	m_impl.m_tmp.tmpint = m_impl.Send( fd, data.data(), data.size() );
	if ( m_impl.m_tmp.tmpint2 <= 0 ) {
		if (errno == EAGAIN)
		{
			return true; // no data but connection is alive
		}
		Log( "Error writing data to socket" );
		return false;
	}
	//Log( "Write successful" );
	return true;
}

bool SimpleTCP::MaybePing( Impl::remote_socket_data_t& socket, const size_t cid ) {
	m_impl.m_tmp.time = m_impl.m_tmp.now - socket.last_data_at;

	if ( m_impl.m_tmp.time > SEND_PING_AFTER && !socket.ping_sent ) {
		Log( "Need ping" );
		socket.ping_needed = true;
	}

	return true;
}

bool SimpleTCP::MaybePingDo( Impl::remote_socket_data_t& socket, const size_t cid ) {
	m_impl.m_tmp.time = m_impl.m_tmp.now - socket.last_data_at;

	if ( m_impl.m_tmp.time > DISCONNECT_AFTER ) {
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
}

void SimpleTCP::CloseSocket( int fd, size_t cid, bool skip_event ) {
	Log( "Closing socket " + std::to_string( fd ) );
	uint32_t bye = 0;
	m_impl.Send( fd, &bye, sizeof( bye ) );
	m_impl.Close( fd );
	if ( !skip_event ) {
		m_impl.m_tmp.event.Clear();
		if ( cid ) {
			m_impl.m_tmp.event.type = Event::ET_CLIENT_DISCONNECT;
			m_impl.m_tmp.event.cid = cid;
		}
		else {
			m_impl.m_tmp.event.type = Event::ET_DISCONNECT;
		}
		AddEvent( m_impl.m_tmp.event );
	}
}

}
}
