#pragma once

#include "base/Base.h"

#include <unordered_map>
#include <string>

#include "InstancedSprite.h"

#include "scene/Scene.h"

namespace task {
namespace game {

CLASS( InstancedSpriteManager, base::Base )
public:

	InstancedSpriteManager( scene::Scene* scene );
	~InstancedSpriteManager();

	InstancedSprite* GetInstancedSprite(
		const std::string& name,
		types::Texture* texture,
		const ::game::map::Consts::pcx_texture_coordinates_t& src_xy,
		const ::game::map::Consts::pcx_texture_coordinates_t& src_wh,
		const ::game::map::Consts::pcx_texture_coordinates_t& src_cxy,
		const types::Vec2< float > dst_xy,
		const float z_index
	);
	InstancedSprite* GetInstancedSpriteByKey( const std::string& key ); // actor must already exist
	InstancedSprite* GetRepaintedInstancedSprite( const std::string& name, const InstancedSprite* original, const types::Texture::repaint_rules_t& rules );
	void RemoveInstancedSpriteByKey( const std::string& key );
	void RemoveRepaintedInstancedSpriteByKey( const std::string& key );

private:

	scene::Scene* m_scene = nullptr;

	std::unordered_map< std::string, InstancedSprite > m_instanced_sprites = {};
	std::unordered_map< std::string, types::Texture* > m_repainted_textures = {};
	std::unordered_map< std::string, InstancedSprite > m_repainted_instanced_sprites = {};

	types::Texture* GetRepaintedSourceTexture( const std::string& name, const types::Texture* original, const types::Texture::repaint_rules_t& rules );

};

}
}
