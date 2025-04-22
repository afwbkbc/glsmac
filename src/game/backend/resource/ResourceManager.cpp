#include "ResourceManager.h"

#include "Resource.h"

#include "gse/callable/Native.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/Bindings.h"

namespace game {
namespace backend {
namespace resource {

ResourceManager::ResourceManager( Game* game )
	: gse::GCWrappable( game->GetGCSpace() )
	, m_game( game ) {
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
}

void ResourceManager::DefineResource( resource::Resource* resource ) {
	Log( "Defining resource ('" + resource->m_id + "')" );

	ASSERT( m_resources.find( resource->m_id ) == m_resources.end(), "resource already exists" );

	m_resources.insert(
		{
			resource->m_id,
			resource
		}
	);
}

void ResourceManager::UndefineResource( const std::string& id ) {
	Log( "Undefining resource ('" + id + "')" );

	ASSERT( m_resources.find( id ) != m_resources.end(), "resource does not exist" );
	m_resources.erase( id );
}

const map::tile::yields_t ResourceManager::GetYields( GSE_CALLABLE, map::tile::Tile* tile, slot::Slot* slot ) {
	map::tile::yields_t yields = {};
	for ( const auto& it : m_resources ) {
		const auto result = m_game->GetState()->TriggerObject( this, "get_yield", ARGS_F( ARGS_GSE_CALLABLE, &tile, &slot, &it ) {
			{
				"tile",
				tile->Wrap( GSE_CALL, gc_space )
			},
			{
				"resource",
				VALUE( gse::value::String,, it.first )
			},
			{
				"player",
				slot->Wrap( GSE_CALL, gc_space )
			},
		}; } );
		if ( result->type != gse::Value::T_INT ) {
			GSE_ERROR( gse::EC.INVALID_HANDLER, "unexpected return type: expected Object, got " + result->GetTypeString() );
		}
		yields.insert(
			{
				it.first,
				( (gse::value::Int*)result )->value
			}
		);
	}
	return yields;
}

WRAPIMPL_BEGIN( ResourceManager )
	WRAPIMPL_PROPS
	WRAPIMPL_TRIGGERS
		{
			"define",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( def, 1, Object );
				N_GETPROP( name, def, "name", String );
				N_GETPROP( render, def, "render", Object );
				N_GETPROP( type, render, "type", String );

				if ( m_resources.find( id ) != m_resources.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Resource '" + id + "' already exists" );
				}

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
					DefineResource( resource );
					return VALUE( gse::value::Undefined );
				}
				else {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported resource type: " + type );
				}

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"undefine",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( id, 0, String );

				if ( m_resources.find( id ) == m_resources.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Resource \"" + id + "\" does not exist" );
				}

				UndefineResource( id );

				return VALUE( gse::value::Undefined );
			} )
		}
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( ResourceManager )

void ResourceManager::Serialize( types::Buffer& buf ) const {
	Log( "Serializing " + std::to_string( m_resources.size() ) + " resources" );
	buf.WriteInt( m_resources.size() );
	for ( const auto& it : m_resources ) {
		const auto& res = it.second;
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
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineResource( resource::Resource::Unserialize( b ) );
	}
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string ResourceManager::ToString() {
	return "game::ResourceManager()";
}
#endif

}
}
}
