#pragma once

#include "base/MTModule.h"

#include "Types.h"

#include "Event.h"

namespace types {
class Packet;
}

namespace network {

CLASS( Network, MTModule )

	base::mt_id_t MT_Connect( const connection_mode_t connect_mode, const std::string& remote_address = "" );
	base::mt_id_t MT_Disconnect();
	base::mt_id_t MT_DisconnectClient( const cid_t cid );

	base::mt_id_t MT_GetEvents();
	base::mt_id_t MT_SendEvent( const Event& event );

	base::mt_id_t MT_SendPacket( const types::Packet* packet, const cid_t cid = 0 );

	MT_Response MT_GetResult( base::mt_id_t mt_id );

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
		std::string remote_address = "";
		fd_t fd = 0;
		cid_t cid = 0;
		struct {
			char* data1 = nullptr;
			char* data2 = nullptr;
			char* data = nullptr;
			char* ptr = nullptr;
			size_t len = 0;
		} buffer = {};
		time_t last_data_at = 0;
		bool ping_needed = false;
		bool ping_sent = false;
		bool pong_needed = false;
	};

	struct {
		std::unordered_map< fd_t, local_socket_data_t > listening_sockets = {};
		std::unordered_map< fd_t, remote_socket_data_t > client_sockets = {};
		std::unordered_map< cid_t, fd_t > cid_to_fd = {};
		cid_t next_cid = 1; // 0 is reserved for server
		struct {
			void* client_addr = nullptr;
			fd_t newfd = 0;
			std::vector< int > to_remove = {};
		} tmp = {};
	} m_server = {};

	struct {
		remote_socket_data_t socket = {};
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
	virtual MT_Response DisconnectClient( const cid_t cid ) = 0;
	virtual void ProcessEvents() = 0;

	const MT_Response ProcessRequest( const MT_Request& request, MT_CANCELABLE ) override;
	void DestroyRequest( const MT_Request& request ) override;
	void DestroyResponse( const MT_Response& response ) override;

	const MT_Response Error( const std::string& errmsg = "" ) const;
	const MT_Response Success() const;
	const MT_Response Canceled() const;

	base::mt_id_t MT_Success();

	void AddEvent( const Event& event );
	events_t GetEvents();
	void InvalidateEventsForDisconnectedClient( const cid_t cid );

	const connection_mode_t GetCurrentConnectionMode() const;

	const fd_t GetFdFromCid( const cid_t cid ) const;

private:
	connection_mode_t m_current_connection_mode = CM_NONE;

	events_t m_events_out = {}; // from network to other modules
	events_t m_events_in = {}; // from other modules to network

};

}
