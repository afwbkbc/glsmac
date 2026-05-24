#include "Base.h"

#include "gse/context/Context.h"
#include "gse/value/Object.h"
#include "gse/value/Int.h"
#include "gse/value/String.h"
#include "gse/value/Undefined.h"
#include "gse/value/Array.h"
#include "gse/value/Bool.h"
#include "gse/callable/Native.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/faction/Faction.h"
#include "game/backend/base/BaseManager.h"
#include "Pop.h"
#include "PopDef.h"
#include "game/backend/Random.h"
#include "game/backend/resource/ResourceManager.h"

namespace game {
namespace backend {
namespace base {

static size_t next_id = 1;
const size_t Base::GetNextId() {
	return next_id;
}
const void Base::SetNextId( const size_t id ) {
	next_id = id;
}

Base::Base(
	Game* game,
	const size_t id,
	slot::Slot* owner,
	faction::Faction* faction,
	map::tile::Tile* tile,
	const std::string& name,
	const pops_t& pops,
	const size_t next_pop_id
)
	: MapObject( game->GetMap(), tile )
	, m_game( game )
	, m_id( id )
	, m_owner( owner )
	, m_faction( faction )
	, m_name( name )
	, m_pops( pops )
	, m_next_pop_id( next_pop_id ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
	ASSERT( !tile->base, "tile already has base" );
	tile->base = this;
	m_tile = tile;
	for ( auto& it : m_pops ) {
		it.second.SetBase( this );
	}
}

Pop* const Base::AddPop( const Pop& pop ) {
	ASSERT( m_pops.find( pop.m_id ) == m_pops.end(), "pop already exists" );
	m_pops.insert_or_assign( pop.m_id, pop );
	m_game->GetBM()->RefreshBase( this );
	return &m_pops.at( pop.m_id );
}

void Base::RemovePop( const size_t pop_id ) {
	ASSERT( m_pops.find( pop_id ) != m_pops.end(), "pop id not found" );
	m_pops.erase( pop_id );
}

const types::Buffer Base::Serialize( const Base* base ) {
	types::Buffer buf;
	buf.WriteInt( base->m_id );
	buf.WriteInt( base->m_owner->GetIndex() );
	buf.WriteString( base->m_faction->m_id );
	buf.WriteInt( base->m_tile->coord.x );
	buf.WriteInt( base->m_tile->coord.y );
	buf.WriteString( base->m_name );
	buf.WriteInt( base->m_pops.size() );
	for ( const auto& it : base->m_pops ) {
		buf.WriteInt( it.first );
		it.second.Serialize( buf );
	}
	buf.WriteInt( base->m_next_pop_id );
	return buf;
}

Base* Base::Deserialize( types::Buffer& buf, Game* game ) {
	ASSERT( game, "game is null" );
	const auto id = buf.ReadInt();
	auto* slot = &game->GetState()->m_slots->GetSlot( buf.ReadInt() );
	const auto faction_id = buf.ReadString();
	auto* faction = game->GetFaction( faction_id );
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	auto* tile = game->GetMap()->GetTile( pos_x, pos_y );
	const auto name = buf.ReadString();
	pops_t pops = {};
	const auto pops_count = buf.ReadInt();
	for ( size_t i = 0 ; i < pops_count ; i++ ) {
		const auto pop_id = buf.ReadInt();
		Pop pop = {};
		pop.Deserialize( buf, game );
		pops.insert_or_assign( pop_id, pop );
	}
	const auto next_pop_id = buf.ReadInt();
	return new Base( game, id, slot, faction, tile, name, pops, next_pop_id );
}

WRAPIMPL_SERIALIZE( Base )
	buf->WriteInt( obj->m_id );
}

WRAPIMPL_DESERIALIZE( Base )
	const auto id = buf->ReadInt();
	const auto& base = game->GetBM()->GetBase( id );
	ASSERT( base, "base id not found: " + std::to_string( id ) );
	return base->Wrap( GSE_CALL );
}

WRAPIMPL_DYNAMIC_GETTERS( Base )
	WRAPIMPL_GET_CUSTOM( "id", Int, m_id )
	WRAPIMPL_GET_CUSTOM( "name", String, m_name )
	WRAPIMPL_LINK( "get_owner", m_owner )
	WRAPIMPL_LINK( "get_tile", m_tile )
	WRAPIMPL_CUSTOM_SETTERS
	{
		"create_pop",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE( data, 0, Object );
			N_GETPROP( poptype, data, "type", String );
			N_GETPROP_UNWRAP_OPT( worked_tile, data, "worked_tile", map::tile::Tile );
			auto* def = m_game->GetBM()->GetPopDef( poptype );
			if ( !def ) {
				GSE_ERROR( gse::EC.INVALID_DEFINITION, "Unknown pop type: " + poptype );
			}
			const auto max_variants = (m_faction->m_flags & faction::Faction::FF_PROGENITOR)
				? 1 // aliens have 1 gender
				: 2; // humans have 2
			ASSERT( max_variants > 0, "no variants found for pop type: " + poptype );

			auto* const pop = AddPop( Pop( this, m_next_pop_id++, def, m_game->GetRandom()->GetUInt(0, max_variants - 1), worked_tile ) );

			return pop->Wrap( GSE_CALL );
		} )
	},
	{
		"remove_pop",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE( pop_id, 0, Int );

			if ( pop_id >= m_pops.size() ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "Base does not have pop id " + std::to_string( pop_id ) );
			}

			RemovePop( pop_id );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"add_worked_tile",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );

			if ( m_worked_tiles.find( tile ) != m_worked_tiles.end() ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "This tile is already worked" );
			}
			m_worked_tiles.insert( tile );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"remove_worked_tile",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );

			if ( m_worked_tiles.find( tile ) == m_worked_tiles.end() ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "This tile is not worked" );
			}
			m_worked_tiles.erase( tile );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"is_tile_worked",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );
			return VALUE( gse::value::Bool,, m_worked_tiles.find( tile ) != m_worked_tiles.end() );
		} )
	},
	{
		"get_pops",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );

			gse::value::array_elements_t elements = {};

			for ( auto& it : m_pops ) {
				elements.push_back( it.second.Wrap( GSE_CALL ) );
			}

			return VALUE( gse::value::Array,, elements );
		} ),
	},
	{
		"get_size",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return VALUE( gse::value::Int,, m_pops.size() );
		} ),
	},
	{
		"get_workable_tiles",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetWorkableTiles( GSE_CALL );
		} ),
	},
	{
		"get_worked_tiles",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetWorkedTiles( GSE_CALL );
		} ),
	},
	{
		"get_unworked_tiles",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetUnworkedTiles( GSE_CALL );
		} ),
	},
	{
		"get_intake",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetIntake( GSE_CALL );
		} ),
	},
	{
		"get_consumption",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetConsumption( GSE_CALL );
		} ),
	},
