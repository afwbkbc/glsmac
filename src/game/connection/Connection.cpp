#include "Connection.h"

#include "game/State.h"
#include "engine/Engine.h"
#include "network/Network.h"
#include "ui/UI.h"
#include "game/event/Event.h"

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
	m_on_flags_update = nullptr;
	m_on_message = nullptr;
	m_on_global_settings_update = nullptr;
	m_on_game_event_validate = nullptr;
	m_on_game_event_apply = nullptr;
	if ( m_mt_ids.events ) {
		m_network->MT_Cancel( m_mt_ids.events );
		m_mt_ids.events = 0;
	}
}

Connection::Connection( const network::connection_mode_t connection_mode, settings::LocalSettings* const settings )
	: m_connection_mode( connection_mode )
	, m_settings( settings )
	, m_network( g_engine->GetNetwork() ) {
	//
}

Connection::~Connection() {
	if ( m_mt_ids.disconnect ) {
		Log( "WARNING: connection destroyed while still disconnecting!" );
	}
	if ( m_mt_ids.events ) {
		m_network->MT_Cancel( m_mt_ids.events );
	}
	ClearPending();
}

void Connection::Connect() {
	ASSERT( !m_is_connected, "already connected" );
	ASSERT( !m_mt_ids.connect, "connection already in progress" );

	m_is_canceled = false;

	m_game_state = GS_NONE;

	Log( "Connecting..." );

	m_mt_ids.connect = m_network->MT_Connect( m_connection_mode, m_settings->remote_address );

	g_engine->GetUI()->ShowLoader(
		m_connection_mode == network::CM_SERVER
			? "Creating game"
			: "Connecting to " + m_settings->remote_address,
		LCH( this ) {
			ASSERT( m_mt_ids.connect, "connect mt id is zero" );
			m_network->MT_Cancel( m_mt_ids.connect );
			m_mt_ids.connect = 0;
			m_is_canceled = true;
			m_mt_ids.disconnect = m_network->MT_Disconnect();

			return true;
		}
	);
}

void Connection::Iterate() {

	if ( m_mt_ids.disconnect ) {
		bool should_delete = false;
		auto result = m_network->MT_GetResult( m_mt_ids.disconnect );
		if ( result.result != network::R_NONE ) {
			m_mt_ids.disconnect = 0;
			m_game_state = GS_NONE;
			Log( "Connection closed" );
			if ( m_is_connected ) {
				if ( m_on_disconnect ) {
					should_delete |= m_on_disconnect();
				}
			}
			if ( m_is_canceled ) {
				if ( m_on_cancel ) {
					should_delete |= m_on_cancel();
				}
			}
			m_is_connected = false;
			m_is_canceled = false;
			if ( !m_disconnect_reason.empty() && m_on_error ) {
				should_delete |= m_on_error( m_disconnect_reason );
				m_disconnect_reason.clear();
			}
		}
		if ( should_delete ) {
			DELETE( this );
			return;
		}
	}

	if ( m_mt_ids.connect ) {
		auto result = m_network->MT_GetResult( m_mt_ids.connect );
		if ( result.result != network::R_NONE ) {
			m_mt_ids.connect = 0;
			g_engine->GetUI()->HideLoader();
			switch ( result.result ) {
				case network::R_ERROR: {
					Log( "Connection error: " + result.message );
					if ( m_on_error ) {
						if ( m_on_error( result.message ) ) {
							DELETE( this );
							return;
						}
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
					THROW( "unknown network result " + std::to_string( result.result ) );
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
						if ( m_on_error( result.message ) ) {
							DELETE( this );
							return;
						}
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
		ProcessPending();
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

void Connection::SendGameEvent( game::event::Event* event ) {
	if ( m_pending_game_events.size() >= PENDING_GAME_EVENTS_LIMIT ) {
		SendGameEvents( m_pending_game_events );
		m_pending_game_events.clear();
	}
	m_pending_game_events.push_back( event );
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
	ClearPending();
	m_mt_ids.disconnect = m_network->MT_Disconnect();
}

void Connection::ProcessPending() {
	if ( !m_pending_game_events.empty() ) {
		SendGameEvents( m_pending_game_events );
		m_pending_game_events.clear();
	}
}

void Connection::ClearPending() {
	for ( auto& it : m_pending_game_events ) {
		delete it;
	}
}

}
}

