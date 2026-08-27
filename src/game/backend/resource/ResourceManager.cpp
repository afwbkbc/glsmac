#include "ResourceManager.h"

#include <algorithm>
#include <memory>
#include <unordered_set>

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
	if ( m_noresource_defined ) {
		m_noresource_defined = false;
		m_noresource_info = {};
	}
}

void ResourceManager::DefineResource( resource::Resource* resource ) {
	Log( "Defining resource ('" + resource->m_id + "')" );

	if ( m_resources.find( resource->m_id ) != m_resources.end() ) {
		THROW( "resource already exists: " + resource->m_id );
	}

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

	const auto resource_it = m_resources.find( id );
	if ( resource_it == m_resources.end() ) {
		THROW( "resource does not exist: " + id );
	}

	auto* const resource = resource_it->second;
	m_resources.erase( resource_it );
	delete resource;
	auto it = std::find( m_resources_order.begin(), m_resources_order.end(), id );
	if ( it != m_resources_order.end() ) {
		m_resources_order.erase( it );
	}

	auto fr = FrontendRequest( FrontendRequest::FR_RESOURCE_UNDEFINE );
	NEW( fr.data.resource_undefine.id, std::string, id );
	m_game->AddFrontendRequest( fr );
}

void ResourceManager::DefineNoResource( const render_info_t& info ) {
	Log( "Defining no-resource" );

	if ( m_noresource_defined ) {
		THROW( "no-resource already defined" );
	}
	Resource::ValidateRenderInfo( info, true );
	m_noresource_defined = true;
	m_noresource_info = info;

	types::Buffer buf = {};
	buf.WriteString( m_noresource_info.file );
	ASSERT( m_noresource_info.coords.size() == 1, "no-resource coords size mismatch" );
	const auto& c = m_noresource_info.coords.at( 0 );
	buf.WriteInt( c.first.x );
	buf.WriteInt( c.first.y );
	buf.WriteInt( c.second.x );
	buf.WriteInt( c.second.y );

	auto fr = FrontendRequest( FrontendRequest::FR_NORESOURCE_DEFINE );
	NEW( fr.data.noresource_define.serialized_noresource, std::string, buf.ToString() );
	m_game->AddFrontendRequest( fr );
}

void ResourceManager::UndefineNoResource() {
	Log( "Undefining no-resource" );

	if ( !m_noresource_defined ) {
		THROW( "no-resource not defined" );
	}

	m_noresource_defined = false;
	m_noresource_info = {};

	auto fr = FrontendRequest( FrontendRequest::FR_NORESOURCE_UNDEFINE );
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
					render_coords_t render_coords = {};

					for ( const auto& v : coords ) {
						AddRenderCoords( GSE_CALL, render_coords, v );
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
		},
		{
			"define_no_resource",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( def, 0, Object );
				N_GETPROP( render, def, "render", Object );
				N_GETPROP( type, render, "type", String );

				if ( m_noresource_defined ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "No-resource already defined" );
				}

				if ( type == "sprites" ) {
					N_GETPROP( file, render, "file", String );

					N_GETPROP_VALUE( coords, render, "coords", Array );
					render_coords_t render_coords = {};
					AddRenderCoords( GSE_CALL, render_coords, coords );

					DefineNoResource({
						file,
						render_coords,
					});

					return VALUE( gse::value::Undefined );
				}
				else {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported resource type: " + type );
				}

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"undefine_no_resource",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 0 );

				if ( !m_noresource_defined ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "No-resource is not defined" );
				}

				UndefineNoResource();

				return VALUE( gse::value::Undefined );
			} )
		},
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
	buf.WriteBool( m_noresource_defined );
	if ( m_noresource_defined ) {
		Log( "Serializing no-resource" );
		buf.WriteString( m_noresource_info.file );
		ASSERT( m_noresource_info.coords.size() == 1, "no-resource coords size mismatch" );
		const auto& c = m_noresource_info.coords.at( 0 );
		buf.WriteInt( c.first.x );
		buf.WriteInt( c.first.y );
		buf.WriteInt( c.second.x );
		buf.WriteInt( c.second.y );
	}
}

void ResourceManager::Deserialize( types::Buffer& buf ) {
	const size_t sz = buf.ReadCollectionSize( "resource" );
	Log( "Deserializing " + std::to_string( sz ) + " resources" );
	std::vector< std::unique_ptr< resource::Resource > > resources = {};
	resources.reserve( sz );
	std::unordered_set< std::string > resource_ids = {};
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto id = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		auto resource = std::unique_ptr< resource::Resource >( resource::Resource::Deserialize( b ) );
		if ( id != resource->m_id ) {
			THROW( "serialized resource id mismatch" );
		}
		if ( !resource_ids.insert( id ).second ) {
			THROW( "duplicate serialized resource: " + id );
		}
		resources.push_back( std::move( resource ) );
	}
	const bool has_noresource = buf.ReadBool();
	render_info_t noresource_info = {};
	if ( has_noresource ) {
		Log( "Deserializing no-resource" );
		noresource_info = {
			buf.ReadString(),
			{
				{
					{
						buf.ReadInt(),
						buf.ReadInt(),
					},
					{
						buf.ReadInt(),
						buf.ReadInt(),
					},
				}
			},
		};
		Resource::ValidateRenderInfo( noresource_info, true );
	};
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized resource manager" );
	}

	const auto old_resource_ids = m_resources_order;
	for ( const auto& id : old_resource_ids ) {
		UndefineResource( id );
	}
	if ( m_noresource_defined ) {
		UndefineNoResource();
	}
	m_resources.reserve( resources.size() );
	for ( auto& resource : resources ) {
		DefineResource( resource.release() );
	}
	if ( has_noresource ) {
		DefineNoResource( noresource_info );
	}
}

void ResourceManager::AddRenderCoords( GSE_CALLABLE, render_coords_t& render_coords, gse::Value* const value ) {
	if ( value->type != gse::VT_ARRAY ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Resource coords must be array." );
	}
	const auto& c = ((gse::value::Array*)value)->value;
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

}
}
}
