#pragma once

#include "base/MTModule.h"

#include "Event.h"

#include "types/Packet.h"

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

protected:
	
	virtual MT_Response ListenStart() = 0;
	virtual MT_Response ListenStop() = 0; 
	virtual MT_Response Connect( const std::string& remote_address ) = 0;
	virtual MT_Response Disconnect() = 0;
	virtual MT_Response DisconnectClient( const size_t cid ) = 0;
	
	const MT_Response ProcessRequest( const MT_Request& request, MT_CANCELABLE );
	void DestroyRequest( const MT_Request& request );
	void DestroyResponse( const MT_Response& response );
	
	const MT_Response Error( const std::string& errmsg = "" );
	const MT_Response Success();
	
	mt_id_t MT_Success();
	
	void AddEvent( const Event& event );
	events_t GetEvents();
	
private:
	connection_mode_t m_current_connection_mode = CM_NONE;
	
	events_t m_events_out = {}; // from network to other modules
	events_t m_events_in = {}; // from other modules to network
};

}
