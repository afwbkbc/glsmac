#include "Connection.h"

#include "game/backend/State.h"
#include "engine/Engine.h"
#include "network/Network.h"
#include "ui_legacy/UI.h"
#include "game/backend/event/Event.h"

namespace game {
namespace backend {
namespace connection {

void Connection::SetState( State* state ) {
	/*if ( m_state ) {
		m_state->DetachConnection();
	}*/
	m_state = state;
}

void Connection::ResetHandlers() {
	if ( m_f_on_open ) {
		Unpersist( m_f_on_open );
		m_f_on_open = nullptr;
	}
	m_on_connect = nullptr;
	m_on_cancel = nullptr;
	m_on_disconnect = nullptr;
	m_on_error = nullptr;
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

Connection::Connection( gc::Space* const gc_space, const network::connection_mode_t connection_mode, settings::LocalSettings* const settings )
	: gse::GCWrappable( gc_space )
	, m_gc_space( gc_space )
	, m_connection_mode( connection_mode )
	, m_settings( settings )
	, m_network( g_engine->GetNetwork() ) {
	//
}

Connection::~Connection() {
	if ( m_mt_ids.disconnect ) {
		for ( uint8_t tries = 25 ; tries > 0 ; tries-- ) {
			IterateAndMaybeDelete();
			if ( m_mt_ids.disconnect ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( 40 ) );
			}
			else {
				break;
			}
		}
		if ( m_mt_ids.disconnect ) {
			Log( "WARNING: connection destroyed while still disconnecting!" );
		}
	}
	if ( m_mt_ids.events ) {
		m_network->MT_Cancel( m_mt_ids.events );
	}
	/* if ( m_state ) {
		m_state->DetachConnection();
	}*/
	ClearPending();
}

void Connection::Connect() {
	ASSERT( !m_is_connected, "already connected" );
	ASSERT( !m_mt_ids.connect, "connection already in progress" );

	m_is_canceled = false;

	m_game_state = GS_NONE;

	Log( "Connecting..." );

	m_mt_ids.connect = m_network->MT_Connect( m_connection_mode, m_settings->remote_address );

	/*g_engine->GetUI()->ShowLoader(
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
	);*/
}

const bool Connection::IterateAndMaybeDelete() {

	if ( m_mt_ids.disconnect ) {
		bool should_delete = false;
		auto result = m_network->MT_GetResult( m_mt_ids.disconnect );
		if ( result.result != network::R_NONE ) {
			m_mt_ids.disconnect = 0;
			m_game_state = GS_NONE;
			Log( "Connection closed" );
			if ( m_mt_ids.events ) {
				m_network->MT_Cancel( m_mt_ids.events );
				m_mt_ids.events = 0;
			}
			ClearPending();
			if ( m_is_connected ) {
				WTrigger(
					"disconnect", ARGS_F() {}; }
				);
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
//			if ( should_delete ) {
			return true;
//			}
		}
	}

	if ( m_mt_ids.connect ) {
		auto result = m_network->MT_GetResult( m_mt_ids.connect );
		if ( result.result != network::R_NONE ) {
			m_mt_ids.connect = 0;
			//g_engine->GetUI()->HideLoader();
			switch ( result.result ) {
				case network::R_ERROR: {
					if ( m_f_on_open ) {
						Unpersist( m_f_on_open );
						m_f_on_open = nullptr;
					}
					Log( "Connection error: " + result.message );
					if ( m_on_error ) {
						if ( m_on_error( result.message ) ) {
							return true;
						}
					}
					WTrigger(
						"error", ARGS_F( result ) {
							{
								"message", VALUE( gse::value::String,, result.message )
							}
						}; }
					);
					return true;
				}
				case network::R_SUCCESS: {
					Log( "Connection open" );
					m_is_connected = true;
					if ( m_on_connect ) {
						m_on_connect();
					}
					WTrigger(
						"connect", ARGS_F() {}; }
					);
					break;
				}
				case network::R_CANCELED: {
					if ( m_f_on_open ) {
						Unpersist( m_f_on_open );
						m_f_on_open = nullptr;
					}
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
							return true;
						}
					}
					return false;
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
	return false;
}

Client* Connection::AsClient() const {
	ASSERT( IsClient(), "not client connection" );
	return (Client*)
		this;
}

void Connection::IfClient( std::function< void( Client* ) > cb ) {
	if ( IsClient() ) {
		cb(
			(Client*)
				this
		);
	}
}

Server* Connection::AsServer() const {
	ASSERT( IsServer(), "not server connection" );
	return (Server*)
		this;
}

void Connection::IfServer( std::function< void( Server* ) > cb ) {
	if ( IsServer() ) {
		cb(
			(Server*)
				this
		);
	}
}

void Connection::SendGameEvent( backend::event::Event* event ) {
	if ( m_pending_game_events.size() >= PENDING_GAME_EVENTS_LIMIT ) {
		SendGameEvents( m_pending_game_events );
		m_pending_game_events.clear();
	}
	m_pending_game_events.push_back( event );
}

const bool Connection::IsConnected() const {
	return m_is_connected && m_connection_mode != network::CM_NONE;
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

WRAPIMPL_BEGIN( Connection )
	WRAPIMPL_PROPS
		WRAPIMPL_TRIGGERS
		{
			"open",
			NATIVE_CALL( this ) {

				N_EXPECT_ARGS_MIN_MAX( 0, 1 );
				N_GET_CALLABLE_OPT( f_on_open, 0 );
				if ( f_on_open ) {
					m_f_on_open = f_on_open;
					Persist( m_f_on_open );
				}

				if ( m_is_connected ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Connection already active" );
				}
				if ( !m_mt_ids.connect ) {
					Connect();
				}
				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"close",
			NATIVE_CALL( this ) {

				N_EXPECT_ARGS_MIN_MAX( 0, 1 );

				if ( !m_mt_ids.disconnect ) {

					if ( m_f_on_open ) {
						Unpersist( m_f_on_open );
						m_f_on_open = nullptr;
					}

					if ( arguments.size() >= 1 ) {
						N_GETVALUE( reason, 0, String );
						Disconnect( reason );
					}
					else {
						Disconnect();
					}
				}
				return VALUE( gse::value::Undefined );
			} )
		},
	};
WRAPIMPL_END_PTR()

void Connection::ProcessEvent( const network::LegacyEvent& event ) {
	ASSERT( m_state, "connection state not set" );
}

gc::Space* Connection::GetGCSpace() const {
	ASSERT( m_state, "state not set" );
	ASSERT( m_state->m_gc_space, "state gc space not set" );
	return m_state->m_gc_space;
}

void Connection::WTrigger( const std::string& event, const f_args_t& fargs, const std::function<void()>& f_after ) {
	ASSERT( m_state, "state not set" );
	m_state->WithGSE( this, [ this, event, fargs, f_after ]( GSE_CALLABLE ) {
		Trigger( GSE_CALL, event, fargs );
	}, f_after );
}

void Connection::OnOpen() {
	if ( m_f_on_open ) {
		m_state->WithGSE( this, [ this ]( GSE_CALLABLE ) {
			m_f_on_open->Run( GSE_CALL, {} );
			Unpersist( m_f_on_open );
			m_f_on_open = nullptr;
		});
	}
}

void Connection::Disconnect( const std::string& reason ) {
	if ( !reason.empty() ) {
		m_disconnect_reason = reason;
	}
	if ( m_mt_ids.disconnect ) {
		return; // already disconnecting
	}
	Log(
		"Disconnecting" + ( !reason.empty()
			? " (reason: " + reason + ")"
			: ""
		)
	);
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
}
