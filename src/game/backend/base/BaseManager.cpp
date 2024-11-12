#include "BaseManager.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/bindings/Bindings.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/event/SpawnBase.h"
#include "game/backend/Player.h"
#include "game/backend/faction/Faction.h"

#include "Base.h"
#include "PopDef.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/type/Bool.h"
#include "gse/type/Float.h"
#include "gse/type/Array.h"

namespace game {
namespace backend {
namespace base {

BaseManager::BaseManager( Game* game )
	: m_game( game ) {
	//
}

BaseManager::~BaseManager() {
	Clear();
}

void BaseManager::Clear() {
	for ( auto& it : m_base_popdefs ) {
		delete it.second;
	}
	m_base_popdefs.clear();
	for ( auto& it : m_bases ) {
		delete it.second;
	}
	m_bases.clear();

	m_base_updates.clear();
}

base::PopDef* BaseManager::GetPopDef( const std::string& id ) const {
	const auto& it = m_base_popdefs.find( id );
	if ( it != m_base_popdefs.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

base::Base* BaseManager::GetBase( const size_t id ) const {
	const auto& it = m_bases.find( id );
	if ( it != m_bases.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

void BaseManager::DefinePop( base::PopDef* pop_def ) {
	Log( "Defining base pop ('" + pop_def->m_id + "')" );

	ASSERT( m_base_popdefs.find( pop_def->m_id ) == m_base_popdefs.end(), "Base pop def '" + pop_def->m_id + "' already exists" );

	m_base_popdefs.insert(
		{
			pop_def->m_id,
			pop_def
		}
	);

	auto fr = FrontendRequest( FrontendRequest::FR_BASE_POP_DEFINE );
	NEW( fr.data.base_pop_define.serialized_popdef, std::string, base::PopDef::Serialize( pop_def ).ToString() );
	m_game->AddFrontendRequest( fr );
}

void BaseManager::SpawnBase( base::Base* base ) {
	if ( !m_game->IsRunning() ) {
		m_unprocessed_bases.push_back( base );
		return;
	}

	auto* tile = base->GetTile();

	// validate and fix name if needed (or assign if empty)
	std::vector< std::string > names_to_try = {};
	if ( base->m_name.empty() ) {
		const auto& names = base->m_owner->GetPlayer()->GetFaction()->m_base_names;
		names_to_try = tile->is_water_tile
			? names.water
			: names.land;
	}
	else if ( m_registered_base_names.find( base->m_name ) != m_registered_base_names.end() ) {
		names_to_try = { base->m_name };
	}
	if ( !names_to_try.empty() ) {
		size_t cycle = 0;
		bool found = false;
		while ( !found ) {
			cycle++;
			for ( const auto& name_to_try : names_to_try ) {
				base->m_name = cycle == 1
					? name_to_try
					: name_to_try + " " + std::to_string( cycle );
				if ( m_registered_base_names.find( base->m_name ) == m_registered_base_names.end() ) {
					found = true;
					break;
				}
			}
		}
	}
	m_registered_base_names.insert( base->m_name );

	Log( "Spawning base #" + std::to_string( base->m_id ) + " ( " + base->m_name + " ) at " + base->GetTile()->ToString() );

	ASSERT( m_bases.find( base->m_id ) == m_bases.end(), "duplicate base id" );
	m_bases.insert_or_assign( base->m_id, base );

	QueueBaseUpdate( base, BUO_SPAWN );

	auto* state = m_game->GetState();
	if ( state->IsMaster() ) {
		state->m_bindings->Trigger( this, "base_spawn",{
			{
				"base",
				base->Wrap()
			},
		});
	}

	RefreshBase( base );
}

const std::map< size_t, Base* >& BaseManager::GetBases() const {
	return m_bases;
}

void BaseManager::ProcessUnprocessed() {
	for ( auto& it : m_unprocessed_bases ) {
		SpawnBase( it );
	}
	m_unprocessed_bases.clear();
}

void BaseManager::PushUpdates() {
	if ( m_game->IsRunning() && !m_base_updates.empty() ) {
		for ( const auto& it : m_base_updates ) {
			const auto base_id = it.first;
			const auto& bu = it.second;
			const auto& base = bu.base;
			if ( bu.ops & BUO_SPAWN ) {
				auto fr = FrontendRequest( FrontendRequest::FR_BASE_SPAWN );
				fr.data.base_spawn.base_id = base->m_id;
				fr.data.base_spawn.slot_index = base->m_owner->GetIndex();
				const auto* tile = base->GetTile();
				fr.data.base_spawn.tile_coords = {
					tile->coord.x,
					tile->coord.y
				};
				const auto c = base->GetRenderCoords();
				fr.data.base_spawn.render_coords = {
					c.x,
					c.y,
					c.z
				};
				NEW( fr.data.base_spawn.name, std::string, base->m_name );
				m_game->AddFrontendRequest( fr );
			}
			if ( bu.ops & BUO_REFRESH ) {
				auto fr = FrontendRequest( FrontendRequest::FR_BASE_UPDATE );
				fr.data.base_update.base_id = base->m_id;
				fr.data.base_update.slot_index = base->m_owner->GetIndex();
				NEW( fr.data.base_update.name, std::string, base->m_name );
				NEW( fr.data.base_update.faction_id, std::string, base->m_faction->m_id );
				NEW( fr.data.base_update.pops, FrontendRequest::base_pops_t, {} );
				for ( const auto& pop : base->m_pops ) {
					fr.data.base_update.pops->push_back(
						{
							pop.m_def->m_id,
							pop.m_variant
						}
					);
				}
				m_game->AddFrontendRequest( fr );
			}
			if ( bu.ops & BUO_DESPAWN ) {
				THROW( "TODO: BASE DESPAWN" );
			}
		}
		m_base_updates.clear();
	}
}

WRAPIMPL_BEGIN( BaseManager, CLASS_BM )
	WRAPIMPL_PROPS
		{
			"spawn_base",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS_MIN_MAX( 3, 4 );
				N_GETVALUE_UNWRAP( owner, 0, slot::Slot );
				N_GETVALUE_UNWRAP( tile, 1, map::tile::Tile );

				N_GETVALUE( info, 2, Object );
				N_GETPROP_OPT( std::string, name, info, "name", String, "" );

				if ( arguments.size() > 3 ) {
					N_PERSIST_CALLABLE( on_spawn, 3 );
				}

				return m_game->AddEvent( new event::SpawnBase(
					m_game->GetSlotNum(),
					owner->GetIndex(),
					tile->coord.x,
					tile->coord.y,
					name
				) );
			})
		},
	};
WRAPIMPL_END_PTR( BaseManager )

UNWRAPIMPL_PTR( BaseManager )

void BaseManager::Serialize( types::Buffer& buf ) const {

	Log( "Serializing " + std::to_string( m_base_popdefs.size() ) + " base pop defs" );
	buf.WriteInt( m_base_popdefs.size() );
	for ( const auto& it : m_base_popdefs ) {
		buf.WriteString( it.first );
		buf.WriteString( base::PopDef::Serialize( it.second ).ToString() );
	}

	Log( "Serializing " + std::to_string( m_bases.size() ) + " bases" );
	buf.WriteInt( m_bases.size() );
	for ( const auto& it : m_bases ) {
		buf.WriteInt( it.first );
		buf.WriteString( base::Base::Serialize( it.second ).ToString() );
	}
	buf.WriteInt( base::Base::GetNextId() );

	Log( "Saved next base id: " + std::to_string( base::Base::GetNextId() ) );
}

void BaseManager::Unserialize( types::Buffer& buf ) {
	ASSERT( m_base_popdefs.empty(), "base pop defs not empty" );
	ASSERT( m_bases.empty(), "bases not empty" );
	ASSERT( m_unprocessed_bases.empty(), "unprocessed bases not empty" );

	size_t sz = buf.ReadInt();
	m_base_popdefs.reserve( sz );
	Log( "Unserializing " + std::to_string( sz ) + " base pop defs" );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefinePop( base::PopDef::Unserialize( b ) );
	}

	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " bases" );
	if ( !m_game->IsRunning() ) {
		m_unprocessed_bases.reserve( sz );
	}
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto base_id = buf.ReadInt();
		auto b = types::Buffer( buf.ReadString() );
		SpawnBase( base::Base::Unserialize( b, m_game ) );
	}

	base::Base::SetNextId( buf.ReadInt() );
	Log( "Restored next base id: " + std::to_string( base::Base::GetNextId() ) );
}

void BaseManager::RefreshBase( const base::Base* base ) {
	QueueBaseUpdate( base, BUO_REFRESH );
}

void BaseManager::QueueBaseUpdate( const Base* base, const base_update_op_t op ) {
	auto it = m_base_updates.find( base->m_id );
	if ( it == m_base_updates.end() ) {
		it = m_base_updates.insert(
			{
				base->m_id,
				{
					{},
					base,
				}
			}
		).first;
	}
	auto& update = it->second;
	if ( op == BUO_DESPAWN ) {
		if ( op & BUO_SPAWN ) {
			// if base is despawned immediately after spawning - frontend doesn't need to know
			m_base_updates.erase( it );
			return;
		}
		update.ops = BUO_NONE; // clear other actions if base was despawned
	}
	// add to operations list
	update.ops = (base_update_op_t)( (uint8_t)update.ops | (uint8_t)op );
}

}
}
}
