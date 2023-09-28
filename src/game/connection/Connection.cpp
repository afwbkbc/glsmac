#include "Connection.h"

namespace game {
namespace connection {

void Connection::SetState( game::State* state ) {
	if ( m_state ) {
		m_state->DetachConnection();
	}
	m_state = state;
}

void Connection::ResetHandlers() {
	m_on_connect = nullptr;
	m_on_cancel = nullptr;
	m_on_disconnect = nullptr;
	m_on_error = nullptr;
	m_on_player_join = nullptr;
	m_on_player_leave = nullptr;
	m_on_slot_update = nullptr;
	m_on_message = nullptr;
	m_on_global_settings_update = nullptr;
}

Connection::Connection( const network::connection_mode_t connection_mode, LocalSettings* const settings )
	: m_connection_mode( connection_mode )
	, m_settings( settings ) {
	//
}

Connection::~Connection() {
	if ( m_mt_ids.disconnect ) {
		Log( "WARNING: connection destroyed while still disconnecting!" );
	}
	if ( m_mt_ids.events ) {
		m_network->MT_Cancel( m_mt_ids.events );
	}
}

void Connection::Connect() {
	ASSERT( !m_is_connected, "already connected" );
	ASSERT( !m_mt_ids.connect, "connection already in progress" );

	Log( "connecting..." );

	m_mt_ids.connect = m_network->MT_Connect( m_connection_mode, m_settings->remote_address );

	g_engine->GetUI()->GetLoader()->Show(
		m_connection_mode == network::CM_SERVER
			? "Creating game"
			: "Connecting to " + m_settings->remote_address,
		LCH( this ) {
			ASSERT( m_mt_ids.connect, "connect mt id is zero" );
			m_network->MT_Cancel( m_mt_ids.connect );
			m_mt_ids.connect = 0;
			m_network->MT_Disconnect();
			if ( m_on_cancel ) {
				m_on_cancel();
			}
			return true;
		}
	);
}

void Connection::Iterate() {

	if ( m_mt_ids.disconnect ) {
		auto result = m_network->MT_GetResult( m_mt_ids.disconnect );
		if ( result.result != network::R_NONE ) {
			m_mt_ids.disconnect = 0;
			m_is_connected = false;
			Log( "Connection closed" );
			if ( m_on_disconnect ) {
				m_on_disconnect();
			}
			if ( !m_disconnect_reason.empty() && m_on_error ) {
				m_on_error( m_disconnect_reason );
				m_disconnect_reason.clear();
			}
		}
	}

	if ( m_mt_ids.connect ) {
		auto result = m_network->MT_GetResult( m_mt_ids.connect );
		if ( result.result != network::R_NONE ) {
			m_mt_ids.connect = 0;
			g_engine->GetUI()->GetLoader()->Hide();
			switch ( result.result ) {
				case network::R_ERROR: {
					Log( "Connection error: " + result.message );
					if ( m_on_error ) {
						m_on_error( result.message );
					}
					break;
				}
				case network::R_SUCCESS: {
					Log( "Connection open" );
					m_is_connected = true;
					if ( m_on_connect ) {
						m_on_connect();
					}
					break;
				}
				case network::R_CANCELED: {
					Log( "Connection canceled" );
					break;
				}
				default: {
					ASSERT( false, "unknown network result " + std::to_string( result.result ) );
				}
			}
		}
	}
	else if ( m_is_connected ) {
		if ( !m_mt_ids.events ) {
			m_mt_ids.events = m_network->MT_GetEvents();
		}
		else {
			auto result = m_network->MT_GetResult( m_mt_ids.events );
			if ( result.result != network::R_NONE ) {
				m_mt_ids.events = 0;
				if ( result.result == network::R_ERROR ) {
					Log( "received error event" );
					if ( m_on_error ) {
						m_on_error( result.message );
					}
				}
				else if ( result.result == network::R_SUCCESS ) {
					if ( !result.events.empty() ) {
						Log( "got " + std::to_string( result.events.size() ) + " event(s)" );
						for ( auto& event : result.events ) {
							ProcessEvent( event );
						}
					}
				}
			}
		}
	}
}

Client* Connection::AsClient() const {
	ASSERT( IsClient(), "not client connection" );
	return (Client*)this;
}

void Connection::IfClient( std::function< void( Client* ) > cb ) {
	if ( IsClient() ) {
		cb( (Client*)this );
	}
}

Server* Connection::AsServer() const {
	ASSERT( IsServer(), "not server connection" );
	return (Server*)this;
}

void Connection::IfServer( std::function< void( Server* ) > cb ) {
	if ( IsServer() ) {
		cb( (Server*)this );
	}
}

const bool Connection::IsConnected() const {
	return m_connection_mode != network::CM_NONE;
}

const bool Connection::IsServer() const {
	return m_connection_mode == network::CM_SERVER;
}

const bool Connection::IsClient() const {
	return m_connection_mode == network::CM_CLIENT;
}

const size_t Connection::GetSlotNum() const {
	return m_slot;
}

const Player* Connection::GetPlayer() const {
	return m_player;
}

void Connection::ProcessEvent( const network::Event& event ) {
	ASSERT( m_state, "connection state not set" );
}

void Connection::Disconnect( const std::string& reason ) {
	if ( !reason.empty() ) {
		m_disconnect_reason = reason;
	}
	if ( m_mt_ids.disconnect ) {
		return; // already disconnecting
	}
	if ( m_mt_ids.events ) {
		m_network->MT_Cancel( m_mt_ids.events );
		m_mt_ids.events = 0;
	}
	Log(
		"Disconnecting" + ( !reason.empty()
			? " (reason: " + reason + ")"
			: ""
		)
	);
	m_mt_ids.disconnect = m_network->MT_Disconnect();
}

}
}

