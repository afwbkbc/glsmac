#include "ResourceManager.h"

#include "game/frontend/Game.h"
#include "game/frontend/sprite/InstancedSpriteManager.h"
#include "game/backend/resource/Resource.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "game/backend/map/Consts.h"

namespace game {
namespace frontend {
namespace resource {

ResourceManager::ResourceManager( game::frontend::Game* game )
	: m_game( game )
	, m_ism( game->GetISM() ) {}

ResourceManager::~ResourceManager() {
	for ( const auto& it : m_resources ) {
		for ( const auto& sprite : it.second.sprites ) {
			m_ism->RemoveInstancedSpriteByKey( sprite->key );
		}
	}
}

void ResourceManager::DefineResource( const backend::resource::Resource* const resource ) {
	const auto& ri = resource->m_render_info;
	auto* texture = g_engine->GetTextureLoader()->LoadCustomTexture( ri.file );
	const auto& coords = ri.coords;
	ASSERT( m_resources.find( resource->m_id ) == m_resources.end(), "resource sprites already exists" );
	auto& r = m_resources.insert( { resource->m_id, {} } ).first->second;
	for ( size_t i = 0 ; i < coords.size() ; i++ ) {
		const auto& c = coords.at( i );
		const auto* const sprite = m_ism->GetInstancedSprite(
			"Resource_" + resource->m_id + "_" + std::to_string( i + 1 ),
			texture,
			{
				(uint32_t)c.first.x,
				(uint32_t)c.first.y,
			},
			{
				(uint32_t)( c.second.x - c.first.x + 1 ),
				(uint32_t)( c.second.y - c.first.y + 1 ),
			},
			{
				(uint32_t)( c.second.x - c.first.x + 1 ) / 2,
				(uint32_t)( c.second.y - c.first.y + 1 ) / 2,
			},
			{
				backend::map::s_consts.tile.scale.x * 0.2f, // TODO: this is not used for some reason
				backend::map::s_consts.tile.scale.y * 0.3f
			},
			ZL_RESOURCES
		);
		r.sprites.push_back( sprite );
		if ( sprite->wh.x > r.max_wh.x ) {
			r.max_wh.x = sprite->wh.x;
		}
		if ( sprite->wh.y > r.max_wh.y ) {
			r.max_wh.y = sprite->wh.y;
		}
	}
}

void ResourceManager::UndefineResource( const std::string& id ) {
	ASSERT( m_resources.find( id ) != m_resources.end(), "resource sprite " + id + " not found" );
	const auto& r = m_resources.at( id );
	for ( const auto& sprite : r.sprites ) {
		m_ism->RemoveInstancedSpriteByKey( sprite->key );
	}
	m_resources.erase( id );
}

const sprite::InstancedSprite* const ResourceManager::GetResourceSprite( const std::string& id, const size_t value ) {
	ASSERT( m_resources.find( id ) != m_resources.end(), "resource sprite " + id + " not found" );
	const auto& r = m_resources.at( id );
	ASSERT( value > 0, "resource sprite " + id + " value is zero" );
	ASSERT( r.sprites.size() > value - 1, "resource sprite " + id + " overflow: " + std::to_string( value ) + " > " + std::to_string( r.sprites.size() ) );
	return r.sprites.at( value - 1 );
}

const types::Vec2< size_t >& ResourceManager::GetResourceMaxWH( const std::string& id ) {
	ASSERT( m_resources.find( id ) != m_resources.end(), "resource sprite " + id + " not found" );
	return m_resources.at( id ).max_wh;
}

}
}
}
