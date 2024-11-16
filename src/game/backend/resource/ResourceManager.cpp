#include "ResourceManager.h"

#include "Resource.h"

#include "gse/callable/Native.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/Bindings.h"
#include "game/backend/event/DefineResource.h"

namespace game {
namespace backend {
namespace resource {

ResourceManager::ResourceManager( Game* game )
	: m_game( game ) {
	//
}

ResourceManager::~ResourceManager() {
	Clear();
}

void ResourceManager::Clear() {
	for ( auto& it : m_resources ) {
		delete it.second;
	}
	m_resources.clear();
	m_resource_idx.clear();
	m_resource_idx_map.clear();
}

void ResourceManager::DefineResource( resource::Resource* resource ) {
	Log( "Defining resource ('" + resource->m_id + "')" );

	ASSERT( m_resources.find( resource->m_id ) == m_resources.end(), "Resource '" + resource->m_id + "' already exists" );

	m_resources.insert(
		{
			resource->m_id,
			resource
		}
	);
	m_resource_idx_map.insert(
		{
			resource->m_id,
			m_resource_idx.size()
		}
	);
	m_resource_idx.push_back( resource->m_id );
}

const ResourceManager::yields_t ResourceManager::GetYields( map::tile::Tile* tile, slot::Slot* slot ) {
	const auto result = m_game->GetState()->m_bindings->Trigger( this, "get_yields", {
			{
				"tile",
				tile->Wrap()
			},
			{
				"player",
				slot->Wrap()
			},
		}
	);
	if ( result.Get()->type != gse::type::Type::T_OBJECT ) {
		THROW( "unexpected return type: expected Object, got " + result.GetTypeString() );
	}
	const auto& values = ( (gse::type::Object*)result.Get() )->value;
	for ( const auto& v : values ) {
		if ( m_resources.find( v.first ) == m_resources.end() ) {
			THROW( "unknown resource type: " + v.first );
		}
	}
	yields_t yields = {};
	yields.reserve( m_resource_idx.size() );
	for ( const auto& idx : m_resource_idx ) {
		const auto& v = values.find( idx );
		if ( v == values.end() ) {
			THROW( "missing yields for resource: " + idx );
		}
		if ( v->second.Get()->type != gse::type::Type::T_INT ) {
			THROW( "invalid resource value, expected Int, got " + v->second.GetTypeString() + ": " + v->second.ToString() );
		}
		yields.push_back(
			{
				idx,
				( (gse::type::Int*)v->second.Get() )->value
			}
		);
	}
	return yields;
}

WRAPIMPL_BEGIN( ResourceManager, CLASS_RM )
	WRAPIMPL_PROPS
		{
			"define",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( def, 1, Object );
				N_GETPROP( name, def, "name", String );
				N_GETPROP( render, def, "render", Object );
				N_GETPROP( type, render, "type", String );

				if ( type == "sprite_map" ) {
					N_GETPROP( file, render, "file", String );

					N_GETPROP( yields, render, "yields", Object );
					N_GETPROP( grid_x, yields, "grid_x", Int );
					N_GETPROP( grid_y, yields, "grid_y", Int );
					N_GETPROP( grid_margin, yields, "grid_margin", Int );
					N_GETPROP( cell_width, yields, "cell_width", Int );
					N_GETPROP( cell_height, yields, "cell_height", Int );
					N_GETPROP( cells_count, yields, "cells_count", Int );

#define X( _n ) \
					N_GETPROP( _n, render, "plus", Object ); \
					N_GETPROP( _n ## _x, _n, "x", Int ); \
					N_GETPROP( _n ## _y, _n, "y", Int ); \
					N_GETPROP( _n ## _width, _n, "width", Int ); \
					N_GETPROP( _n ## _height, _n, "height", Int );
					X( plus )
					X( minus )

					auto* resource = new resource::Resource(
						id,
						name,
						{
							file,
							{
								(uint16_t)grid_x, (uint16_t)grid_y, (uint8_t)grid_margin,
								(uint16_t)cell_width, (uint16_t)cell_height, (uint8_t)cells_count,
							},
							{
								(uint16_t)plus_x, (uint16_t)plus_y,
								(uint16_t)plus_width, (uint16_t)plus_height,
							},
							{
								(uint16_t)minus_x, (uint16_t)minus_y,
								(uint16_t)minus_width, (uint16_t)minus_height,
							},
						}
					);
					return m_game->AddEvent( new event::DefineResource( m_game->GetSlotNum(), resource ) );
				}
				else {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported resource type: " + type );
				}

				return VALUE( gse::type::Undefined );
			} )
		},
	};
WRAPIMPL_END_PTR( ResourceManager )

UNWRAPIMPL_PTR( ResourceManager )

void ResourceManager::Serialize( types::Buffer& buf ) const {
	Log( "Serializing " + std::to_string( m_resources.size() ) + " resources" );
	buf.WriteInt( m_resources.size() );
	for ( const auto& it : m_resource_idx ) {
		ASSERT( m_resources.find( it ) != m_resources.end(), "invalid resource idx" );
		const auto& res = m_resources.at( it );
		buf.WriteString( res->m_id );
		buf.WriteString( resource::Resource::Serialize( res ).ToString() );
	}
}

void ResourceManager::Unserialize( types::Buffer& buf ) {
	Clear();
	ASSERT( m_resources.empty(), "resources not empty" );
	size_t sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " resources" );
	m_resources.reserve( sz );
	m_resource_idx.reserve( sz );
	m_resource_idx_map.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineResource( resource::Resource::Unserialize( b ) );
	}
}


}
}
}
