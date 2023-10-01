#include "../Network.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <thread>

namespace network {

Network::Impl::Impl() {
	signal( SIGPIPE, SIG_IGN );
}

Network::Impl::~Impl() {

}

void Network::Impl::Start() {
	signal( SIGPIPE, SIG_IGN );
}

void Network::Impl::Stop() {

}

const Network::Impl::ec_t Network::Impl::GetLastErrorCode() const {
	return errno;
}

bool Network::Impl::IsSocketInvalid( const fd_t fd ) const {
	return fd == -1;
}

void Network::Impl::ConfigureSocket( const fd_t socket ) const {
	unsigned long flags = fcntl( socket, F_GETFL, 0 );
	fcntl( socket, F_SETFL, flags | O_NONBLOCK );
	unsigned long l = 1;
	setsockopt( socket, SOL_SOCKET, MSG_NOSIGNAL, (void*)&l, sizeof( l ) );
}

bool Network::Impl::IsConnectionSucceeded( const ec_t ec ) const {
	return ec == 0;
}

bool Network::Impl::IsConnectionIdle( const ec_t ec ) const {
	return ec == EAGAIN || ec == EINPROGRESS || ec == EALREADY;
}

const std::string Network::Impl::GetErrorMessage( const ec_t ec ) const {
	// TODO
	return std::to_string( ec );
}

int Network::Impl::Receive( const fd_t fd, void* buf, const int len ) const {
	return recv( fd, buf, len, O_NONBLOCK );
}

int Network::Impl::Send( const fd_t fd, const void* buf, const int len ) const {
	ASSERT( len <= BUFFER_SIZE, "packet size overflow ( " + std::to_string( len ) + " > " + std::to_string( BUFFER_SIZE ) + "), consider increasing BUFFER_SIZE" );
	return send( fd, buf, len, MSG_NOSIGNAL );
}

void Network::Impl::CloseSocket( const fd_t socket ) const {
	close( socket );
}

}
