#include "Network.h"

namespace network {

mt_id_t Network::MT_Connect( const connection_mode_t connect_mode, const std::string& remote_address ) {
	MT_Request request;
	request.op = OP_CONNECT;
	request.connect.mode = connect_mode;
	request.connect.remote_address = remote_address;
	return MT_CreateRequest( request );
}

mt_id_t Network::MT_Disconnect() {
	MT_Request request;
	request.op = OP_DISCONNECT;
	return MT_CreateRequest( request );
}

mt_id_t Network::MT_DisconnectClient( const size_t cid ) {
	MT_Request request;
	request.op = OP_DISCONNECT_CLIENT;
	request.cid = cid;
	return MT_CreateRequest( request );
}

mt_id_t Network::MT_GetEvents() {
	MT_Request request;
	request.op = OP_GETEVENTS;
	return MT_CreateRequest( request );
}

mt_id_t Network::MT_SendEvent( const Event& event ) {
	MT_Request request;
	request.op = OP_SENDEVENT;
	request.event = event;
	return MT_CreateRequest( request );
}

mt_id_t Network::MT_SendPacket( const Packet& packet, const size_t cid ) {
	if ( m_current_connection_mode == CM_NONE ) {
		// maybe old event, nothing to do
		return MT_Success();
	}
	ASSERT( 
		( m_current_connection_mode == CM_SERVER && cid ) ||
		( m_current_connection_mode == CM_CLIENT && !cid ),
		"unexpected cid value for connection mode"
	);
	Event e;
	e.cid = cid;
	e.type = Event::ET_PACKET;
	e.data.packet_data = packet.Serialize().ToString();
	Log( "Sending packet ( type = " + std::to_string( packet.type ) + " )" + ( cid ? " to client " + std::to_string( cid ) : "" ) );
	return MT_SendEvent( e );
}

const MT_Response Network::ProcessRequest( const MT_Request& request, MT_CANCELABLE ) {
	// TODO: check if canceled
	switch ( request.op ) {
		case OP_SUCCESS: {
			return Success();
		}
		case OP_CONNECT: {
			if ( m_current_connection_mode != CM_NONE ) {
				Log( "Existing connection detected, trying disconnect" );
				MT_Request dcrequest;
				dcrequest.op = OP_DISCONNECT;
				auto dcres = ProcessRequest( dcrequest, MT_C );
				if ( dcres.result == R_ERROR ) {
					return dcres;
				}
			}
			switch ( request.connect.mode ) {
				case CM_SERVER: {
					auto response = ListenStart();
					if ( response.result == R_SUCCESS ) {
						m_current_connection_mode = request.connect.mode;
					}
					return response;
				}
				case CM_CLIENT: {
					auto response = Connect( request.connect.remote_address );
					if ( response.result == R_SUCCESS ) {
						m_current_connection_mode = request.connect.mode;
					}
					return response;
				}
				default: {
					ASSERT( false, "invalid mode on connect" );
				}
			}
			break;
		}
		case OP_DISCONNECT: {
			if ( m_current_connection_mode == CM_NONE ) {
				return Success(); // already disconnected
			}
			switch ( m_current_connection_mode ) {
				case CM_SERVER: {
					auto response = ListenStop();
					if ( response.result == R_SUCCESS ) {
						m_current_connection_mode = CM_NONE;
					}
					return response;
				}
				case CM_CLIENT: {
					auto response = Disconnect();
					if ( response.result == R_SUCCESS ) {
						m_current_connection_mode = CM_NONE;
					}
					return response;
				}
				default: {
					ASSERT( false, "invalid mode on disconnect" );
				}
			}
			break;
		}
		case OP_DISCONNECT_CLIENT: {
			MT_Response response;
			response.result = R_SUCCESS;
			if ( m_current_connection_mode == CM_SERVER ) {
				m_events_in.push_back( request.event );
			}
			return response;
		}
		case OP_GETEVENTS: {
			MT_Response response;
			response.result = R_SUCCESS;
			response.events = m_events_out;
			m_events_out.clear();
			return response;
		}
		case OP_SENDEVENT: {
			MT_Response response;
			response.result = R_SUCCESS;
			m_events_in.push_back( request.event );
			return response;
		}
		default: {
			ASSERT( false, "unknown network request " + std::to_string( request.op ) );
		}
	}
	return Error();
}

void Network::DestroyRequest( const MT_Request& request ) {
	
}

void Network::DestroyResponse( const MT_Response& response ) {
	
}
	
void Network::AddEvent( const Event& event ) {
	m_events_out.push_back( event );
}

events_t Network::GetEvents() {
	events_t events = m_events_in;
	m_events_in.clear();
	return events;
}

MT_Response Network::MT_GetResult( mt_id_t mt_id ) {
	return MT_GetResponse( mt_id );
}

const MT_Response Network::Error( const std::string& errmsg ) {
	MT_Response response;
	response.result = R_ERROR;
	response.message = errmsg;
	return response;
}

const MT_Response Network::Success() {
	MT_Response response;
	response.result = R_SUCCESS;
	return response;
}

mt_id_t Network::MT_Success() {
	MT_Request request;
	request.op = OP_SUCCESS;
	return MT_CreateRequest( request );
}

}
