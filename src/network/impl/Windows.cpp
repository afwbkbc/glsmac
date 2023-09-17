#include "../Network.h"

#include <winsock2.h>

namespace network {

Network::Impl::Impl() {
	
}

void Network::Impl::Start() {

}

void Network::Impl::Stop() {

}

std::string Network::Impl::Listen() {
	return "Listen() not implemented";
}

std::string Network::Impl::Connect( const std::string& remote_address ) {
	return "Connect() not implemented";
}

std::string Network::Impl::Accept( const int cid ) {
	return "";
}

int Network::Impl::Receive( const int fd, void *buf, const int len ) {
	return -1;
}

int Network::Impl::Send( const int fd, const void *buf, const int len ) {
	return -1;
}

void Network::Impl::Close( const int fd ) {

}

}
