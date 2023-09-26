#include "State.h"

namespace game {

State::State() {
	//
}

State::~State() {
	Reset();
}

void State::Iterate() {
	if ( m_connection ) {
		m_connection->Iterate();
	}
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
		ASSERT( m_slots.GetSlot( it.second ).GetPlayer() != player, "player still in cid slots" );
	}
#endif
	m_players.erase( player );
}

void State::AddCIDSlot( const size_t cid, const size_t slot ) {
	Log( "adding CID " + std::to_string( cid ) + " for slot " + std::to_string( slot ) );
#ifdef DEBUG
	ASSERT( m_cid_slots.find( cid ) == m_cid_slots.end(), "duplicate cid add" );
	for ( auto& it : m_cid_slots ) {
		ASSERT( it.second != slot, "slot " + std::to_string( slot ) + " already in cids" );
	}
#endif
	m_cid_slots[ cid ] = slot;
}

void State::RemoveCIDSlot( const size_t cid ) {
	Log( "removing CID " + std::to_string( cid ) );
	ASSERT( m_cid_slots.find( cid ) != m_cid_slots.end(), "cid not found" );
	m_cid_slots.erase( cid );
}

const std::unordered_map< size_t, size_t >& State::GetCidSlots() const {
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

void State::Reset() {
	if ( m_connection ) {
		if ( m_connection->IsConnected() ) {
			auto* connection = m_connection;
			m_connection->m_on_disconnect = [ connection ]() -> void {
				// TODO: do this synchronously?
				DELETE( connection );
			};
			m_connection->Disconnect();
		}
		m_connection = nullptr;
	}
	for ( auto& player : m_players ) {
		DELETE( player );
	}
	m_players.clear();
	m_slots.Clear();
	m_cid_slots.clear();
}

void State::DetachConnection() {
	ASSERT( m_connection, "state connection not set" );
	m_connection = nullptr;
}

}
