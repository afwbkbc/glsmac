#pragma once

#include "base/MTModule.h"

#include "Event.h"
#include "types/Packet.h"
#include "impl/Impl.h"

#ifdef _WIN32
#define fd_t unsigned long long
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
};

typedef std::vector<Event> events_t;

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

	CLASS( Impl, base::Base )
		public:
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
				std::unordered_map< int, local_socket_data_t > listening_sockets;
				std::unordered_map< int, remote_socket_data_t > client_sockets;
				struct {
					void* client_addr;
					fd_t newfd;
					std::vector< int > to_remove;
				} tmp;
			} m_server = {};

			struct {
				remote_socket_data_t socket;
			} m_client = {};

			Impl();
			~Impl();
			void Start();
			void Stop();
			std::string Listen(); // error or empty
			std::string Connect( const std::string& remote_address ); // error or empty
			std::string Accept( const int cid );
			int Receive( const fd_t fd, void *buf, const int len );
			int Send( const fd_t fd, const void *buf, const int len );
			void Close( const fd_t fd );

	};

	Impl m_impl = {};

	virtual MT_Response ListenStart() = 0;
	virtual MT_Response ListenStop() = 0;
	virtual MT_Response Connect( const std::string& remote_address ) = 0;
	virtual MT_Response Disconnect() = 0;
	virtual MT_Response DisconnectClient( const size_t cid ) = 0;
	virtual void ProcessEvents() = 0;

	const MT_Response ProcessRequest( const MT_Request& request, MT_CANCELABLE ) override;
	void DestroyRequest( const MT_Request& request ) override;
	void DestroyResponse( const MT_Response& response ) override;

	const MT_Response Error( const std::string& errmsg = "" );
	const MT_Response Success();

	mt_id_t MT_Success();

	void AddEvent( const Event& event );
	events_t GetEvents();

	const connection_mode_t GetCurrentConnectionMode() const;

private:
	connection_mode_t m_current_connection_mode = CM_NONE;

	events_t m_events_out = {}; // from network to other modules
	events_t m_events_in = {}; // from other modules to network
};

}
