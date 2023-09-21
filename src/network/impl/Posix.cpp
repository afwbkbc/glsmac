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

static unsigned int sockaddr_in_size = sizeof(struct sockaddr_in);


Network::Impl::Impl() {
	signal(SIGPIPE, SIG_IGN);
}

void Network::Impl::Start() {
	signal(SIGPIPE, SIG_IGN);
}

void Network::Impl::Stop() {

}

std::string Network::Impl::Listen() {
	addrinfo hints, * res, * p;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	m_tmp.tmpint = getaddrinfo(nullptr, std::to_string(GLSMAC_PORT).c_str(), &hints, &res);
	if (m_tmp.tmpint != 0) {
		return ((std::string)"Failed to getaddrinfo: " + gai_strerror(m_tmp.tmpint));
	}

	unsigned int addr_i = 0;
	char ip_str[INET6_ADDRSTRLEN];

	for (p = res; p != nullptr; p = p->ai_next) {
		void* addr;
		std::string ip_ver;

		if (p->ai_family == AF_INET) {
			ip_ver = "IPv4";
			sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(p->ai_addr);
			addr = &(ipv4->sin_addr);
			++addr_i;
		}
		else {
			ip_ver = "IPv6";
			sockaddr_in6* ipv6 = reinterpret_cast<sockaddr_in6*>(p->ai_addr);
			addr = &(ipv6->sin6_addr);
			++addr_i;
		}

		inet_ntop(p->ai_family, addr, ip_str, sizeof(ip_str));

		local_socket_data_t socket_data;
		socket_data.local_address = ip_str;

		Log("Opening listening socket on " + socket_data.local_address);
		socket_data.fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (socket_data.fd == -1) {
			Log("Failed to creating listening socket on " + socket_data.local_address);
			continue; // don't fail right away, maybe we can open on some other interface // TODO: show warning to user?
		}

		m_tmp.tmpint = fcntl(socket_data.fd, F_GETFL);
		if (m_tmp.tmpint == -1) {
			return "Failed to get socket flags";
		}
		if (fcntl(socket_data.fd, F_SETFL, m_tmp.tmpint | O_NONBLOCK) == -1) {
			return "Failed to set socket flags";
		}

		m_tmp.tmpint = bind(socket_data.fd, p->ai_addr, p->ai_addrlen);
		if (m_tmp.tmpint) {
			Log("Failed to bind to listening socket on " + socket_data.local_address);
			close(socket_data.fd);
			continue;
		}

		m_tmp.tmpint = listen(socket_data.fd, GLSMAC_MAX_INCOMING_CONNECTIONS);
		if (m_tmp.tmpint == -1) {
			Log("Failed to listen on " + socket_data.local_address);
			close(socket_data.fd);
			continue;
		}

		// TODO: for some reason there are no errors if port is already taken by other process, need to check it explicitly before listening

		ASSERT(m_server.listening_sockets.find(socket_data.fd) == m_server.listening_sockets.end(), "duplicate listening socket id");
		m_server.listening_sockets[socket_data.fd] = socket_data;

	}

	freeaddrinfo(res);

	return "";
}

std::string Network::Impl::Connect(const std::string& remote_address) {

	addrinfo hints, * p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	m_tmp.tmpint = getaddrinfo(remote_address.c_str(), std::to_string(GLSMAC_PORT).c_str(), &hints, &p);
	if (m_tmp.tmpint != 0) {
		return ((std::string)"Failed to getaddrinfo: " + gai_strerror(m_tmp.tmpint));
	}

	if (p == nullptr) {
		return ("No addresses found");
	}

	m_client.socket.fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	if (m_client.socket.fd == -1) {
		return ("Error while creating socket");
	}

	m_tmp.tmpint = fcntl(m_client.socket.fd, F_GETFL, 0);
	fcntl(m_client.socket.fd, F_SETFL, m_tmp.tmpint | O_NONBLOCK);

	m_tmp.tmpint2 = 100; // max attempts (100*20ms = 2s)
	while ((m_tmp.tmpint = connect(m_client.socket.fd, p->ai_addr, p->ai_addrlen)) == -1) {
		if (errno == EINPROGRESS && m_tmp.tmpint2 > 0)
		{
			m_tmp.tmpint2--;
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		else {
			close(m_client.socket.fd);
			m_client.socket.fd = 0;
			freeaddrinfo(p);
			return ("Connection failed: " + std::to_string(errno));
		}
	}

	if (p->ai_family == AF_INET) {
		struct sockaddr_in* psai = (struct sockaddr_in*)p->ai_addr;
		char ip[INET_ADDRSTRLEN];
		if (inet_ntop(p->ai_family, &(psai->sin_addr), ip, INET_ADDRSTRLEN) != NULL) {
			m_client.socket.remote_address = ip;
		}
	}
	else if (p->ai_family == AF_INET6) {
		struct sockaddr_in6* psai = (struct sockaddr_in6*)p->ai_addr;
		char ip[INET6_ADDRSTRLEN];
		if (inet_ntop(p->ai_family, &(psai->sin6_addr), ip, INET6_ADDRSTRLEN) != NULL) {
			m_client.socket.remote_address = ip;
		}
	}
	else {
		freeaddrinfo(p);
		return ("Unsupported IP type: " + remote_address);
	}

	freeaddrinfo(p);

	return "";
}

std::string Network::Impl::Accept( const int cid ) {
	m_server.tmp.newfd = accept( cid, (sockaddr*)&m_server.tmp.client_addr, &sockaddr_in_size);
	if (m_server.tmp.newfd != -1) {

		Log("Accepting connection " + std::to_string(m_server.tmp.newfd));

		m_tmp.tmpint = 1;
		setsockopt(m_server.tmp.newfd, SOL_SOCKET, MSG_NOSIGNAL, (void*)&m_tmp.tmpint, sizeof(m_tmp.tmpint));

		m_tmp.tmpint = fcntl(m_server.tmp.newfd, F_GETFL, 0);
		fcntl(m_server.tmp.newfd, F_SETFL, m_tmp.tmpint | O_NONBLOCK);

		remote_socket_data_t data;
		data.buffer.len = 0;
		data.buffer.data1 = (char*)malloc(BUFFER_SIZE);
		data.buffer.data2 = (char*)malloc(BUFFER_SIZE);
		data.buffer.data = data.buffer.data1;
		data.buffer.ptr = data.buffer.data;
		data.fd = m_server.tmp.newfd;
		data.remote_address = inet_ntoa( ( (struct sockaddr_in*)&m_server.tmp.client_addr)->sin_addr );
		data.last_data_at = m_tmp.now;
		data.ping_needed = false;
		data.pong_needed = false;
		data.ping_sent = false;

		ASSERT(m_server.client_sockets.find(m_server.tmp.newfd) == m_server.client_sockets.end(), "client socket already added");
		m_server.client_sockets[m_server.tmp.newfd] = data;

		Log( "Accepted connection from " + data.remote_address );

		return data.remote_address;
	}
	return "";
}

int Network::Impl::Receive( const fd_t fd, void *buf, const int len ) {
	return recv(  fd, buf, len, O_NONBLOCK );
}

int Network::Impl::Send( const fd_t fd, const void *buf, const int len ) {
	ASSERT( len <= BUFFER_SIZE, "packet size overflow ( " + std::to_string( len ) + " > " + std::to_string( BUFFER_SIZE ) + "), consider increasing BUFFER_SIZE" );
	return send( fd, buf, len, MSG_NOSIGNAL );
}

void Network::Impl::Close( const fd_t fd ) {
	close( fd );
}

}
