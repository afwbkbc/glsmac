#include "Pop.h"

#include "game/backend/Game.h"
#include "game/backend/base/Base.h"
#include "game/backend/base/PopDef.h"
#include "game/backend/base/BaseManager.h"
#include "game/backend/map/Map.h"
#include "game/backend/map/tile/Tile.h"
#include "gse/value/Int.h"
#include "gse/value/Bool.h"
#include "gse/callable/Native.h"

namespace game {
namespace backend {
namespace base {

Pop::Pop( Base* const base, const size_t id, const PopDef* def, const uint8_t variant, map::tile::Tile* const worked_tile )
	: m_base( base )
	, m_id( id )
	, m_def( def )
	, m_variant( variant )
	, m_worked_tile( worked_tile ) {
	//
}

void Pop::Serialize( types::Buffer& buf ) const {
	ASSERT( m_base, "pop base is null" );
	ASSERT( m_def, "pop def is null" );

	buf.WriteInt( m_id );
	buf.WriteString( m_def->m_id );
	buf.WriteInt( m_variant );
	buf.WriteBool( m_worked_tile != nullptr );
	if ( m_worked_tile ) {
		buf.WriteInt( m_worked_tile->coord.x );
		buf.WriteInt( m_worked_tile->coord.y );
	}
}

void Pop::Deserialize( types::Buffer& buf, Game* game ) {
	ASSERT( !m_base, "pop base is not null" );
	ASSERT( !m_def, "pop def is not null" );

	auto* bm = game->GetBM();
	ASSERT( bm, "bm is null" );

	const auto id = buf.ReadInt< size_t >( "base population id" );
	if ( id == 0 ) {
		THROW( "serialized base population id is zero" );
	}
	m_id = id;
	const auto def_id = buf.ReadString();
	m_def = bm->GetPopDef( def_id );
	if ( !m_def ) {
		THROW( "base pop definition not found: " + def_id );
	}
	m_variant = buf.ReadInt< uint8_t >( "base population variant" );
	if ( buf.ReadBool() ) {
		const auto tile_x = buf.ReadInt< size_t >( "base population worked tile x" );
		const auto tile_y = buf.ReadInt< size_t >( "base population worked tile y" );
		auto* const map = game->GetMap();
		if (
			tile_x >= map->GetWidth() ||
			tile_y >= map->GetHeight() ||
			tile_x % 2 != tile_y % 2
		) {
			THROW( "invalid serialized base population worked tile" );
		}
		m_worked_tile = map->GetTile(
			tile_x,
			tile_y
		);
	}
}

void Pop::SetBase( Base* const base ) {
	ASSERT( !m_base, "pop base already set" );
	m_base = base;
}

bool Pop::HasWorkedTileLink() const {
	return const_cast< Pop* >( this )->CustomHas( "worked_tile" );
}

map::tile::Tile* Pop::GetWorkedTileLink() const {
	auto* const value = const_cast< Pop* >( this )->CustomGet( "worked_tile" );
	if ( !value ) {
		return nullptr;
	}
	auto* const dereferenced = value->type == gse::VT_OBJECT
		? value
		: value->Deref();
	if (
		dereferenced->type != gse::VT_OBJECT ||
		( (gse::value::Object*)dereferenced )->object_class != map::tile::Tile::WRAP_CLASS
	) {
		return nullptr;
	}
	return (map::tile::Tile*)( (gse::value::Object*)dereferenced )->wrapobj;
}

void Pop::SetWorkedTile( GSE_CALLABLE, map::tile::Tile* const tile ) {
	if ( !tile ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "population worked tile is null" );
	}
	if ( m_worked_tile && m_worked_tile != tile ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population already works another tile" );
	}
	auto* const value = CustomGet( "worked_tile" );
	if ( value ) {
		auto* const dereferenced = value->Deref();
		if (
			dereferenced->type != gse::VT_OBJECT ||
			( (gse::value::Object*)dereferenced )->object_class != map::tile::Tile::WRAP_CLASS ||
			( (gse::value::Object*)dereferenced )->wrapobj != tile
		) {
			GSE_ERROR( gse::EC.GAME_ERROR, "population has a conflicting worked tile link" );
		}
	}
	m_worked_tile = tile;
	if ( !value ) {
		CustomSet( "worked_tile", tile->Wrap( GSE_CALL ) );
	}
}

void Pop::UnsetWorkedTile( GSE_CALLABLE, const map::tile::Tile* const tile ) {
	if ( !m_worked_tile || m_worked_tile != tile ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population worked tile does not match" );
	}
	auto* const value = CustomGet( "worked_tile" );
	if ( !value ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population worked tile link is missing" );
	}
	auto* const dereferenced = value->Deref();
	if (
		dereferenced->type != gse::VT_OBJECT ||
		( (gse::value::Object*)dereferenced )->object_class != map::tile::Tile::WRAP_CLASS ||
		( (gse::value::Object*)dereferenced )->wrapobj != tile
	) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population worked tile link does not match" );
	}
	m_worked_tile = nullptr;
	CustomUnset( "worked_tile" );
}

WRAPIMPL_SERIALIZE( Pop )
	if ( !obj->m_base ) {
		THROW( "pop base is null" );
	}
	buf->WriteInt( obj->m_base->m_id );
	buf->WriteInt( obj->m_id );
}

WRAPIMPL_DESERIALIZE( Pop )
	const auto base_id = buf->ReadInt< size_t >( "base population reference base id" );
	const auto pop_id = buf->ReadInt< size_t >( "base population reference id" );
	auto* const base = game->GetBM()->GetBase( base_id );
	if ( !base ) {
		THROW( "base population reference has unknown base: " + std::to_string( base_id ) );
	}
	const auto it = base->m_pops.find( pop_id );
	if ( it == base->m_pops.end() ) {
		THROW( "base population reference has unknown population: " + std::to_string( pop_id ) );
	}
	return it->second.Wrap( GSE_CALL );
}

WRAPIMPL_BEGIN( Pop )
	WRAPIMPL_PROPS
	WRAPIMPL_CUSTOM_SETTERS
		{
			"id",
			VALUE( gse::value::Int,, m_id ),
		},
		{
			"get_type",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				return VALUE( gse::value::String, , m_def->m_id );
			} )
		},
		{
			"variant",
			VALUE( gse::value::Int,, m_variant )
		},
		{
			"get_base",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				ASSERT( m_base, "pop has no base" );
				return m_base->Wrap( GSE_CALL );
			} )
		},
		{
			"set_type",
			NATIVE_CALL( this ) {

				m_base->GetGame()->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( id, 0, String );

				m_base->ChangePopType( GSE_CALL, m_id, id );

				return VALUE( gse::value::Undefined );
			} )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Pop )

}
}
}
