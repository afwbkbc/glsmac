#pragma once

#include <unordered_map>
#include <string>

#include "common/Common.h"

#include "game/frontend/Types.h"
#include "game/backend/map/Types.h"
#include "types/texture/Types.h"
#include "InstancedSprite.h"

namespace types::texture {
class Texture;
}

namespace scene {
class Scene;
}

namespace game {
namespace frontend {
namespace sprite {

CLASS( InstancedSpriteManager, ::common::Class )
public:

	InstancedSpriteManager( scene::Scene* scene );
	~InstancedSpriteManager();

	InstancedSprite* GetInstancedSprite(
		const std::string& name,
		types::texture::Texture* texture,
		const ::game::backend::map::pcx_texture_coordinates_t& src_xy,
		const ::game::backend::map::pcx_texture_coordinates_t& src_wh,
		const ::game::backend::map::pcx_texture_coordinates_t& src_cxy,
		const types::Vec2< float > dst_wh,
		const z_level_t z_level,
		const float z_index_adjustment = 0.0f
	);
	InstancedSprite* GetInstancedSpriteByKey( const std::string& key ); // actor must already exist
	InstancedSprite* GetRepaintedInstancedSprite( const std::string& name, const InstancedSprite* original, const types::texture::repaint_rules_t& rules );
	void RemoveInstancedSpriteByKey( const std::string& key );
	void RemoveRepaintedInstancedSpriteByKey( const std::string& key );

private:

	scene::Scene* m_scene = nullptr;

	std::unordered_map< std::string, InstancedSprite > m_instanced_sprites = {};
	std::unordered_map< std::string, types::texture::Texture* > m_repainted_textures = {};
	std::unordered_map< std::string, InstancedSprite > m_repainted_instanced_sprites = {};

	types::texture::Texture* GetRepaintedSourceTexture( const std::string& name, const types::texture::Texture* original, const types::texture::repaint_rules_t& rules );

};

}
}
}
