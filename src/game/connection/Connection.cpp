#include "Connection.h"

namespace game {
namespace connection {

void Connection::SetState( game::State *state ) {
	ASSERT( !m_state, "connection state already set" );
	m_state = state;
}

Connection::Connection( const network::connection_mode_t connection_mode, LocalSettings* const settings )
	: m_connection_mode( connection_mode )
	, m_settings( settings )
{
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
				default: {
					ASSERT( false, "unknown network result " + std::to_string( result.result ) );
				}
			}
		}
	}

	if ( m_is_connected ) {
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
		return;
	}
	if ( m_mt_ids.events ) {
		m_network->MT_Cancel( m_mt_ids	.events );
		m_mt_ids.events = 0;
	}
	m_mt_ids.disconnect = g_engine->GetNetwork()->MT_Disconnect();
}



}
}

