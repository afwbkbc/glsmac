#pragma once

#include "base/MTModule.h"

#include "Event.h"
#include "types/Packet.h"

#ifdef _WIN32
#include <WinSock2.h>
#define fd_t SOCKET
#else
#define fd_t int
#endif

using namespace types;

namespace network {

enum op_t {
	OP_NONE,
	OP_SUCCESS, // just confirm success
	OP_CONNECT,
	OP_DISCONNECT,
	OP_DISCONNECT_CLIENT,
	OP_GETEVENTS,
	OP_SENDEVENT, // todo: multiple?
};

enum connection_mode_t {
	CM_NONE,
	CM_SERVER,
	CM_CLIENT,
};

enum result_t {
	R_NONE,
	R_SUCCESS,
	R_ERROR,
	R_CANCELED,
};

typedef std::vector< Event > events_t;

struct MT_Request {
	op_t op;
	struct {
		connection_mode_t mode;
		std::string remote_address;
	} connect;
	size_t cid;
	Event event;
};

struct MT_Response {
	result_t result;
	std::string message;
	events_t events;
};

typedef base::MTModule< MT_Request, MT_Response > MTModule;

CLASS( Network, MTModule )

	mt_id_t MT_Connect( const connection_mode_t connect_mode, const std::string& remote_address = "" );
	mt_id_t MT_Disconnect();
	mt_id_t MT_DisconnectClient( const size_t cid );

	mt_id_t MT_GetEvents();
	mt_id_t MT_SendEvent( const Event& event );

	mt_id_t MT_SendPacket( const Packet& packet, const size_t cid = 0 );

	MT_Response MT_GetResult( mt_id_t mt_id );

	void Iterate() override;

protected:

	static const int GLSMAC_PORT = 4888;
	static const int GLSMAC_MAX_INCOMING_CONNECTIONS = 64;

	// should be sufficient to fit any packet
	static const int BUFFER_SIZE = 65536;

	// shared structures to minimize reallocations

	struct {
		int tmpint;
		int tmpint2;
		Event event;
		char* ptr;
		time_t now;
		time_t time;
		char c;
	} m_tmp = {};

	struct local_socket_data_t {
		std::string local_address;
		fd_t fd;
	};

	struct remote_socket_data_t {
		std::string remote_address;
		fd_t fd;
		size_t cid;
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
		std::unordered_map< fd_t, local_socket_data_t > listening_sockets;
		std::unordered_map< fd_t, remote_socket_data_t > client_sockets;
		std::vector< fd_t > cid_to_fd;
		struct {
			void* client_addr;
			fd_t newfd;
			std::vector< int > to_remove;
		} tmp;
	} m_server = {};

	struct {
		remote_socket_data_t socket;
	} m_client = {};

	CLASS( Impl, base::Base )
	public:
		Impl();
		~Impl();
		void Start();
		void Stop();

		typedef int ec_t;
		const ec_t GetLastErrorCode() const;
		bool IsSocketInvalid( const fd_t fd ) const;
		void ConfigureSocket( const fd_t socket ) const;
		bool IsConnectionSucceeded( const ec_t ec ) const;
		bool IsConnectionIdle( const ec_t ec ) const;
		const std::string GetErrorMessage( const ec_t ec ) const;
		int Receive( const fd_t fd, void* buf, const int len ) const;
		int Send( const fd_t fd, const void* buf, const int len ) const;
		void CloseSocket( const fd_t fd ) const;
	};

	Impl m_impl = {};

	virtual MT_Response ListenStart() = 0;
	virtual MT_Response ListenStop() = 0;
	virtual MT_Response Connect( const std::string& remote_address, MT_CANCELABLE ) = 0;
	virtual MT_Response Disconnect() = 0;
	virtual MT_Response DisconnectClient( const size_t cid ) = 0;
	virtual void ProcessEvents() = 0;

	const MT_Response ProcessRequest( const MT_Request& request, MT_CANCELABLE ) override;
	void DestroyRequest( const MT_Request& request ) override;
	void DestroyResponse( const MT_Response& response ) override;

	const MT_Response Error( const std::string& errmsg = "" ) const;
	const MT_Response Success() const;
	const MT_Response Canceled() const;

	mt_id_t MT_Success();

	void AddEvent( const Event& event );
	events_t GetEvents();
	void InvalidateEventsForDisconnectedClient( const size_t cid );

	const connection_mode_t GetCurrentConnectionMode() const;

	const fd_t GetFdFromCid( const size_t cid ) const;

private:
	connection_mode_t m_current_connection_mode = CM_NONE;

	events_t m_events_out = {}; // from network to other modules
	events_t m_events_in = {}; // from other modules to network

};

}
