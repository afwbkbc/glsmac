#include "network/Network.h"

namespace network {

static WSADATA wsa_data = {};

Network::Impl::Impl() {
	Log( "Initializing Winsock" );
	WSAStartup( MAKEWORD( 2, 2 ), &wsa_data );
}

Network::Impl::~Impl() {
	Log( "Deinitializing Winsock" );
	WSACleanup();
}

void Network::Impl::Start() {
}

void Network::Impl::Stop() {
}

const Network::Impl::ec_t Network::Impl::GetLastErrorCode() const {
	return WSAGetLastError();
}

bool Network::Impl::IsSocketInvalid( const fd_t fd ) const {
	return fd == INVALID_SOCKET;
}

void Network::Impl::ConfigureSocket( const fd_t socket ) const {
	unsigned long l = 1;
	ioctlsocket( socket, FIONBIO, &l );
}

bool Network::Impl::IsConnectionSucceeded( const ec_t ec ) const {
	return ec == WSAEISCONN;
}

bool Network::Impl::IsConnectionIdle( const ec_t ec ) const {
	return ec == WSAEWOULDBLOCK;
}

const std::string Network::Impl::GetErrorMessage( const ec_t ec ) const {
	// TODO
	return std::to_string( ec );
}

void Network::Impl::CloseSocket( const fd_t socket ) const {
	shutdown( socket, SD_SEND );
	closesocket( socket );
}

int Network::Impl::Receive( const fd_t fd, void* buf, const int len ) const {
	return recv( fd, (char*)buf, len, 0 );
}

int Network::Impl::Send( const fd_t fd, const void* buf, const int len ) const {
	return send( fd, (const char*)buf, len, 0 );
}

}
