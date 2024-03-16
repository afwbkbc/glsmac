#include "InstancedSpriteManager.h"

namespace task {
namespace game {

InstancedSpriteManager::InstancedSpriteManager( scene::Scene* scene )
	: m_scene( scene ) {

}

InstancedSpriteManager::~InstancedSpriteManager() {
	for ( auto& sprite : m_instanced_sprites ) {
		m_scene->RemoveActor( sprite.second.actor );
		DELETE( sprite.second.actor );
	}
	for ( auto& sprite : m_repainted_instanced_sprites ) {
		m_scene->RemoveActor( sprite.second.actor );
		DELETE( sprite.second.actor );
	}
	m_instanced_sprites.clear();

	for ( const auto& it : m_repainted_textures ) {
		delete it.second;
	}
}

InstancedSprite* InstancedSpriteManager::GetInstancedSprite(
	const std::string& name,
	types::Texture* texture,
	const ::game::map::Consts::pcx_texture_coordinates_t& src_xy,
	const ::game::map::Consts::pcx_texture_coordinates_t& src_wh,
	const ::game::map::Consts::pcx_texture_coordinates_t& src_cxy,
	const types::Vec2< float > dst_wh,
	const float z_index
) {

	const auto key = name + " " + src_xy.ToString() + " " + src_wh.ToString();

	auto it = m_instanced_sprites.find( key );
	if ( it == m_instanced_sprites.end() ) {

		Log( "Creating instanced sprite: " + key );

		NEWV(
			sprite,
			scene::actor::Sprite,
			name,
			{
				dst_wh.x,
				dst_wh.y
			},
			texture,
			{
				{
					(float)1.0f / texture->m_width * ( src_xy.x ),
					(float)1.0f / texture->m_height * ( src_xy.y )
				},
				{
					(float)1.0f / texture->m_width * ( src_xy.x + src_wh.x ),
					(float)1.0f / texture->m_height * ( src_xy.y + src_wh.y )
				}
			}
		);
		NEWV( instanced, scene::actor::Instanced, sprite );
		instanced->SetZIndex( z_index ); // needs to be higher than map terrain z position
		m_scene->AddActor( instanced );
		it = m_instanced_sprites.insert(
			{
				key,
				{
					key,
					name,
					src_xy,
					src_wh,
					src_cxy,
					instanced
				}
			}
		).first;
	}
	return &it->second;
}

InstancedSprite* InstancedSpriteManager::GetInstancedSpriteByKey( const std::string& key ) {
	const auto& it = m_instanced_sprites.find( key );
	ASSERT( it != m_instanced_sprites.end(), "sprite actor '" + key + "' not found" );
	return &it->second;
}

InstancedSprite* InstancedSpriteManager::GetRepaintedInstancedSprite( const std::string& name, const InstancedSprite* original, const types::Texture::repaint_rules_t& rules ) {
	const auto& key = name;

	auto it = m_repainted_instanced_sprites.find( key );
	if ( it == m_repainted_instanced_sprites.end() ) {

		Log( "Creating repainted instanced sprite: " + key );

		const auto* original_sprite = original->actor->GetSpriteActor();
		auto* texture = GetRepaintedSourceTexture( name, original_sprite->GetTexture(), rules );

		NEWV(
			sprite,
			scene::actor::Sprite,
			name,
			original_sprite->GetDimensions(),
			texture,
			original_sprite->GetTexCoords()
		);
		sprite->SetRenderFlags( original_sprite->GetRenderFlags() );
		NEWV( instanced, scene::actor::Instanced, sprite );
		instanced->SetZIndex( original->actor->GetZIndex() );
		m_scene->AddActor( instanced );
		it = m_repainted_instanced_sprites.insert(
			{
				key,
				{
					key,
					name,
					original->xy,
					original->wh,
					original->cxy,
					instanced
				}
			}
		).first;

	}
	return &it->second;
}

void InstancedSpriteManager::RemoveInstancedSpriteByKey( const std::string& key ) {
	const auto& it = m_instanced_sprites.find( key );
	ASSERT( it != m_instanced_sprites.end(), "instanced sprite not found: " + key );
	Log( "Removing instanced sprite: " + key );
	const auto& sprite = it->second;
	m_scene->RemoveActor( sprite.actor );
	m_instanced_sprites.erase( it );
}

void InstancedSpriteManager::RemoveRepaintedInstancedSpriteByKey( const std::string& key ) {
	const auto& it = m_repainted_instanced_sprites.find( key );
	ASSERT( it != m_repainted_instanced_sprites.end(), "instanced sprite not found: " + key );
	Log( "Removing instanced sprite: " + key );
	const auto& sprite = it->second;
	m_scene->RemoveActor( sprite.actor );
	m_repainted_instanced_sprites.erase( it );
}

types::Texture* InstancedSpriteManager::GetRepaintedSourceTexture( const std::string& name, const types::Texture* original, const types::Texture::repaint_rules_t& rules ) {
	const auto it = m_repainted_textures.find( name );
	if ( it != m_repainted_textures.end() ) {
		return it->second;
	}
	NEWV( texture, types::Texture, original->m_name, original->m_width, original->m_height );
	texture->RepaintFrom( original, rules );
	m_repainted_textures.insert(
		{
			name,
			texture
		}
	);
	return texture;
}

}
}