WRAPIMPL_DYNAMIC_SETTERS( Base )
WRAPIMPL_DYNAMIC_ON_SET( Base )
WRAPIMPL_DYNAMIC_END()

UNWRAPIMPL_PTR( Base )

gse::value::Array* const Base::GetWorkableTiles( GSE_CALLABLE ) {
	auto* const result = m_game->GetBM()->Trigger( GSE_CALL, "get_base_workable_tiles", ARGS_F( this ) {
		{
			"base",
			Wrap( GSE_CALL )
		},
	}; } );
	ASSERT( result, "GetWorkableTiles result is null" );
	if ( result->type != gse::VT_ARRAY ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "get_base_workable_tiles must return array, got: " + result->ToString() );
	}
	for ( const auto& v : ((gse::value::Array*)result)->value ) {
		if ( v->type != gse::VT_OBJECT || ((gse::value::Object*)v)->object_class != "Tile" ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "get_base_workable_tiles elements must be objects of type Tile, got: " + v->ToString() );
		}
	}
	return (gse::value::Array*)result;
}

gse::value::Array* const Base::GetWorkedTiles( GSE_CALLABLE ) {
	gse::value::array_elements_t result = {};
	for ( const auto& tile : m_worked_tiles ) {
		result.push_back( tile->Wrap( GSE_CALL ) );
	}
	return VALUE( gse::value::Array,, result );
}

gse::value::Array* const Base::GetUnworkedTiles( GSE_CALLABLE ) {
	gse::value::array_elements_t result = {};
	const auto* workable_tiles_v = GetWorkableTiles( GSE_CALL );
	for ( const auto& v : workable_tiles_v->value ) {
		ASSERT( v->type == gse::VT_OBJECT && ((gse::value::Object*)v)->object_class == "Tile", "invalid tile object" );
		auto* tile = (map::tile::Tile*)((gse::value::Object*)v)->wrapobj;
		if ( m_worked_tiles.find( tile ) == m_worked_tiles.end() ) {
			result.push_back( v );
		}
	}
	return VALUE( gse::value::Array,, result );
}

gse::value::Object* const Base::GetIntake( GSE_CALLABLE ) {
	return GetResourcesFromCallback( GSE_CALL, m_game->GetBM(), m_game->GetRM(), "get_base_intake", ARGS_F( this ) {
		{
			"base",
			Wrap( GSE_CALL )
		},
		{
			"player",
			m_owner->Wrap( GSE_CALL )
		},
	}; } );
}

gse::value::Object* const Base::GetConsumption( GSE_CALLABLE ) {
	return GetResourcesFromCallback( GSE_CALL, m_game->GetBM(), m_game->GetRM(), "get_base_consumption", ARGS_F( this ) {
		{
			"base",
			Wrap( GSE_CALL )
		},
		{
			"player",
			m_owner->Wrap( GSE_CALL )
		},
	}; } );
}

}
}
}
