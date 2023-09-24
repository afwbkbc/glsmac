#include "../Network.h"

#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <thread>

namespace network {

static unsigned int sockaddr_in_size = sizeof( struct sockaddr_in );


Network::Impl::Impl() {
	signal( SIGPIPE, SIG_IGN );
}

void Network::Impl::Start() {
	signal( SIGPIPE, SIG_IGN );
}

void Network::Impl::Stop() {

}

const Network::Impl::ec_t Network::Impl::GetLastErrorCode() const {
	return errno;
}

void Network::Impl::ConfigureSocket( const fd_t socket ) const {
	unsigned long flags = fcntl( m_client.socket.fd, F_GETFL, 0 );
	fcntl( socket, F_SETFL, flags | O_NONBLOCK );
	m_tmp.tmpint = 1;
	setsockopt( m_server.tmp.newfd, SOL_SOCKET, MSG_NOSIGNAL, (void*)&m_tmp.tmpint, sizeof( m_tmp.tmpint ) );
}

bool Network::Impl::IsConnectionSucceeded( const ec_t ec ) const {
	return ec == 0;
}

bool Network::Impl::IsConnectionIdle( const ec_t ec ) const {
	return ec == EINPROGRESS;
}

const std::string Network::Impl::GetErrorMessage( const ec_t ec ) const {
	// TODO
	return std::to_string( ec );
}

int Network::Impl::Receive( const fd_t fd, void* buf, const int len ) {
	return recv( fd, buf, len, O_NONBLOCK );
}

int Network::Impl::Send( const fd_t fd, const void* buf, const int len ) {
	ASSERT( len <= BUFFER_SIZE, "packet size overflow ( " + std::to_string( len ) + " > " + std::to_string( BUFFER_SIZE ) + "), consider increasing BUFFER_SIZE" );
	return send( fd, buf, len, MSG_NOSIGNAL );
}

void Network::Impl::CloseSocket( const fd_t socket ) {
	close( socket );
}

}
