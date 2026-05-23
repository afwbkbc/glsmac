#include "ResourceManager.h"

#include <algorithm>

#include "Resource.h"

#include "gse/callable/Native.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/Bindings.h"
#include "gse/value/Array.h"

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
	m_resources_order.clear();
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
	m_resources_order.push_back( resource->m_id );

	auto fr = FrontendRequest( FrontendRequest::FR_RESOURCE_DEFINE );
	NEW( fr.data.resource_define.serialized_resourcedef, std::string, Resource::Serialize( resource ).ToString() );
	m_game->AddFrontendRequest( fr );
}

void ResourceManager::UndefineResource( const std::string& id ) {
	Log( "Undefining resource ('" + id + "')" );

	ASSERT( m_resources.find( id ) != m_resources.end(), "resource does not exist" );

	m_resources.erase( id );
	auto it = std::find( m_resources_order.begin(), m_resources_order.end(), id );
	if ( it != m_resources_order.end() ) {
		m_resources_order.erase( it );
	}

	auto fr = FrontendRequest( FrontendRequest::FR_RESOURCE_UNDEFINE );
	NEW( fr.data.resource_undefine.id, std::string, id );
	m_game->AddFrontendRequest( fr );
}

const std::vector< std::string >& ResourceManager::GetDefinedResourcesOrder() const {
	return m_resources_order;
}

const ResourceManager::resource_definitions_t& ResourceManager::GetDefinedResources() const {
	return m_resources;
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

				if ( type == "sprites" ) {
					N_GETPROP( file, render, "file", String );

					N_GETPROP( coords, render, "coords", Array );
					Resource::render_coords_t render_coords = {};

					for ( const auto& v : coords ) {
						if ( v->type != gse::VT_ARRAY ) {
							GSE_ERROR( gse::EC.GAME_ERROR, "Resource coords must be array." );
						}
						const auto& c = ((gse::value::Array*)v)->value;
						if ( c.size() != 4 ) {
							GSE_ERROR( gse::EC.GAME_ERROR, "Resource coords arrays must contain exactly 4 elements (x1, y1, x2, y2)." );
						}
						for ( const auto& cv : c ) {
							if ( cv->type != gse::VT_INT ) {
								GSE_ERROR( gse::EC.GAME_ERROR, "Resource coords array elements must be of type Int." );
							}
						}
						render_coords.push_back({
							{
								( (gse::value::Int*)c.at( 0 ) )->value,
								( (gse::value::Int*)c.at( 1 ) )->value,
							},
							{
								( (gse::value::Int*)c.at( 2 ) )->value,
								( (gse::value::Int*)c.at( 3 ) )->value,
							}
						});
					}

					auto* resource = new resource::Resource(
						id,
						name,
						{
							file,
							render_coords,
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

void ResourceManager::Deserialize( types::Buffer& buf ) {
	Clear();
	ASSERT( m_resources.empty(), "resources not empty" );
	size_t sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " resources" );
	m_resources.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineResource( resource::Resource::Deserialize( b ) );
	}
}

}
}
}
