#include "Network.h"

#include "types/Packet.h"

namespace network {

common::mt_id_t Network::MT_Connect( const connection_mode_t connect_mode, const std::string& remote_address ) {
	ASSERT( !( connect_mode == CM_CLIENT && remote_address.empty() ), "client connection without remote address" );
	MT_Request request;
	request.op = OP_CONNECT;
	request.connect.mode = connect_mode;
	request.connect.remote_address = remote_address;
	return MT_CreateRequest( request );
}

common::mt_id_t Network::MT_Disconnect() {
	MT_Request request;
	request.op = OP_DISCONNECT;
	return MT_CreateRequest( request );
}

common::mt_id_t Network::MT_DisconnectClient( const network::cid_t cid ) {
	MT_Request request;
	request.op = OP_DISCONNECT_CLIENT;
	request.cid = cid;
	return MT_CreateRequest( request );
}

common::mt_id_t Network::MT_GetEvents() {
	MT_Request request;
	request.op = OP_GETEVENTS;
	return MT_CreateRequest( request );
}

common::mt_id_t Network::MT_SendEvent( const Event& event ) {
	MT_Request request;
	request.op = OP_SENDEVENT;
	request.event = event;
	return MT_CreateRequest( request );
}

common::mt_id_t Network::MT_SendPacket( const types::Packet* packet, const network::cid_t cid ) {
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
	e.data.packet_data = packet->Serialize().ToString();
	/*Log(
		"Sending packet ( type = " + std::to_string( packet->type ) + " )" + ( cid
			? " to client " + std::to_string( cid )
			: ""
		)
	);*/
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
			m_events_in.clear();
			m_events_out.clear();
			switch ( request.connect.mode ) {
				case CM_SERVER: {
					auto response = ListenStart();
					if ( response.result == R_SUCCESS ) {
						m_current_connection_mode = request.connect.mode;
					}
					return response;
				}
				case CM_CLIENT: {
					auto response = Connect( request.connect.remote_address, MT_C );
					if ( response.result == R_SUCCESS ) {
						m_current_connection_mode = request.connect.mode;
					}
					return response;
				}
				default: {
					THROW( "invalid mode on connect" );
				}
			}
			break;
		}
		case OP_DISCONNECT: {
			if ( GetCurrentConnectionMode() != CM_NONE ) {
				Event event;
				event.type = Event::ET_DISCONNECT;
				m_events_in.push_back( event );
				ProcessEvents();
				m_current_connection_mode = CM_NONE;
			}
			return Success();
		}
		case OP_DISCONNECT_CLIENT: {
			if ( m_current_connection_mode == CM_SERVER ) {
				Event event;
				event.type = Event::ET_CLIENT_DISCONNECT;
				event.cid = request.cid;
				m_events_in.push_back( event );
				ProcessEvents();
			}
			return Success();
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
			THROW( "unknown network request " + std::to_string( request.op ) );
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

void Network::InvalidateEventsForDisconnectedClient( const network::cid_t cid ) {
	ASSERT( cid != 0, "can't invalidate cid 0" );
	events_t events_new = {};
	bool disconnect_event_kept = false;
	for ( auto& event : m_events_out ) {
		if ( !disconnect_event_kept && event.type == Event::ET_CLIENT_DISCONNECT ) {
			disconnect_event_kept = true;
		}
		else if ( event.cid == cid ) {
			continue;
		}
		events_new.push_back( event );
	}
	if ( m_events_out.size() != events_new.size() ) {
		Log( "Invalidated " + std::to_string( m_events_out.size() - events_new.size() ) + " events for cid " + std::to_string( cid ) );
	}
	m_events_out = events_new;
}

const connection_mode_t Network::GetCurrentConnectionMode() const {
	return m_current_connection_mode;
}

MT_Response Network::MT_GetResult( common::mt_id_t mt_id ) {
	return MT_GetResponse( mt_id );
}

void Network::Iterate() {
	MTModule::Iterate();
	ProcessEvents();
}

const MT_Response Network::Error( const std::string& errmsg ) const {
	MT_Response response;
	response.result = R_ERROR;
	response.message = errmsg;
	return response;
}

const MT_Response Network::Success() const {
	MT_Response response;
	response.result = R_SUCCESS;
	return response;
}

const MT_Response Network::Canceled() const {
	MT_Response response;
	response.result = R_CANCELED;
	return response;
}

common::mt_id_t Network::MT_Success() {
	MT_Request request;
	request.op = OP_SUCCESS;
	return MT_CreateRequest( request );
}

const fd_t Network::GetFdFromCid( const network::cid_t cid ) const {
	const auto& it = m_server.cid_to_fd.find( cid );
	if ( it == m_server.cid_to_fd.end() ) {
		return 0;
	}
	else {
		return it->second;
	}
}

}
