#pragma once

#include "common/Common.h"

#include <unordered_map>
#include <vector>

#include "types/Vec2.h"
#include "types/Buffer.h"

namespace game::backend::resource {
class Resource;
}

namespace game {
namespace frontend {

class Game;

namespace sprite {
class InstancedSprite;
class InstancedSpriteManager;
}

namespace resource {

CLASS( ResourceManager, common::Class )

	ResourceManager( Game* game );
	~ResourceManager();

	void DefineResource( const backend::resource::Resource* const resource );
	void UndefineResource( const std::string& id, const bool no_erase = false );
	void DefineNoResource( types::Buffer& buf );
	void UndefineNoResource();

	const sprite::InstancedSprite* const GetResourceSprite( const std::string& id, const size_t value ) const;
	const types::Vec2< size_t >& GetResourceMaxWH( const std::string& id ) const;

	const sprite::InstancedSprite* const GetNoResourceSprite() const;

private:
	Game* m_game;
	sprite::InstancedSpriteManager* m_ism;

	struct resource_info_t {
		std::vector< const sprite::InstancedSprite* > sprites = {};
		types::Vec2< size_t > max_wh = {};
	};

	std::unordered_map< std::string, resource_info_t > m_resources = {};
	resource_info_t m_no_resource = {};

};

}
}
}
