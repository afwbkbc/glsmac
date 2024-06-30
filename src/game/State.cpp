#include "State.h"

#include "Game.h"
#include "connection/Connection.h"
#include "bindings/Bindings.h"
#include "slot/Slots.h"
#include "Player.h"

namespace game {

State::State()
	: m_slots( new slot::Slots( this ) ) {

}

State::~State() {
	Reset();
	if ( m_bindings ) {
		delete m_bindings;
	}
	delete m_slots;
}

void State::SetGame( Game* game ) {
	ASSERT( !m_game, "game already set" );
	m_game = game;
	m_on_gse_error = [ this ]( gse::Exception& e ) -> void {
		m_game->OnGSEError( e );
	};
}

void State::UnsetGame() {
	m_game = nullptr;
	m_on_gse_error = nullptr;
}

Game* State::GetGame() const {
	return m_game;
}

void State::Iterate() {
	if ( m_connection ) {
		m_connection->Iterate();
	}
}

bool State::IsMaster() const {
	return !m_connection || m_connection->IsServer();
}

bool State::IsSlave() const {
	return m_connection && m_connection->IsClient();
}

void State::AddPlayer( Player* player ) {
	Log( "adding player: " + player->GetPlayerName() );
	ASSERT( m_players.find( player ) == m_players.end(), "duplicate player add" );
	m_players.insert( player );
}

void State::RemovePlayer( Player* player ) {
	Log( "removing player: " + player->GetPlayerName() );
#ifdef DEBUG
	ASSERT( m_players.find( player ) != m_players.end(), "player not found" );
	for ( auto& it : m_cid_slots ) {
		ASSERT( m_slots->GetSlot( it.second ).GetPlayer() != player, "player still in cid slots" );
	}
#endif
	m_players.erase( player );
}

void State::AddCIDSlot( const network::cid_t cid, const size_t slot ) {
	Log( "adding CID " + std::to_string( cid ) + " for slot " + std::to_string( slot ) );
#ifdef DEBUG
	ASSERT( m_cid_slots.find( cid ) == m_cid_slots.end(), "duplicate cid add" );
	for ( auto& it : m_cid_slots ) {
		ASSERT( it.second != slot, "slot " + std::to_string( slot ) + " already in cids" );
	}
#endif
	m_cid_slots[ cid ] = slot;
}

void State::RemoveCIDSlot( const network::cid_t cid ) {
	Log( "removing CID " + std::to_string( cid ) );
	ASSERT( m_cid_slots.find( cid ) != m_cid_slots.end(), "cid not found" );
	m_cid_slots.erase( cid );
}

const std::unordered_map< network::cid_t, size_t >& State::GetCidSlots() const {
	return m_cid_slots;
}

void State::SetConnection( connection::Connection* connection ) {
	ASSERT( !m_connection, "state connection already set" );
	connection->SetState( this );
	m_connection = connection;
}

connection::Connection* State::GetConnection() const {
	return m_connection;
}

void State::InitBindings() {
	if ( !m_bindings ) {
		Log( "Initializing bindings" );
		m_bindings = new bindings::Bindings( this );
		try {
			m_bindings->RunMain();
		}
		catch ( gse::Exception& err ) {
			if ( m_game ) {
				m_game->OnGSEError( err );
			}
			else {
				throw std::runtime_error( err.ToStringAndCleanup() );
			}
		}
	}
}

void State::Configure() {
	ASSERT( m_bindings, "bindings not initialized" );

	Log( "Configuring state" );

	// reset
	m_settings.global.game_rules.m_factions.clear();
	m_settings.global.game_rules.m_factions_order.clear();

	// configure
	m_bindings->Call( ::game::bindings::Bindings::CS_ON_CONFIGURE );

	// check
	ASSERT( !m_settings.global.game_rules.m_factions.empty(), "no factions were defined" );
	ASSERT( m_settings.global.game_rules.m_factions_order.size() == m_settings.global.game_rules.m_factions.size(), "factions order size mismatch" );
}

void State::Reset() {
	if ( m_connection ) {
		if ( m_connection->IsConnected() ) {
			auto* connection = m_connection;
			m_connection->m_on_disconnect = [ connection ]() -> bool {
				// TODO: do this synchronously?
				return true;
			};
			m_connection->Disconnect();
		}
		m_connection = nullptr;
	}
	for ( auto& player : m_players ) {
		DELETE( player );
	}
	m_players.clear();
	m_slots->Clear();
	m_cid_slots.clear();
	m_game = nullptr;
	m_on_gse_error = nullptr;
}

void State::DetachConnection() {
	ASSERT( m_connection, "state connection not set" );
	m_connection = nullptr;
}

}
