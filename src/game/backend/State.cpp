#include "State.h"

#include "game/backend/faction/FactionManager.h"
#include "Game.h"
#include "game/backend/connection/Connection.h"
#include "Bindings.h"
#include "game/backend/slot/Slots.h"
#include "Player.h"
#include "GLSMAC.h"
#include "gc/Space.h"

namespace game {
namespace backend {

State::State( gc::Space* const gc_space, gse::context::Context* const ctx, GLSMAC* glsmac )
	: gse::GCWrappable( gc_space )
	, m_gc_space( gc_space )
	, m_ctx( ctx )
	, m_glsmac( glsmac )
	, m_slots( new slot::Slots( this ) ) {
	NEW( m_fm, faction::FactionManager );
}

State::~State() {
	Reset();
	if ( m_bindings ) {
		delete m_bindings;
	}
	delete m_slots;
	DELETE( m_fm );
}

void State::SetGame( Game* game ) {
	{
		std::lock_guard guard( m_gc_mutex );
		ASSERT( !m_game, "game already set" );
		m_game = game;
	}
	m_on_gse_error = [ this ]( const gse::Exception& e ) -> void {
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

void State::WithGSE( const gse_func_t& f ) {
	m_gc_space->Accumulate(
		[ this, &f ]() {
			gse::ExecutionPointer ep;
			f( m_gc_space, m_ctx, {}, ep );
		}
	);
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
#if defined( DEBUG ) || defined( FASTDEBUG )
	ASSERT( m_players.find( player ) != m_players.end(), "player not found" );
	for ( auto& it : m_cid_slots ) {
		ASSERT( m_slots->GetSlot( it.second ).GetPlayer() != player, "player still in cid slots" );
	}
#endif
	m_players.erase( player );
}

void State::AddCIDSlot( const network::cid_t cid, const size_t slot ) {
	Log( "adding CID " + std::to_string( cid ) + " for slot " + std::to_string( slot ) );
#if defined( DEBUG ) || defined( FASTDEBUG )
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
	ASSERT_NOLOG( !m_glsmac, "do not use state bindings with new ui" );
	if ( !m_bindings ) {
		ASSERT_NOLOG( !m_gc_space, "init bindings but gc space already set" );
		Log( "Initializing bindings" );
		m_bindings = new Bindings( this );
		m_gc_space = m_bindings->GetGCSpace();
		m_ctx = m_bindings->GetContext();
		try {
			m_bindings->RunMainScript();
		}
		catch ( gse::Exception& err ) {
			if ( m_game ) {
				m_game->OnGSEError( err );
			}
			else {
				throw std::runtime_error( err.ToString() );
			}
		}
	}
}

void State::Configure() {
	ASSERT( m_bindings, "bindings not initialized" );

	Log( "Configuring state" );

	m_fm->Clear();

	m_bindings->RunMain();

	if ( m_fm->GetAll().empty() ) {
		THROW( "no factions were defined" );
	}
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

faction::FactionManager* State::GetFM() const {
	return m_fm;
}

gse::Value* const State::TriggerObject( gse::GCWrappable* object, const std::string& event, const f_args_t& f_args ) {
	if ( m_glsmac ) {
		// new ui
		return m_glsmac->TriggerObject( object, event, f_args );
	}
	else {
		// legacy
		return m_bindings->Trigger( object, event, f_args );
	}
}

WRAPIMPL_BEGIN( State )
	WRAPIMPL_PROPS
		WRAPIMPL_TRIGGERS
		{
			"fm",
			m_fm->Wrap( GSE_CALL, true )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( State )

const types::Buffer State::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_settings.global.Serialize().ToString() );
	buf.WriteString( m_fm->Serialize().ToString() );

	return buf;
}

void State::Unserialize( types::Buffer buf ) {
	m_settings.global.Unserialize( buf.ReadString() );
	m_fm->Unserialize( buf.ReadString() );
}

void State::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	gse::GCWrappable::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "State" );

	{
		std::lock_guard guard( m_gc_mutex );
		if ( m_game ) {
			GC_DEBUG_BEGIN( "game" );
			GC_REACHABLE( m_game );
			GC_DEBUG_END();
		}
	}

	GC_DEBUG_END();
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string State::ToString() {
	return "game::State()";
}
#endif



}
}
