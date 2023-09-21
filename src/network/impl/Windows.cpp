#include "../Network.h"

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

namespace network {

static WSADATA wsa_data = {};
static std::string wsa_error = "";

Network::Impl::Impl() {
    Log("Initializing Winsock");
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (iResult != 0) {
        wsa_error = "WSAStartup failed with error: " + std::to_string(iResult);
        Log(wsa_error);
    }
    else {
        wsa_error.clear();
    }
}

Network::Impl::~Impl() {
    if (wsa_error.empty()) {
        Log("Deinitializing Winsock");
        WSACleanup();
    }
}

void Network::Impl::Start() {
}

void Network::Impl::Stop() {
}

std::string Network::Impl::Listen() {
    if (!wsa_error.empty()) return wsa_error;

    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[BUFFER_SIZE];
    int recvbuflen = BUFFER_SIZE;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, std::to_string(GLSMAC_PORT).c_str(), &hints, &result);
    if (iResult != 0) {
        return "getaddrinfo failed with error: " + std::to_string(iResult);
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        const auto code = WSAGetLastError();
        freeaddrinfo(result);
        return "socket failed with error: " + std::to_string(code);
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        const auto code = WSAGetLastError();
        freeaddrinfo(result);
        closesocket(ListenSocket);
        return "bind failed with error: " + std::to_string(code);
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        const auto code = WSAGetLastError();
        closesocket(ListenSocket);
        return "Listen failed with error: " + std::to_string(code);
    }

    return "";
}

std::string Network::Impl::Connect( const std::string& remote_address ) {
    if (!wsa_error.empty()) return wsa_error;

    struct addrinfo* p = NULL, hints;
    const char* sendbuf = "this is a test";
    int recvbuflen = BUFFER_SIZE;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    // Resolve the server address and port
    m_tmp.tmpint = getaddrinfo(remote_address.c_str(), std::to_string(GLSMAC_PORT).c_str(), &hints, &p);
    if (m_tmp.tmpint != 0) {
        return ((std::string)"Failed to getaddrinfo: " + gai_strerror(m_tmp.tmpint));
    }

    if (p == nullptr) {
        return ("No addresses found");
    }

    // Create a SOCKET for connecting to server
    m_client.socket.fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (m_client.socket.fd == INVALID_SOCKET) {
        return "Socket failed with error: " + std::to_string(WSAGetLastError());
    }

    //m_tmp.tmpint = fcntl(m_client.socket.fd, F_GETFL, 0);
    //fcntl(m_client.socket.fd, F_SETFL, m_tmp.tmpint | O_NONBLOCK);

    m_tmp.tmpint = 1;
    m_tmp.tmpint = ioctlsocket(m_client.socket.fd, FIONBIO, (unsigned long*)&m_tmp.tmpint);

    if (m_tmp.tmpint == SOCKET_ERROR) {
        closesocket(m_client.socket.fd);
        m_client.socket.fd = INVALID_SOCKET;
        freeaddrinfo(p);
        return "Failed to put socket into non-blocking mode";
    }

    m_tmp.tmpint2 = 100; // max attempts (100*20ms = 2s)
    unsigned long l = 0;
    while (connect(m_client.socket.fd, p->ai_addr, p->ai_addrlen) == -1) {
        if (ioctlsocket(m_client.socket.fd, FIONREAD, &l) == SOCKET_ERROR) {
            // error
            closesocket(m_client.socket.fd);
            m_client.socket.fd = INVALID_SOCKET;
            freeaddrinfo(p);
            return "Connection failed";
        }
        if (l > 0) {
            // connected
            break;
        }
        if (m_tmp.tmpint2 > 0)
        {
            // wait
            m_tmp.tmpint2--;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        else {
            // timeout
            closesocket(m_client.socket.fd);
            m_client.socket.fd = INVALID_SOCKET;
            freeaddrinfo(p);
            return ("Connection failed: " + std::to_string(errno));
        }
    }

    freeaddrinfo(p);

    return "";
}

std::string Network::Impl::Accept( const int cid ) {
    if (!wsa_error.empty()) return wsa_error;

	return "";
}

int Network::Impl::Receive( const fd_t fd, void *buf, const int len ) {
    return recv(fd, (char*)buf, len, 0);
}

int Network::Impl::Send( const fd_t fd, const void *buf, const int len ) {
	return send(fd, (const char*)buf, len, 0);
}

void Network::Impl::Close( const fd_t fd ) {
    closesocket(fd);
}

}
